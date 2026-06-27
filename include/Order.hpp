#pragma once

#include<User.hpp>
#include<Train.hpp>

#pragma pack(1)
class Order
{
public:
    static const int memory_size=35+25+35+35+4*9;

    char username[35],train_id[25],s_station[35],t_station[35];
    int tag,depar_date,s_date,s_time,t_date,t_time,price,count,status;

    inline Order();
    inline Order(const std::string &_username,const std::string &_train_id,const std::string &_s_station
         ,const std::string &_t_station,int _tag,int _depar_date,int _s_date,int _s_time,int _t_date
         ,int _t_time,int _price,int _count,int _status);
};
class OrderHashByUsernameAndTag
{
public:
    inline HashResult operator () (const Order &a);
};

class SemiOrder
{
public:
    static const int memory_size=25+35+4*2;

    char train_id[25],username[35];
    int depar_date,tag;

    inline SemiOrder();
    inline SemiOrder(const std::string &_username,const std::string &_train_id,int _depar_date,int _tag);
};
class SOHashByTrainidAndDeparDate
{
public:
    inline HashResult operator () (const SemiOrder &a);
};

BPlusTree<Order,OrderHashByUsernameAndTag,80,1000> Orders("Orders");
BPlusTree<SemiOrder,SOHashByTrainidAndDeparDate,80,1000> Pend("Pend");

inline void queryOrder(const std::string &username);
inline void buyTicket(const std::string &username,const std::string &train_id,const std::string &S
              ,const std::string &T,int date,int count,bool allow_queue,int tag);
inline void refundTicket(const std::string &username,int n);

#include<Order_implement.hpp>