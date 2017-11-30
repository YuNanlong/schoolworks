#include <iostream>
#include "list.h"

using namespace std;

int main() {
    DataBase *pData;

    cout << "初始化操作测试" << endl;
    cout << "初始化链表l1和l2" << endl;
    List l1, l2;
    cout << endl;

    cout << "插入操作测试" << endl;
    cout << "向l1的头部中插入1" << endl;
    pData = new Data<int>(1, INT);
    l1.insertHead(pData);
    cout << "向l1的头部中插入2" << endl;
    pData = new Data<int>(2, INT);
    l1.insertHead(pData);
    cout << "向l1的尾部中插入3.3" << endl;
    pData = new Data<float>(3.3, FLOAT);
    l1.insertRear(pData);
    cout << "向l1的第2个位置插入'hahaha'" << endl;
    pData = new Data<string>("hahaha", STRING);
    l1.insertIndex(pData, 2);
    cout << "向l1的第6个位置插入'heiheihei'" << endl;
    pData = new Data<string>("heiheihei", STRING);
    l1.insertIndex(pData, 6);
    cout << "向l1的第0个位置插入1.2" << endl;
    pData = new Data<float>(1.2, FLOAT);
    l1.insertIndex(pData, 0);
    cout << "输出l1" << endl;
    l1.print();
    cout << endl;

    cout << "向l2的头部中插入1000" << endl;
    pData = new Data<int>(1000, INT);
    l2.insertHead(pData);
    cout << "向l2的尾部中插入95.9" << endl;
    pData = new Data<float>(95.9, FLOAT);
    l2.insertRear(pData);
    cout << "向l2的第1个位置插入'hohoho'" << endl;
    pData = new Data<string>("hohoho", STRING);
    l2.insertIndex(pData, 1);
    cout << "输出l2" << endl;
    l2.print();
    cout << endl;

    cout << "倒置操作测试" << endl;
    cout << "倒置l1" << endl;
    l1.reverse();
    cout << "输出l1" << endl;
    l1.print();
    cout << endl;

    cout << "合并操作测试" << endl;
    cout << "合并l1和l2，以l1作为新的头指针" << endl;
    l1.merge(l2);
    cout << "输出l1" << endl;
    l1.print();
    cout << endl;

    cout << "删除操作测试" << endl;
    cout << "删除l1头结点" << endl;
    l1.deleteHead();
    cout << "删除l1尾节点" << endl;
    l1.deleteRear();
    cout << "删除l1第2个位置的节点" << endl;
    l1.deleteIndex(2);
    cout << "删除l1第6个位置的节点" << endl;
    l1.deleteIndex(6);
    cout << "输出l1" << endl;
    l1.print();
    cout << endl;

    cout << "清空操作测试" << endl;
    cout << "清空l1和l2" << endl;
    l1.clear();
    l2.clear();
    cout << "输出l1" << endl;
    l1.print();
    return 0;
}