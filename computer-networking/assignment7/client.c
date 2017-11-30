#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <gtk/gtk.h> //Gtk+2.0库

#define PACKAGE_SIZE 4104 //数据包字节数
#define DATA_SIZE 4096 //数据包中包体字节数
#define MESSAGE_SIZE 2048 //发送消息的最大字节数

extern int errno;
int client_socket; //客户端连接的句柄
char server_ip[16]; //服务器ip地址
uint16_t server_port; //服务器端口号
pthread_t tid;
int status = 0; //连接状态

struct ClientRecord{ //客户端信息
    int client_id; //客户端编号
    int connected_socket; //客户端socket连接句柄
    uint16_t sin_port; //客户端端口号
    uint32_t s_addr; //客户端ip地址
    int tid; //负责处理该客户端连接的子线程id
    struct ClientRecord *next;
};

struct Package{ //数据包
    //包头
    int mission_id; //任务代号
    int data_size; //data数组中有效数据的字节数
    //包体
    char data[DATA_SIZE];
};

struct Message{ //发送消息的结构
    int from_client_id; //发送消息的客户端id（由服务器编辑）
    int to_client_id; //接收消息的客户端id
    uint16_t sin_port; //发送消息的客户端端口号（由服务器编辑）
    uint32_t s_addr; //发送消息的客户端ip地址（由服务器编辑）
    char message[MESSAGE_SIZE]; //消息内容
};

gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
void connect_to_server(GtkWidget *widget, GtkWidget *entry[]);
void disconnect_to_server();
void get_time();
void get_server_name();
void get_client_list();
void send_message();
void quit();

void *receive_from_server(void *para);

int main(int argc, char **argv){
    int choice; //用户选项
    pthread_t tid;

    //构件的存储类型为GtkWidget
    GtkWidget *window;
    //顶层表模型
    GtkWidget *top_table; 
    //连接服务器组件
    GtkWidget *connect_button; //连接服务器的按钮
    GtkWidget *disconnect_button; //断开连接的按钮
    GtkWidget *addr_entry[2]; //服务器ip地址和端口的输入框
    //功能组件
    GtkWidget *mission_button[5]; //各种功能的按钮
    GtkWidget *message_entry[2]; //发送消息的输入框
    //标签
    GtkWidget *labels[4];

    gtk_init(&argc, &argv);

    //创建一个窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); 
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL); //关闭窗口的信号槽

    //创建顶层表模型
    top_table = gtk_table_new(5, 4, FALSE);
    gtk_container_add(GTK_CONTAINER(window), top_table);
    gtk_widget_show(top_table);
    
    //创建服务器地址输入框
    addr_entry[0] = gtk_entry_new(); 
    gtk_entry_set_max_length(GTK_ENTRY(addr_entry[0]), 50);
    gtk_table_attach_defaults(GTK_TABLE(top_table), addr_entry[0], 3, 4, 0, 1);
    gtk_widget_show(addr_entry[0]);
    //创建服务器端口输入框
    addr_entry[1] = gtk_entry_new(); 
    gtk_entry_set_max_length(GTK_ENTRY(addr_entry[1]), 10);
    gtk_table_attach_defaults(GTK_TABLE(top_table), addr_entry[1], 3, 4, 1, 2);
    gtk_widget_show(addr_entry[1]);
    //创建连接按钮
    connect_button = gtk_button_new_with_label("连接"); 
    g_signal_connect(G_OBJECT(connect_button), "clicked", G_CALLBACK(connect_to_server), addr_entry);
    gtk_table_attach_defaults(GTK_TABLE(top_table), connect_button, 2, 3, 2, 3);
    gtk_widget_show(connect_button);
    //创建断开连接按钮
    disconnect_button = gtk_button_new_with_label("断开"); 
    g_signal_connect(G_OBJECT(disconnect_button), "clicked", G_CALLBACK(disconnect_to_server), NULL);
    gtk_table_attach_defaults(GTK_TABLE(top_table), disconnect_button, 3, 4, 2, 3);
    gtk_widget_show(disconnect_button);

    //获取时间按钮
    mission_button[0] = gtk_button_new_with_label("获取当前时间"); 
    g_signal_connect(G_OBJECT(mission_button[0]), "clicked", G_CALLBACK(get_time), NULL);
    gtk_table_attach_defaults(GTK_TABLE(top_table), mission_button[0], 0, 1, 3, 4);
    gtk_widget_show(mission_button[0]);
    //获取主机名称按钮
    mission_button[1] = gtk_button_new_with_label("获取主机名称"); 
    g_signal_connect(G_OBJECT(mission_button[1]), "clicked", G_CALLBACK(get_server_name), NULL);
    gtk_table_attach_defaults(GTK_TABLE(top_table), mission_button[1], 1, 2, 3, 4);
    gtk_widget_show(mission_button[1]);
    //获取客户端列表按钮
    mission_button[2] = gtk_button_new_with_label("获取客户端列表"); 
    g_signal_connect(G_OBJECT(mission_button[2]), "clicked", G_CALLBACK(get_client_list), NULL);
    gtk_table_attach_defaults(GTK_TABLE(top_table), mission_button[2], 0, 1, 4, 5);
    gtk_widget_show(mission_button[2]);
    //发送消息按钮
    mission_button[3] = gtk_button_new_with_label("发送消息"); 
    g_signal_connect(G_OBJECT(mission_button[3]), "clicked", G_CALLBACK(send_message), message_entry);
    gtk_table_attach_defaults(GTK_TABLE(top_table), mission_button[3], 0, 2, 2, 3);
    gtk_widget_show(mission_button[3]);
    //退出软件按钮
    mission_button[4] = gtk_button_new_with_label("退出"); 
    g_signal_connect(G_OBJECT(mission_button[4]), "clicked", G_CALLBACK(quit), NULL);
    gtk_table_attach_defaults(GTK_TABLE(top_table), mission_button[4], 1, 2, 4, 5);
    gtk_widget_show(mission_button[4]);

    //创建目标客户端id输入框
    message_entry[0] = gtk_entry_new(); 
    gtk_entry_set_max_length(GTK_ENTRY(message_entry[0]), 10);
    gtk_table_attach_defaults(GTK_TABLE(top_table), message_entry[0], 1, 2, 0, 1);
    gtk_widget_show(message_entry[0]);
    //创建消息输入框
    message_entry[1] = gtk_entry_new(); 
    gtk_entry_set_max_length(GTK_ENTRY(message_entry[1]), 2048);
    gtk_table_attach_defaults(GTK_TABLE(top_table), message_entry[1], 1, 2, 1, 2);
    gtk_widget_show(message_entry[1]);

    //标签
    labels[0] = gtk_label_new("服务器IP地址");
    gtk_table_attach_defaults(GTK_TABLE(top_table), labels[0], 2, 3, 0, 1);
    gtk_widget_show(labels[0]);
    labels[1] = gtk_label_new("服务器端口号");
    gtk_table_attach_defaults(GTK_TABLE(top_table), labels[1], 2, 3, 1, 2);
    gtk_widget_show(labels[1]);
    labels[2] = gtk_label_new("目标客户端编号");
    gtk_table_attach_defaults(GTK_TABLE(top_table), labels[2], 0, 1, 0, 1);
    gtk_widget_show(labels[2]);
    labels[3] = gtk_label_new("消息内容");
    gtk_table_attach_defaults(GTK_TABLE(top_table), labels[3], 0, 1, 1, 2);
    gtk_widget_show(labels[3]);

    gtk_widget_show(window);

    gtk_main();
    
    return 0;
}

//关闭窗口
gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data){
    struct Package send_package;
    
    if(status == 1){
        send_package.mission_id = 5;
        send(client_socket, &send_package, PACKAGE_SIZE, 0);
    }
    return FALSE;
}

//连接服务器
void connect_to_server(GtkWidget *widget, GtkWidget *entry[]){
    const gchar *entry_text;
    struct sockaddr_in server_addr; //存储服务端地址的信息
    int i;
    int length;

    if(status == 1){
        printf("您已经连接到了服务器，无需再次连接\n");
        return;
    }

    //获取输入框中的ip地址和端口号
    entry_text = gtk_entry_get_text(GTK_ENTRY(entry[0]));
    memmove(server_ip, entry_text, 16);
    entry_text = gtk_entry_get_text(GTK_ENTRY(entry[1]));
    length = strlen(entry_text);
    server_port = 0;
    for(i = 0; i < length; i++){
        server_port = server_port * 10 + entry_text[i] - '0';
    }
    //创建socket句柄
    client_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if(client_socket < 0){ //创建句柄失败
        printf("Fail: socket() Code: %d\n", errno);
        return;
    }
    //设置服务端地址信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);
    bzero(&(server_addr.sin_zero), 8);
    //连接到服务端
    if(connect(client_socket, (struct sockaddr *)(&server_addr), sizeof(server_addr)) < 0){ //连接失败
        printf("Fail: connect() Code: %d\n", errno);
        return;
    }
    printf("连接成功\n");
    status = 1; //连接状态标记为已连接

    pthread_create(&tid, NULL, receive_from_server, NULL); //创建接收数据的子线程
}

//断开连接
void disconnect_to_server(){
    struct Package send_package;

    if(status == 0){
        printf("对不起，您尚未连接到服务器，请先连接\n");
        return;
    }

    status = 0; //连接状态标记为未连接
    send_package.mission_id = 5;
    printf("连接已断开\n");
    send(client_socket, &send_package, PACKAGE_SIZE, 0);
}

//请求时间
void get_time(){
    struct Package send_package;

    if(status == 0){
        printf("对不起，您尚未连接到服务器，请先连接\n");
        return;
    }

    send_package.mission_id = 1;
    send(client_socket, &send_package, PACKAGE_SIZE, 0);
}

//请求服务器名称
void get_server_name(){
    struct Package send_package;

    if(status == 0){
        printf("对不起，您尚未连接到服务器，请先连接\n");
        return;
    }

    send_package.mission_id = 2;
    send(client_socket, &send_package, PACKAGE_SIZE, 0);
}

//请求客户端列表
void get_client_list(){
    struct Package send_package;

    if(status == 0){
        printf("对不起，您尚未连接到服务器，请先连接\n");
        return;
    }

    send_package.mission_id = 3;
    send(client_socket, &send_package, PACKAGE_SIZE, 0);
}

//发送消息
void send_message(GtkWidget *widget, GtkWidget *entry[]){
    const gchar *entry_text;
    struct Package send_package;
    struct Message send_message; //发送的消息
    int i;
    int length;

    if(status == 0){
        printf("对不起，您尚未连接到服务器，请先连接\n");
        return;
    }

    send_package.mission_id = 4;
    send_package.data_size = sizeof(struct Message);
    //获取输入框中的目标客户端id和消息
    entry_text = gtk_entry_get_text(GTK_ENTRY(entry[0]));
    length = strlen(entry_text);
    if(length == 0){
        printf("请输入目标客户端编号\n");
        return;
    }
    send_message.to_client_id = 0;
    for(i = 0; i < length; i++){
        send_message.to_client_id = send_message.to_client_id * 10 + entry_text[i] - '0';
    }
    entry_text = gtk_entry_get_text(GTK_ENTRY(entry[1]));
    memmove(send_message.message, entry_text, strlen(entry_text) + 1);
    memmove(send_package.data, &send_message, send_package.data_size);
    send(client_socket, &send_package, PACKAGE_SIZE, 0);
}

//客户端退出
void quit(){
    struct Package send_package;

    if(status == 1){
        send_package.mission_id = 5;
        send(client_socket, &send_package, PACKAGE_SIZE, 0);
    }
    gtk_main_quit();
}

void *receive_from_server(void * para){
    struct Package recv_package;
    int num, i;
    size_t ret, left;
    struct ClientRecord *client_array;
    struct Message *recv_message;
    void *ptr;
    struct in_addr temp_ip;
    int temp = 0;

    while(1){
        left = PACKAGE_SIZE;
        ptr = &recv_package;
        while(left > 0){
            ret = recv(client_socket, &recv_package, left, 0);
            if(ret < 0){
                printf("Fail: recv() Code: %d\n", errno);
            }
            left -= ret;
            ptr = (char *)ptr + ret;
        }
        if(recv_package.mission_id == 1){
            temp++;
            printf("当前时间为: %s, 这是第%d条时间请求\n", ctime((time_t *)(recv_package.data)), temp);
        }
        else if(recv_package.mission_id == 2){
            printf("当前服务器主机名为: %s\n", recv_package.data);
        }
        else if(recv_package.mission_id == 3){
            recv_package.data_size -= sizeof(int); //最后sizeof(int)个字节存储的是当前客户端的id
            client_array = (struct ClientRecord *)malloc(recv_package.data_size);
            memmove(client_array, recv_package.data, recv_package.data_size);
            num = recv_package.data_size / sizeof(struct ClientRecord); //客户端列表的客户端数目
            printf("客户端列表为:\n");
            for(i = 0; i < num; i++){
                printf("第%d个客户端编号为: %d\n", i, client_array[i].client_id);
            }
            free(client_array);
            client_array = NULL;
            printf("您的客户端编号为: %d\n", *(int *)(recv_package.data + recv_package.data_size));
        }
        else if(recv_package.mission_id == 4){
            recv_message = (struct Message *)recv_package.data;
            memcpy(&temp_ip, &recv_message->s_addr, 4); //将长整型ip转为点分十进制ip
            printf("来自编号为%d的客户端的消息，它的ip地址是%s，端口号是%hd\n", recv_message->from_client_id, inet_ntoa(temp_ip), recv_message->sin_port);
            printf("消息内容为: %s\n", recv_message->message);
        }
        else if(recv_package.mission_id == 0){
            printf("消息发送成功\n");
        }
        else if(recv_package.mission_id == -1){
            printf("没有找到客户端\n");
        }
        else if(recv_package.mission_id == -2){
            printf("消息发送失败\n");
        }
        memset(&recv_package, 0, sizeof(struct Package));
    }
}
