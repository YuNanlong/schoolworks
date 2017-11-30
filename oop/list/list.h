//
// Created by yunanlong on 2017/6/11.
//

#ifndef LIST_LIST_H
#define LIST_LIST_H

#include "data.h"
#include <iostream>

class List{
public:
    List()
        :head(0){}

    void insertHead(DataBase *d);
    void insertRear(DataBase *d);
    void insertIndex(DataBase *d, int index);
    void deleteHead();
    void deleteRear();
    void deleteIndex(int index);
    void reverse();
    void clear();
    void merge(List &rhs);

    void print();

    class Node{
    public:
        DataBase *data;
        Node *next;

        Node(){};
        Node(DataBase *d, Node *n = nullptr)
                :data(d), next(n){}
    };
private:
    Node *head;
};


#endif //LIST_LIST_H
