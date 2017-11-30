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

#define SERVER_PORT 2238 //端口号
#define PACKAGE_SIZE 4104 //数据包字节数
#define DATA_SIZE 4096 //数据包中包体字节数
#define MESSAGE_SIZE 2048 //发送消息的最大字节数

extern int errno;

struct Package{ //数据包
    //包头
    int mission_id; //任务代号
    int data_size; //data数组中有效数据的字节数
    //包体
    char data[DATA_SIZE];
};

struct Message{ //发送消息的结构
    int from_client_id; //发送消息的客户端id
    int to_client_id; //接收消息的客户端id
    uint16_t sin_port; //发送消息的客户端端口号（由服务器编辑）
    uint32_t s_addr; //发送消息的客户端ip地址（由服务器编辑）
    char message[MESSAGE_SIZE]; //消息内容
};

struct ClientRecord{ //客户端信息
    int client_id; //客户端编号
    int connected_socket; //客户端socket连接句柄
    uint16_t sin_port; //客户端端口号
    uint32_t s_addr; //客户端ip地址
    pthread_t tid; //负责处理该客户端连接的子线程id
    struct ClientRecord *next;
};

struct ClientList{ //客户端列表
    struct ClientRecord *head, *tail;
    int client_num; //已连接的客户端数量
    int id_counter; //客户端编号计数器，用于记录客户端编号已经编到了哪个数
};

struct ClientList client_list; //当前连接的客户端列表
pthread_mutex_t mutex_list;

void *service(void *para);
void init_client_list(struct ClientList *client_list);
struct ClientRecord *init_client_record(int connected_socket, struct sockaddr_in *client_addr);
void add_client(struct ClientList *client_list, struct ClientRecord *client_record);
void drop_client(struct ClientList *client_list, struct ClientRecord *client_record);
struct ClientRecord *search_client(int client_id);

int main(int argc, char **argv){
    int listening_socket, connected_socket; //服务端侦听句柄和连接句柄
    socklen_t client_length;
    struct sockaddr_in client_addr, server_addr; //客户端地址信息和服务端地址信息
    struct ClientRecord *client_record;

    pthread_mutex_init(&mutex_list, NULL);
    //创建侦听句柄
    listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(listening_socket == -1){
        printf("Fail: socket() Code: %d\n", errno);
        return -1;
    }
    //设置服务端地址信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    bzero(&(server_addr.sin_zero), 8);
    //绑定服务端地址
    if(bind(listening_socket, (struct sockaddr *)(&server_addr), sizeof(server_addr)) == -1){
        printf("Fail: bind() Code: %d\n", errno);
        close(listening_socket);
        return -1;
    }
    //侦听客户端连接请求
    if(listen(listening_socket, 6) == -1){
        printf("Fail: listen() Code: %d\n", errno);
        close(listening_socket);
        return -1;
    }
    printf("服务器运行中\n等待连接\nCtrl+C终止运行\n");

    init_client_list(&client_list); //初始化客户端列表
    while(1){
        connected_socket = accept(listening_socket, (struct sockaddr *)(&client_addr), &client_length); //接收客户端连接请求
        if(connected_socket == -1){ //创建连接句柄失败
            printf("Fail: accept Code: %d\n", errno);
        }
        else{
            pthread_mutex_lock(&mutex_list);
            client_record = init_client_record(connected_socket, &client_addr); //初始化客户端信息
            add_client(&client_list, client_record); //将客户端信息添加进客户端列表
            printf("客户端%d连接成功\n", client_record->client_id);
            pthread_create(&(client_record->tid), NULL, service, (void *)(&(client_record->client_id))); //创建子线程
            pthread_mutex_unlock(&mutex_list);
        }
    }
}

//子线程调用的函数
void *service(void *para){
    int client_id = *(int *)para; //获取当前连接的客户端id
    size_t ret;
    struct Package recv_package, send_package; //接收的数据包和发送的数据包
    int connected_socket; //客户端连接句柄
    struct ClientRecord *client_array, *client_record, *to_client, *ptr;
    int i;
    FILE *fp;
    time_t current_time;

    client_record = search_client(client_id); //根据id获取当前连接的客户端信息
    if(client_record == NULL){ //没有找到客户端
        printf("Client %d does not exist", client_id);
        pthread_exit(NULL);
    }
    connected_socket = client_record->connected_socket; //获取连接句柄

    while(1){
        ret = recv(connected_socket, &recv_package, PACKAGE_SIZE, 0);
        if(ret > 0){
            memset(&send_package, 0, sizeof(send_package));
            //printf("recv_package.mission_id = %d\n", recv_package.mission_id);
            if(recv_package.mission_id == 1){ //请求时间
                send_package.mission_id = 1;
                send_package.data_size = sizeof(time_t);
                current_time = time(NULL);
                memmove(send_package.data, &current_time, send_package.data_size);
                send(connected_socket, &send_package, PACKAGE_SIZE, 0);
                //memset(&send_package, 0, sizeof(send_package));
                printf("客户端%d请求获取时间, 相关信息已送达\n", client_id);
            }
            else if(recv_package.mission_id == 2){ //请求服务器名称
                send_package.mission_id = 2;
                send_package.data_size = 256;
                fp = fopen("/etc/hostname", "r");
                fgets(send_package.data, 256, fp);
                send(connected_socket, &send_package, PACKAGE_SIZE, 0);
                printf("客户端%d请求获取服务器名称, 相关信息已送达\n", client_id);
                //memset(&send_package, 0, sizeof(send_package));
            }
            else if(recv_package.mission_id == 3){ //请求客户端连接列表
                //将客户端列表写入数据包
                send_package.mission_id = 3;
                send_package.data_size = sizeof(struct ClientRecord) * client_list.client_num;
                client_array = (struct ClientRecord *)malloc(send_package.data_size);
                memset(client_array, 0, send_package.data_size);
                ptr = client_list.head;
                for(i = 0; ptr != NULL; i++){
                    client_array[i].client_id = ptr->client_id;
                    client_array[i].sin_port = ptr->sin_port;
                    client_array[i].s_addr = ptr->s_addr;
                    ptr = ptr->next;
                }
                memmove(send_package.data, client_array, send_package.data_size);
                //将当前客户端id写入数据包
                *(int *)(send_package.data + send_package.data_size) = client_record->client_id;
                send_package.data_size += sizeof(int);
                send(connected_socket, &send_package, PACKAGE_SIZE, 0);
                printf("客户端%d请求获取客户端连接列表, 相关信息已送达\n", client_id);
                free(client_array);
                client_array = NULL;
                //memset(&send_package, 0, sizeof(send_package));
            }
            else if(recv_package.mission_id == 4){ //发送消息
                pthread_mutex_lock(&mutex_list);
                ((struct Message *)recv_package.data)->s_addr = client_record->s_addr;
                //((struct Message *)recv_package.data)->sin_port = client_record->sin_port;
                ((struct Message *)recv_package.data)->from_client_id = client_record->client_id;
                to_client = search_client(((struct Message *)recv_package.data)->to_client_id);
                if(to_client != NULL){
                    if(send(to_client->connected_socket, &recv_package, PACKAGE_SIZE, 0) > 0){
                        send_package.mission_id = 0; //发送消息成功，返回的结果数据包mission_id为0
                        printf("客户端%d请求向客户端%d发送信息, 相关信息已送达\n", client_id, to_client->client_id);
                    }
                    else{
                        send_package.mission_id = -2; //发送消息失败，返回的结果数据包mission_id为1
                    }
                }
                else{
                    send_package.mission_id = -1; //没有找到接收客户端，返回的结果数据包mission_id为2
                }
                send_package.data_size = sizeof(int);
                send(connected_socket, &send_package, PACKAGE_SIZE, 0);
                pthread_mutex_unlock(&mutex_list);
            }
            else if(recv_package.mission_id == 5){ //关闭连接
                printf("客户端%d已断开连接\n", client_record->client_id);
                pthread_mutex_lock(&mutex_list);
                drop_client(&client_list, client_record);
                client_record = NULL;
                close(connected_socket);
                pthread_mutex_unlock(&mutex_list);
                pthread_exit(NULL);
            }
        }
        else if(ret == 0){
            continue;
        }
        else{
            printf("Fail: recv() Code: %d\n", errno);
        }
    }
}

//初始化客户端列表
void init_client_list(struct ClientList *client_list){
    client_list->head = client_list->tail = NULL;
    client_list->client_num = 0;
    client_list->id_counter = 0;
}

//初始化客户端信息
struct ClientRecord *init_client_record(int connected_socket, struct sockaddr_in *client_addr){
    struct ClientRecord *client_record = (struct ClientRecord *)malloc(sizeof(struct ClientRecord));
    client_record->connected_socket = connected_socket;
    client_record->sin_port = client_addr->sin_port;
    client_record->s_addr = client_addr->sin_addr.s_addr;
    client_record->next = NULL;
    return client_record;
}

//将客户端信息添加进客户端列表
void add_client(struct ClientList *client_list, struct ClientRecord *client_record){
    client_record->client_id = client_list->id_counter;
    client_list->id_counter += 1;
    if(client_list->client_num == 0){
        client_list->head = client_list->tail = client_record;
    }
    else{
        client_list->tail->next = client_record;
        client_list->tail = client_record;
    }
    client_list->client_num += 1;
}

//将客户端信息从客户端列表移除
void drop_client(struct ClientList *client_list, struct ClientRecord *client_record){
    struct ClientRecord *temp_ptr;
    if(client_list->head == client_record){
        if(client_list->tail == client_record){
            client_list->head = client_list->tail = NULL;
        }
        else{
            client_list->head = client_record->next;
        }
    }
    else{
        temp_ptr = client_list->head;
        while(temp_ptr->next != client_record){
            temp_ptr = temp_ptr->next;
        }
        temp_ptr->next = client_record->next;
        if(client_list->tail == client_record){
            client_list->tail = temp_ptr;
        }
    }
    free(client_record);
    client_list->client_num -= 1;
}

//在客户端列表中搜索客户端
struct ClientRecord *search_client(int client_id){
    struct ClientRecord *ptr = client_list.head;
    while(ptr != NULL){
        if(ptr->client_id == client_id){
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}
