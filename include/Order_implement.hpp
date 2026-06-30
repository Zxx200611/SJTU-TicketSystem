#pragma once

#include<Order.hpp>

#pragma pack(1)
// Order******************************************************************************
Order::Order():username(""),train_id(""),s_station(""),t_station(""),tag(0),depar_date(0)
              ,s_date(0),s_time(0),t_date(0),t_time(0),price(0),count(0),status(0){}
Order::Order(const std::string &_username,const std::string &_train_id,const std::string &_s_station
            ,const std::string &_t_station,int _tag,int _depar_date,int _s_date,int _s_time,int _t_date
            ,int _t_time,int _price,int _count,int _status)
{
    tag=_tag,depar_date=_depar_date,s_date=_s_date,s_time=_s_time,t_date=_t_date,t_time=_t_time;
    price=_price,count=_count,status=_status;

    strcpy(username,_username.c_str());
    strcpy(train_id,_train_id.c_str());
    strcpy(s_station,_s_station.c_str());
    strcpy(t_station,_t_station.c_str());
}

HashResult OrderHashByUsernameAndTag::operator () (const Order &a)
{
    HashResult res;
    res.t[0]=utils::stringHash(a.username,0);
    res.t[1]=utils::stringHash(a.username,1);
    res.t[2]=a.tag;
    return res;
}
// Order******************************************************************************


// SemiOrder**************************************************************************
SemiOrder::SemiOrder():train_id(""),username(""),depar_date(0),tag(0){}
SemiOrder::SemiOrder(const std::string &_username,const std::string &_train_id,int _depar_date,int _tag)
{
    depar_date=_depar_date,tag=_tag;
    strcpy(username,_username.c_str());
    strcpy(train_id,_train_id.c_str());
}

HashResult SOHashByTrainidAndDeparDate::operator () (const SemiOrder &a)
{
    HashResult res;
    res.t[0]=utils::stringHash(a.train_id,0);
    res.t[1]=utils::stringHash(a.train_id,1);
    res.t[2]=a.depar_date;
    res.t[3]=a.tag;
    return res;
}
// SemiOrder**************************************************************************


// Others ****************************************************************************
void queryOrder(const std::string &username)
{
    // Timer timer(__func__);
    if(login_list[username]==0)
    {
        std::cout<<"-1\n";
        return;
    }

    sjtu::vector<Order> tmp=Orders.find
    (
        Order(username,"","","",0,0,0,0,0,0,0,0,0),
        Order(username,"","","",INT_MAX,0,0,0,0,0,0,0,0)
    );

    std::cout<<tmp.size()<<'\n';
    for(int i=(int)tmp.size()-1;i>=0;i--)
    {
        const Order &o=tmp[i];
        // std::cout<<o.tag<<" ";
        if(o.status==0) std::cout<<"[pending] ";
        if(o.status==1) std::cout<<"[success] ";
        if(o.status==2) std::cout<<"[refunded] ";
        std::cout<<o.train_id<<" ";
        std::cout<<o.s_station<<" "<<utils::intToDate(o.s_date)<<" "<<utils::intToTime(o.s_time)<<" -> ";
        std::cout<<o.t_station<<" "<<utils::intToDate(o.t_date)<<" "<<utils::intToTime(o.t_time)<<" ";
        std::cout<<o.price<<" "<<o.count<<'\n';
    }
}
void buyTicket(const std::string &username,const std::string &train_id,const std::string &S
              ,const std::string &T,int date,int count,bool allow_queue,int tag)
{
    // Timer timer(__func__);
    if(login_list[username]==0)
    {
        std::cout<<"-1\n";
        return;
    }

    Train t;
    int p=Trains.findFirstGe(Train(train_id,0,0,"","",0,"","","",0),t);
    if(p==-1||t.train_id!=train_id)
    {
        std::cout<<"-1\n";
        return;
    }

    if(t.is_released==0||t.seat_num<count)
    {
        std::cout<<"-1\n";
        return;
    }

    int s_date=t.sale_start,s_time=t.departure,sp=0;
    for(sp=0;sp<t.station_num&&strcmp(t.stations[sp],S.c_str())!=0;sp++)
    {
        utils::forward(s_date,s_time,t.travel_times[sp]);
        utils::forward(s_date,s_time,t.stop_times[sp+1]);
    }
    if(sp==t.station_num)
    {
        std::cout<<"-1\n";
        return;
    }
    int depar_date=t.sale_start+date-s_date;
    s_date=date;

    if(depar_date<t.sale_start||depar_date>t.sale_end)
    {
        std::cout<<"-1\n";
        return;
    }

    int mnst=1e8,tp=sp,s_pri=0,t_date=s_date,t_time=s_time;
    for(;tp<t.station_num&&strcmp(t.stations[tp],T.c_str())!=0;tp++)
    {
        utils::forward(t_date,t_time,t.travel_times[tp]);
        utils::forward(t_date,t_time,t.stop_times[tp+1]);
        mnst=std::min(mnst,t.seats[depar_date][tp]);
        s_pri+=t.prices[tp];
    }
    if(tp==t.station_num)
    {
        std::cout<<"-1\n";
        return;
    }
    utils::forward(t_date,t_time,-t.stop_times[tp]);
    

    if(mnst<count)
    {
        if(allow_queue)
        {
            std::cout<<"queue\n";
            Orders.insert(Order(username,train_id,S,T,tag,depar_date,s_date,s_time,t_date,t_time
                               ,s_pri,count,0));
            Pend.insert(SemiOrder(username,train_id,depar_date,tag));
        }
        else std::cout<<"-1\n";
        return;
    }

    for(int i=sp;i!=tp;i++) t.seats[depar_date][i]-=count;
    Trains.dfo.write2(p+25+Train::max_station_cnt*35+4*(1+1+depar_date*Train::max_station_cnt),t.seats[depar_date],Train::max_station_cnt);

    Orders.insert(Order(username,train_id,S,T,tag,depar_date,s_date,s_time,t_date,t_time
                       ,s_pri,count,1));
    std::cout<<1ll*count*s_pri<<'\n';
}
void refundTicket(const std::string &username,int n)
{
    // Timer timer(__func__);
    if(login_list[username]==0)
    {
        std::cout<<"-1\n";
        return;
    }

    sjtu::vector<Order> tmp=Orders.find
    (
        Order(username,"","","",0,0,0,0,0,0,0,0,0),
        Order(username,"","","",1e9,0,0,0,0,0,0,0,0)
    );
    if(tmp.size()<n||tmp[tmp.size()-n].status==2)
    {
        std::cout<<"-1\n";
        return;
    }

    Order ro=tmp[tmp.size()-n];
    bool ro_sta=ro.status;
    Orders.remove(ro);
    ro.status=2;
    Orders.insert(ro);
    if(ro_sta==0)
    {
        Pend.remove(SemiOrder(ro.username,ro.train_id,ro.depar_date,ro.tag));
        std::cout<<"0\n";
        return;
    }

    Train t;
    int t_pos=Trains.findFirstGe(Train(ro.train_id,0,0,"","",0,"","","",0),t);

    int sp=0,tp=0;
    for(sp=0;strcmp(ro.s_station,t.stations[sp]);sp++);
    for(tp=sp;strcmp(ro.t_station,t.stations[tp]);tp++) t.seats[ro.depar_date][tp]+=ro.count;

    sjtu::vector<SemiOrder> W=Pend.find
    (
        SemiOrder("",ro.train_id,ro.depar_date,0),
        SemiOrder("",ro.train_id,ro.depar_date,1e9)
    );
    for(const SemiOrder &so:W)
    {
        sjtu::vector<Order> tmpo=Orders.find
        (
            Order(so.username,"","","",so.tag,0,0,0,0,0,0,0,0),
            Order(so.username,"","","",so.tag+1,0,0,0,0,0,0,0,0)
        );
        assert(tmpo.size()==1);
        Order o=tmpo.front();

        int mnst=1e8;
        for(sp=0 ;strcmp(o.s_station,t.stations[sp]);sp++);
        for(tp=sp;strcmp(o.t_station,t.stations[tp]);tp++) mnst=std::min(mnst,t.seats[o.depar_date][tp]);

        if(mnst>=o.count)
        {
            for(int i=sp;i!=tp;i++) t.seats[o.depar_date][i]-=o.count;
            
            Orders.remove(o);
            o.status=1;
            Orders.insert(o);
            Pend.remove(so);
        }
    }
    Trains.dfo.write(t_pos,&t);
    std::cout<<"0\n";
};
// Others ****************************************************************************
