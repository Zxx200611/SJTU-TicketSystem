#pragma once

#include<BPlusTree.hpp>
#include<Utilities.hpp>
#include<Vector.hpp>
#include<iostream>

#pragma pack(1)
class Train
{
public:
    static const int max_station_cnt=35;
    static const int memory_size=25+max_station_cnt*35+4*(103*max_station_cnt+5)+1+1;
    char train_id[25],stations[max_station_cnt][35];
    int station_num,seat_num,seats[100][max_station_cnt],prices[max_station_cnt];
    int departure,travel_times[max_station_cnt],stop_times[max_station_cnt];
    int sale_start,sale_end;
    char type;
    bool is_released;

    inline Train();
    inline Train(const std::string &_train_id,int _station_num,int _seat_num,const std::string &_stations
         ,const std::string &_prices,int _departure,const std::string &_travel_times
         ,const std::string &_stop_times,const std::string _sale_date,char _type);
};
class TrainHashByIdAndStationNum
{
public:
    inline HashResult operator () (const Train &a);
};

class StationTimeTrain
{
public:
    static const int memory_size=35+25+4*7;
    char station[35],train_id[25];
    int date,time,start_date,start_time,sta_id,s_time,s_price;

    inline StationTimeTrain();
    inline StationTimeTrain(const std::string &_station,int _date,int _time,const std::string&_train_id
                    ,int _start_date,int _start_time,int _sta_id,int _s_time,int _s_price);
};
class STTHaashByStationAndTime
{
public:
    HashResult operator () (const StationTimeTrain &a);
};
class QueryTicketResult
{
public:
    char train_id[25],s[35],t[35];
    int s_date,s_time,t_date,t_time;
    int time,price,seat;

    inline QueryTicketResult();
    inline QueryTicketResult(const std::string &_train_id,const std::string &_s,const std::string &_t
                     ,int _s_date,int _s_time,int _t_date,int _t_time,int _time,int _price,int _seat);
};
class QueryTransferResult
{
public:
    QueryTicketResult ta,tb;

    inline QueryTransferResult();
    inline QueryTransferResult(const QueryTicketResult &a,const QueryTicketResult &b);
};

BPlusTree<Train,TrainHashByIdAndStationNum,80,1000> Trains("Trains");
BPlusTree<StationTimeTrain,STTHaashByStationAndTime,80,1000> Arriv("Arriv"),Depar("Depar");

inline bool addTrain(const std::string &_train_id,int _station_num,int _seat_num
                    ,const std::string &_stations,const std::string &_prices,int _departure
                    ,const std::string &_travel_times,const std::string &_stop_times
                    ,const std::string _sale_date,char _type);
inline bool deleteTrain(const std::string &_train_id);
inline bool releaseTrain(const std::string &_train_id);
inline bool queryTrain(const std::string &_train_id,int date);
inline void queryTicket(const std::string &S,const std::string &T,int date,const std::string &cmp);
inline void queryTransfer(const std::string &S,const std::string &T,int date,const std::string &cmp);

inline std::ostream& operator << (std::ostream &os,const Train &t)
{
    return os<<"["<<t.train_id<<","<<t.station_num<<"]";
}
inline std::ostream& operator << (std::ostream &os,const StationTimeTrain &t)
{
    return os<<"["<<t.station<<","<<t.date<<","<<t.time<<","<<t.train_id<<", sid="<<t.sta_id<<", startdate = "<<t.start_date<<" ]";
}

#include<Train_implement.hpp>