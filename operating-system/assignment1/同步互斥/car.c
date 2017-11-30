#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define EAST 1
#define SOUTH 2
#define WEST 3
#define NORTH 4
#define TRUE 1
#define FALSE 0

//队列
struct Queue{
    int head; //队列首节点的下标
    int tail; //队列尾节点的下标
    int size; //队列大小
    int car[1000]; //存储小车标号的数组
    //用于保证同一路口小车的同步顺序
    pthread_mutex_t queue_mutex; //队列互斥锁
    pthread_cond_t queue_cond; //队列条件变量
};

//路口
struct Crossing{
    struct Queue car_queue; //某个方向上的路口的小车队列
    int has_car; //某个方向上的路口当前是否有车
    int current_car; //某个方向上当前的车的id
    pthread_mutex_t first_go_mutex; //用于防止某个方向上小车饥饿的互斥锁
    pthread_cond_t first_go_cond; //用于防止某个方向上小车饥饿的条件变量
};

//空闲路口
struct EmptyCrossing{
    int num; //空闲路口数
    pthread_mutex_t mutex; //互斥锁，用于保证对空闲路口数的操作的原子性
};

//死锁
struct DeadLock{
    int is_deadlock; //当前是否有死锁
    //用于实现线程等待死锁解决时可以被挂起并在死锁解除后被解除阻塞继续运行
    pthread_mutex_t mutex; //互斥锁
    pthread_cond_t cond; //条件变量
};

//传递给子线程的参数
struct Para{
    int car_id; //小车编号
    int direction; //小车所在方向
};

void *go_through_crossing(void *para);
int queue_init(struct Queue *queue, int size);
int queue_isempty(struct Queue *queue);
int queue_isfull(struct Queue *queue);
int queue_front(struct Queue *queue);
int queue_pop(struct Queue *queue);
int queue_push(struct Queue *queue, int car_id);
int crossing_init(struct Crossing *crossing, int size);
int empty_crossing_init(struct EmptyCrossing *empty_crossing);
int deadlock_init(struct DeadLock *deadlock);

struct Crossing east_crossing, south_crossing, west_crossing, north_crossing; //四个方向的路口
pthread_mutex_t quad_a, quad_b, quad_c, quad_d; //路口象限互斥锁
struct EmptyCrossing empty_crossing; //空闲路口数
struct DeadLock deadlock; //死锁

int main(int argc, char *argv[]){
    int num, i, err;
    pthread_t *tids; //存储线程号
    struct Para *paras; //存储传递给子线程的参数
                  //之所以将传递给子线程的参数分开存储在数组的不同元素中，是因为如果通过同一个临时变量来进行对子线程的参数传递，会发生data race
    num = (unsigned int)strlen(argv[1]); //获得小车的总数
    tids = (pthread_t *)malloc(sizeof(pthread_t) * num);
    paras = (struct Para *)malloc(sizeof(struct Para) * num);
    for(i = 0; i < num; i++){
        tids[i] = 0;
        paras[i].car_id = i + 1;
    }
    crossing_init(&east_crossing, num);
    crossing_init(&south_crossing, num);
    crossing_init(&west_crossing, num);
    crossing_init(&north_crossing, num);
    pthread_mutex_init(&quad_a, NULL);
    pthread_mutex_init(&quad_b, NULL);
    pthread_mutex_init(&quad_c, NULL);
    pthread_mutex_init(&quad_d, NULL);
    empty_crossing_init(&empty_crossing);
    deadlock_init(&deadlock);
    for(i = 0; i < num; i++){ //根据输入创建每个小车对应的子线程
        if(argv[1][i] == 'e'){
            paras[i].direction = EAST;
            queue_push(&(east_crossing.car_queue), paras[i].car_id); //将小车编号压入队列
            err = pthread_create(&tids[i], NULL, go_through_crossing, paras + i); //创建小车对应的子线程
            if(err != 0){ //线程创建失败
                printf("ERROR: can not create new thread\n");
                return -1;
            }
        }
        else if(argv[1][i] == 's'){
            paras[i].direction = SOUTH;
            queue_push(&(south_crossing.car_queue), paras[i].car_id);
            err = pthread_create(&tids[i], NULL, go_through_crossing, paras + i);
            if(err != 0){
                printf("ERROR: can not create new thread\n");
                return -1;
            }
        }
        else if(argv[1][i] == 'w'){
            paras[i].direction = WEST;
            queue_push(&(west_crossing.car_queue), paras[i].car_id);
            err = pthread_create(&tids[i], NULL, go_through_crossing, paras + i);
            if(err != 0){
                printf("ERROR: can not create new thread\n");
                return -1;
            }
        }
        else if(argv[1][i] == 'n'){
            paras[i].direction = NORTH;
            queue_push(&(north_crossing.car_queue), paras[i].car_id);
            err = pthread_create(&tids[i], NULL, go_through_crossing, paras + i);
            if(err != 0){
                printf("ERROR: can not create new thread\n");
                return -1;
            }
        }
    }
    //将队首小车作为每个方向上第一个出发的小车，并解除队首小车对应的子线程的阻塞
    if(queue_isempty(&east_crossing.car_queue) == FALSE){
        east_crossing.current_car = queue_front(&(east_crossing.car_queue)); //设置当前路口小车的编号
        queue_pop(&(east_crossing.car_queue)); //将小车编号从队列中移除
        pthread_cond_broadcast(&(east_crossing.car_queue.queue_cond)); //解除当前路口小车对应子线程的阻塞
    }
    if(queue_isempty(&south_crossing.car_queue) == FALSE){
        south_crossing.current_car = queue_front(&(south_crossing.car_queue));
        queue_pop(&(south_crossing.car_queue));
        pthread_cond_broadcast(&(south_crossing.car_queue.queue_cond));
    }
    if(queue_isempty(&west_crossing.car_queue) == FALSE){
        west_crossing.current_car = queue_front(&(west_crossing.car_queue));
        queue_pop(&(west_crossing.car_queue));
        pthread_cond_broadcast(&(west_crossing.car_queue.queue_cond));
    }
    if(queue_isempty(&north_crossing.car_queue) == FALSE){
        north_crossing.current_car = queue_front(&(north_crossing.car_queue));
        queue_pop(&(north_crossing.car_queue));
        pthread_cond_broadcast(&(north_crossing.car_queue.queue_cond));
    }
    //为了防止主线程在子线程退出前结束，这里等待每个子线程退出
    for(i = 0; i < num; i++){
        err = pthread_join(tids[i], NULL);
        if(err != 0){ //子线程退出失败
            printf("ERROR: can not join the thread\n");
            return -1;
        }
    }
    
    return 0;
}

void *go_through_crossing(void *para){
    int direction = ((struct Para *)para)->direction;
    int car_id = ((struct Para *)para)->car_id;
    int result;
    pthread_mutex_t *quad = NULL, *next_quad = NULL;
    struct Crossing *crossing = NULL, *left_crossing = NULL, *right_crossing = NULL;
    char *d = NULL;
    
    if(direction == EAST){
        quad = &quad_b;
        next_quad = &quad_c;
        crossing = &east_crossing;
        left_crossing = &south_crossing;
        right_crossing = &north_crossing;
        d = "East";
    }
    else if(direction == SOUTH){
        quad = &quad_a;
        next_quad = &quad_b;
        crossing = &south_crossing;
        left_crossing = &west_crossing;
        right_crossing = &east_crossing;
        d = "South";
    }
    else if(direction == WEST){
        quad = &quad_d;
        next_quad = &quad_a;
        crossing = &west_crossing;
        left_crossing = &north_crossing;
        right_crossing = &south_crossing;
        d = "West";
    }
    else if(direction == NORTH){
        quad = &quad_c;
        next_quad = &quad_d;
        crossing = &north_crossing;
        left_crossing = &east_crossing;
        right_crossing = &west_crossing;
        d = "North";
    }
    
    //申请互斥锁，使得之后可以通过条件变量的使用来实现小车按队列中先进先出的顺序到达路口
    pthread_mutex_lock(&(crossing->car_queue.queue_mutex));
    //通过条件变量释放互斥锁，阻塞当前线程，直到当前小车编号为当前路口小车编号才解除阻塞，重新获得互斥锁
    while(car_id != crossing->current_car){
        pthread_cond_wait(&(crossing->car_queue.queue_cond), &(crossing->car_queue.queue_mutex));
    }
    //释放互斥锁
    pthread_mutex_unlock(&(crossing->car_queue.queue_mutex));
    
    //小车到达路口
    //pthread_mutex_lock(quad); //对路口前的象限加锁
    crossing->has_car = TRUE; //当前路口标记有车
    printf("car %d from %s arrives at crossing\n", car_id, d);
    //对空闲路口数量的操作必须是原子的，因为本程序通过空闲路口数量判断是否死锁，如果同时有多个线程对空闲路口数量进行操作，
    //并且使得空闲路口数量最终为0，那么将会有多个线程判断死锁是由自己造成的，同时进行处理死锁的相关操作，
    //根据本程序的死锁解决算法，多个线程同时进行死锁处理会导致错误，所以需要通过互斥锁来保证操作的原子性
    pthread_mutex_lock(&(empty_crossing.mutex));
    empty_crossing.num -= 1; //将空闲路口数量减一
    result = empty_crossing.num;
    pthread_mutex_unlock(&(empty_crossing.mutex));
    
    //解决死锁
    if(result == 0){
        if(direction == EAST){
            printf("DEADLOCK: car jam detected, signalling South to go\n");
        }
        else if(direction == SOUTH){
            printf("DEADLOCK: car jam detected, signalling West to go\n");
        }
        else if(direction == WEST){
            printf("DEADLOCK: car jam detected, signalling North to go\n");
        }
        else if(direction == NORTH){
            printf("DEADLOCK: car jam detected, signalling East to go\n");
        }
        deadlock.is_deadlock = TRUE; //标记死锁的发生
        //最后进入路口造成死锁的小车让出路口资源（互斥锁和has_car标记）
        //如果剩下三辆小车中最右侧的小车的线程已经运行到了请求直行路线上第二个象限的互斥锁的阶段，
        //那么让出路口资源后剩下三辆小车中最右侧的小车的线程就能继续运行
        //pthread_mutex_unlock(quad);
        crossing->has_car = FALSE;
        //让剩下三辆小车中最右侧的小车先通行，解除其阻塞
        //如果剩下三辆小车中最右侧的小车的线程因为等待右侧小车先行而被阻塞，
        //那么这个语句可以解除其阻塞
        pthread_cond_signal(&(left_crossing->first_go_cond));
        //申请互斥锁后通过条件变量阻塞当前进程，等待死锁解除后解除阻塞
        //当前线程需要通过互斥锁和条件变量，使得其在死锁解决后可以被解除阻塞继续运行
        pthread_mutex_lock(&(deadlock.mutex));
        while(deadlock.is_deadlock == TRUE){
            pthread_cond_wait(&(deadlock.cond), &(deadlock.mutex));
        }
        pthread_mutex_unlock(&(deadlock.mutex));
        //死锁解除，当前线程解除阻塞，当前方向小车重新获得路口资源（互斥锁和has_car标记）
        //pthread_mutex_lock(quad);
        crossing->has_car =TRUE;
    }
    
    //等待右侧小车先行
    pthread_mutex_lock(&(crossing->first_go_mutex)); //申请互斥锁，以便之后通过条件变量实现右侧先行的同步顺序
    while(right_crossing->has_car == TRUE){ //如果右侧有车，线程将被阻塞，直到右侧车通过后解除阻塞
        pthread_cond_wait(&(crossing->first_go_cond), &(crossing->first_go_mutex));
    }
    pthread_mutex_unlock(&(crossing->first_go_mutex));
    
    //通过路口
    pthread_mutex_lock(next_quad); //获取直行通过的下一个象限的互斥锁
    //pthread_mutex_unlock(quad); //释放直行通过的第一个象限的互斥锁
    printf("car %d from %s leaving crossing\n", car_id, d);
    //空闲路口加一
    //空闲路口加一的操作必须在释放直行通过的下一个象限的互斥锁之前
    //否则如果先释放了直行通过的下一个象限的互斥锁，而这个时候恰好右侧有车到达申请了互斥锁并对空闲路口减一
    //那么此时空闲路口数可能为0，判断为死锁，但实际空闲路口数为1，没有死锁
    pthread_mutex_lock(&(empty_crossing.mutex));
    empty_crossing.num += 1;
    pthread_mutex_unlock(&(empty_crossing.mutex));
    pthread_mutex_unlock(next_quad); //释放直行通过的下一个象限的互斥锁
    deadlock.is_deadlock = FALSE; //标记死锁已经被解除
    pthread_cond_signal(&(deadlock.cond)); //解除为了解决死锁而让出路口资源的线程的阻塞
    crossing->has_car = FALSE; //当前路口标记无车
    pthread_cond_signal(&(left_crossing->first_go_cond)); //解除下一个将要通行的小车的阻塞
    if(queue_isempty(&crossing->car_queue) == FALSE){
        crossing->current_car = queue_front(&(crossing->car_queue)); //获取接下来将要到达路口的小车的下标
        queue_pop(&(crossing->car_queue));
        pthread_cond_broadcast(&(crossing->car_queue.queue_cond)); //将接下来将要到达路口的小车的线程解除阻塞
    }
    return NULL;
}

//初始化队列
int queue_init(struct Queue *queue, int size){
    queue->head = 0;
    queue->tail = 0;
    queue->size = size;
    if(pthread_mutex_init(&(queue->queue_mutex), NULL) != 0){
        //互斥锁初始化失败
        printf("ERROR: can not init the mutex of queue\n");
        return -1;
    }
    if(pthread_cond_init(&(queue->queue_cond), NULL) != 0){
        //条件变量初始化失败
        printf("ERROR: can not init the cond of queue\n");
        return -1;
    }
    return 0;
}

//判断队列是否为空
int queue_isempty(struct Queue *queue){
    if(queue->head < queue->tail){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

//判断队列是否已满
int queue_isfull(struct Queue *queue){
    if(queue->tail < queue->size){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

//获得队列的首元素
int queue_front(struct Queue *queue){
    if(queue_isempty(queue) == TRUE){ //队列已空
        printf("ERROR: the queue is empty\n");
        return -1;
    }
    return queue->car[queue->head];
}

//首元素出队列
int queue_pop(struct Queue *queue){
    if(queue_isempty(queue) == TRUE){ //队列已空
        printf("ERROR: the queue is empty\n");
        return -1;
    }
    queue->head += 1;
    return 0;
}

//入队列
int queue_push(struct Queue *queue, int car_id){
    if(queue_isfull(queue) == TRUE){ //队列已满
        printf("ERROR: the queue is full\n");
        return -1;
    }
    queue->car[queue->tail] = car_id;
    queue->tail += 1;
    return 0;
}

//初始化路口
int crossing_init(struct Crossing *crossing, int size){
    if(queue_init(&(crossing->car_queue), size) != 0){
        return -1;
    }
    crossing->has_car = FALSE; //初始状态没有车
    crossing->current_car = 0;
    if(pthread_mutex_init(&(crossing->first_go_mutex), NULL) != 0){
        //互斥锁初始化失败
        printf("ERROR: can not init the first go mutex of crossing\n");
        return -1;
    }
    if(pthread_cond_init(&(crossing->first_go_cond), NULL) != 0){
        //条件变量初始化失败
        printf("ERROR: can not init the first go cond of crossing\n");
        return -1;
    }
    return 0;
}

//初始化空闲路口
int empty_crossing_init(struct EmptyCrossing *empty_crossing){
    empty_crossing->num = 4; //初始状态所有路口均为空闲
    if(pthread_mutex_init(&(empty_crossing->mutex), NULL) != 0){
        //互斥锁初始化失败
        printf("ERROR: can not init the mutex of the empty crossing\n");
        return -1;
    }
    return 0;
}

//初始化死锁
int deadlock_init(struct DeadLock *deadlock){
    deadlock->is_deadlock = FALSE; //初始状态没有死锁
    if(pthread_mutex_init(&(deadlock->mutex), NULL) != 0){
        //互斥锁初始化失败
        printf("ERROR: can not init the mutex of deadlock\n");
        return -1;
    }
    if(pthread_cond_init(&(deadlock->cond), NULL) != 0){
        //条件变量初始化失败
        printf("ERROR: can not init the cond of deadlock\n");
        return -1;
    }
    return 0;
}
