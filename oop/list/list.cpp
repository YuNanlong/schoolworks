//
// Created by yunanlong on 2017/6/11.
//

#include "list.h"

using namespace std;

void List::insertHead(DataBase *d) {
    Node *newNode = new Node(d, head);
    head = newNode;
}

void List::insertRear(DataBase *d) {
    if(head == nullptr){
        insertHead(d);
    }
    else{
        Node *pHead = head;
        while(pHead->next != nullptr){
            pHead = pHead->next;
        }
        pHead->next = new Node(d);
    }
}

void List::insertIndex(DataBase *d, int index) {
    if(index < 0){
        cout << "Out of range" << endl;
        return ;
    }
    else if(index == 0){
        insertHead(d);
    }
    else{
        Node *pHead = head;
        for(int i = 0; i < index - 1; i++){
            if(pHead == nullptr){
                cout << "Out of range" << endl;
                return ;
            }
            pHead = pHead->next;
        }
        pHead->next = new Node(d, pHead->next);
    }
}

void List::deleteHead() {
    if(head != nullptr){
        Node *temp = head;
        head = head->next;
        delete temp;
    }
    else{
        cout << "Empty list" << endl;
        return ;
    }
}

void List::deleteRear() {
    if(head != nullptr){
        if(head->next != nullptr){
            Node *pHead = head;
            while(pHead->next->next != nullptr){
                pHead = pHead->next;
            }
            Node *temp = pHead->next;
            pHead->next = nullptr;
            delete temp;
        }
        else{
            deleteHead();
        }
    }
    else{
        cout << "Empty list" << endl;
        return ;
    }
}

void List::deleteIndex(int index) {
    if(head != nullptr){
        if(index < 0){
            cout << "Out of range" << endl;
            return ;
        }
        else if(index == 0){
            deleteHead();
        }
        else{
            Node *pHead = head;
            if(pHead->next == nullptr){
                cout << "Out of range" << endl;
                return ;
            }
            for(int i = 0; i < index - 1; i++){
                if(pHead->next == nullptr){
                    cout << "Out of range" << endl;
                    return ;
                }
                pHead = pHead->next;
            }
            Node *temp = pHead->next;
            pHead->next = pHead->next->next;
            delete temp;
        }
    }
    else{
        cout << "Empty list" << endl;
        return ;
    }
}

void List::reverse() {
    Node *pHead = head, *temp;
    head = nullptr;
    while(pHead != nullptr){
        temp = pHead;
        pHead = pHead->next;
        temp->next = head;
        head = temp;
    }
}

void List::clear() {
    while(head != nullptr){
        deleteHead();
    }
}

void List::merge(List &rhs) {
    if(head == nullptr){
        head = rhs.head;
        rhs.head = nullptr;
    }
    else if(rhs.head == nullptr){
        return ;
    }
    else{
        Node *pHead = head;
        while(pHead->next != nullptr){
            pHead = pHead->next;
        }
        pHead->next = rhs.head;
        rhs.head = nullptr;
    }
}

void List::print(){
    int flag = 0;
    Node *pHead = head;
    if(pHead == nullptr){
        cout << "异质链表为空" << endl;
        return ;
    }
    while(pHead != nullptr){
        if(flag == 0){
            flag = 1;
        }
        else{
            cout << " -> ";
        }
        pHead->data->print();
        pHead = pHead->next;
    }
    cout << endl;
}
