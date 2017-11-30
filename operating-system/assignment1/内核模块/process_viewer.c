#include <stdlib.h> //使用system库函数必须的库
int main(){
    system("cat /proc/process-list"); //通过system库函数调用fork()产生子进程
	                                  //由子进程来调用/bin/sh -c来执行c参数所代表的命令
	return 0; //main函数返回0
}

