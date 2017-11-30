#include <linux/module.h> //内核模块必须头文件
#include <linux/kernel.h> //内核模块必须头文件
#include <linux/sched.h> //定义了struct task_struct头文件
#include <linux/proc_fs.h> //创建/proc
#include <linux/seq_file.h> //使用seq_file必要的头文件

int user_open(struct inode *inode, struct file *file); //在使用seq_file时用户自定义的open函数
int user_show(struct seq_file *m, void *p); //在使用seq_file时用户自定义的show函数

/* 
 * 依照文档https://www.kernel.org/doc/Documentation/filesystems/seq_file.txt
 * 在使用seq_file的时候对struct file_opreations进行必要的设置
 */
static const struct file_operations seq_file_ops = {
    .open = user_open, //user_open函数由用户自定义
    .read = seq_read, //seq_file.h提供的库函数
    .llseek = seq_lseek, //seq_file.h提供的库函数
    .release = single_release, //seq_file.h提供的库函数，因为user_open函数中使用了single_open
                               //所以release函数必须为single_release而不是seq_release，否则会造成内存泄漏
};

//在使用seq_file时用户自定义的open函数
int user_open(struct inode *inode, struct file *file){
    /*
     * 依照文档https://www.kernel.org/doc/Documentation/filesystems/seq_file.txt
     * seq_file的简单用法中自定义的open函数只需要调用single_open函数即可
     */
    return single_open(file, user_show, NULL);
}

//在使用seq_file时用户自定义的show函数
int user_show(struct seq_file *file, void *p){
    int count[8]; //用于计数不同状态的进程的数组
                  //count[0] -- 状态是TASK_RUNNING的进程数
                  //count[1] -- 状态是TASK_INTERRUPTIBLE的进程数
                  //count[2] -- 状态是TASK_UNINTERRUPTIBLE的进程数
                  //count[3] -- 状态是TASK_STOPPED的进程数
                  //count[4] -- 状态是TASK_TRACED的进程数
                  //count[5] -- 状态是EXIT_ZOMBIE的进程数
                  //count[6] -- 状态是EXIT_DEAD的进程数
                  //count[7] -- 总进程数
    int temp_exit_state, temp_state; //用于保存进程的exit_state变量和state变量的值的临时变量
    struct task_struct *ptr; //指向tast_struct的指针
    int i; //循环变量

    for(i = 0; i < 8; i++){ //初始化count数组
        count[i] = 0; //将count数组的每一个元素初始化为0
    }
    for(ptr = &init_task; (ptr = next_task(ptr)) != &init_task; ){ //从init_task开始遍历task_struct的链表
        count[7] += 1; //总进程数加一
        seq_printf(file, "Name: %s Pid: %d State: %ld ParentName: %s\n", ptr->comm, ptr->pid, ptr->state, ptr->real_parent->comm); //输出进程的相应信息
        temp_state = ptr->state; //记录当前进程的state
        temp_exit_state = ptr->exit_state; //记录当前进程的exit_state
        if(temp_exit_state == 0){ //进程没有退出
            if(temp_state == TASK_RUNNING){ //状态是TASK_RUNNING
                count[0] += 1; //数组相应元素加一
            }
            else if(temp_state == TASK_INTERRUPTIBLE){ //状态是TASK_INTERRUPTIBLE
                count[1] += 1; //数组相应元素加一
            }
            else if(temp_state == TASK_UNINTERRUPTIBLE){ //状态是TASK_UNINTERRUPTIBLE
                count[2] += 1; //数组相应元素加一
            }
            else if(temp_state == TASK_STOPPED){ //状态是TASK_STOPPED
                count[3] += 1; //数组相应元素加一
            }
            else if(temp_state == TASK_TRACED){ //状态是TASK_TRACED
                count[4] += 1; //数组相应元素加一
            }
        }
        else{ //进程已经退出
            if(temp_exit_state == EXIT_ZOMBIE){ //状态是EXIT_ZOMBIE
                count[5] += 1; //数组相应元素加一
            }
            else if(temp_exit_state == EXIT_DEAD){ //状态是EXIT_DEAD
                count[6] += 1; //数组相应元素加一
            }
        }
    }
    seq_printf(file, "total tasks: %d\n", count[7]); //打印总进程数
    seq_printf(file, "TASK_RUNNING: %d\n", count[0]); //打印状态是TASK_RUNNING的进程数
    seq_printf(file, "TASK_INTERRUPTIBLE: %d\n", count[1]); //打印状态是TASK_INTERRUPTIBLE的进程数
    seq_printf(file, "TASK_UNINTERRUPTIBLE: %d\n", count[2]); //打印状态是TASK_UNINTERRUPTIBLE的进程数
    seq_printf(file, "TASK_STOPPED: %d\n", count[3]); //打印状态是TASK_STOPPED的进程数
    seq_printf(file, "TASK_TRACED: %d\n", count[4]); //打印状态是TASK_TRACED的进程数
    seq_printf(file, "EXIT_ZOMBIE: %d\n", count[5]); //打印状态是EXIT_ZOMBIE的进程数
    seq_printf(file, "EXIT_DEAD: %d\n", count[6]); //打印状态是EXIT_DEAD的进程数
    
    return 0; //函数返回0
}

//模块编程中必须的函数，向内核注册模块所提供的新功能
int init_module(void){
    struct proc_dir_entry *file; //指向在/proc中新创建的文件的指针

    printk(KERN_INFO "visit_process module started\n"); //以信息型消息KERN_INFO的日志级别在内核日志中打印提示消息
    file = proc_create("process-list", 0755, NULL, &seq_file_ops); //在/proc文件系统的根目录下创建名为process-list的文件
                                                                                          //文件权限为0755
                                                                                          //在/proc文件系统的根目录下创建文件，parent参数为NULL
                                                                                          //该文件的操作函数为seq_file_ops结构中定义的函数
    if(file == NULL){ //如果文件创建失败，proc_create函数返回NULL
        return -1; //函数返回-1
    }   
    return 0; //函数返回0
}

//模块编程中必须的函数，负责注销所有由模块注册的功能
void cleanup_module(void){ 
    remove_proc_entry("process-list", NULL); //在/proc文件系统的根目录下删除名为process-list的文件
    printk(KERN_INFO "visit_process module finished\n"); //以信息型消息KERN_INFO的日志级别在内核日志中打印提示消息
}

MODULE_LICENSE("GPL"); //代码遵循GPL协议
