//
// Created by yunanlong on 2017/6/11.
//

#ifndef LIST_NODE_H
#define LIST_NODE_H

#include <iostream>

enum DataType{INT, FLOAT, STRING};

class DataBase{
public:
    DataType type;
    DataBase(DataType t)
            :type(t){}
    virtual ~DataBase(){}
    virtual void print() = 0;
};

template <typename T>
class Data: public DataBase{
private:
    T val;
public:
    Data(T v, DataType t)
            :val(v), DataBase(t){}
    virtual ~Data(){}
    virtual void print(){
        std::cout << val;
    }
};

#endif //LIST_NODE_H
