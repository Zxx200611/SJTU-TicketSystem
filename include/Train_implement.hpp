#pragma once
#include<Train.hpp>

#include<algorithm>

#pragma pack(1)
// Train******************************************************************************
Train::Train():train_id(""),station_num(0),departure(0),sale_start(0),sale_end(0)
              ,type(0),is_released(0)
{
    memset(stations,0,sizeof(stations));
    memset(seats,0,sizeof(seats));
    memset(prices,0,sizeof(prices));
    memset(travel_times,0,sizeof(travel_times));
    memset(stop_times,0,sizeof(stop_times));
}
Train::Train(const std::string &_train_id,int _station_num,int _seat_num
            ,const std::string &_stations,const std::string &_prices,int _departure
            ,const std::string &_travel_times,const std::string &_stop_times
            ,const std::string _sale_date,char _type):is_released(0)
{
    strcpy(train_id,_train_id.c_str());
    station_num=_station_num,seat_num=_seat_num;
    if(seat_num!=0)
    {
        sjtu::vector<std::string> stations_tmp=utils::splitBy(_stations,'|');
        sjtu::vector<std::string> prices_tmp=utils::splitBy(_prices,'|');
        sjtu::vector<std::string> travel_tmp=utils::splitBy(_travel_times,'|');
        sjtu::vector<std::string> stop_tmp=utils::splitBy(_stop_times,'|');
        sjtu::vector<std::string> sale_tmp=utils::splitBy(_sale_date,'|');
        for(int i=0;i<station_num;i++) strcpy(stations[i],stations_tmp[i].c_str());
        for(int i=0;i<station_num-1;i++) prices[i]=utils::stringToInt(prices_tmp[i]);
        for(int i=0;i<station_num-1;i++) travel_times[i]=utils::stringToInt(travel_tmp[i]);
        for(int i=1;i<station_num-1;i++) stop_times[i]=utils::stringToInt(stop_tmp[i-1]);

        sale_start=utils::stringToDate(sale_tmp.front());
        sale_end=utils::stringToDate(sale_tmp.back());
        
        memset(seats,0,sizeof(seats));
        for(int i=sale_start;i<=sale_end;i++)
        {
            for(int j=0;j<station_num;j++) seats[i][j]=_seat_num;
        }
    }
    else sale_start=sale_end=0;
    departure=_departure,type=_type;
}

HashResult TrainHashByIdAndStationNum::operator () (const Train &a)
{
    HashResult res;
    res.t[0]=utils::stringHash(a.train_id,0);
    res.t[1]=utils::stringHash(a.train_id,1);
    res.t[2]=a.station_num;
    return res;
}
// Train******************************************************************************

// StationTimeTrain*******************************************************************
StationTimeTrain::StationTimeTrain():station_h(0,0),train_id_h(0,0),date(0),time(0){}
StationTimeTrain::StationTimeTrain(const std::string &_station,int _date,int _time,const std::string &_train_id
                                  ,int _start_date,int _start_time,int _sta_id,int _s_time,int _s_price)
{
    date=_date,time=_time;
    start_date=_start_date,start_time=_start_time;
    sta_id=_sta_id,s_time=_s_time,s_price=_s_price;
    station_h=std::make_pair(utils::stringHash(_station,0),utils::stringHash(_station,1));
    train_id_h=std::make_pair(utils::stringHash(_train_id,0),utils::stringHash(_train_id,1));
}

HashResult STTHaashByStationAndTime::operator () (const StationTimeTrain &a)
{
    HashResult res;
    res.t[0]=a.station_h.first;
    res.t[1]=a.station_h.second;
    res.t[2]=a.date;
    res.t[3]=a.time;
    res.t[4]=a.train_id_h.first;
    res.t[5]=a.train_id_h.second;
    return res;
}
// StationTimeTrain*******************************************************************

// QueryTicketResult******************************************************************
QueryTicketResult::QueryTicketResult():train_id(""),s(""),t(""),s_date(0),s_time(0),t_date(0)
                                      ,t_time(0),time(0),price(0),seat(0){}

QueryTicketResult::QueryTicketResult(const std::string &_train_id,const std::string &_s
                                    ,const std::string &_t,int _s_date,int _s_time,int _t_date
                                    ,int _t_time,int _time,int _price,int _seat)
{
    strcpy(train_id,_train_id.c_str());
    strcpy(s,_s.c_str()),strcpy(t,_t.c_str());
    s_date=_s_date,s_time=_s_time,t_date=_t_date,t_time=_t_time;
    time=_time,price=_price,seat=_seat;
}
// QueryTicketResult******************************************************************

// QueryTransferResult****************************************************************
QueryTransferResult::QueryTransferResult():ta(),tb(){}
QueryTransferResult::QueryTransferResult(const QueryTicketResult &a,const QueryTicketResult &b):ta(a),tb(b){}
// QueryTransferResult****************************************************************

// Others ****************************************************************************
inline bool addTrain(const std::string &_train_id,int _station_num,int _seat_num
                    ,const std::string &_stations,const std::string &_prices,int _departure
                    ,const std::string &_travel_times,const std::string &_stop_times
                    ,const std::string _sale_date,char _type)
{
    // Timer timer(__func__);
    // std::cout<<"addtrain Stations1 : "<<_stations<<std::endl;
    // Trains.debugPrint();
    // std::cout<<"addtrain Stations2 : "<<_stations<<std::endl;
    sjtu::vector<Train> tmp=Trains.find
    (
        Train(_train_id,0,0,"","",0,"","","",0),
        Train(_train_id,Train::max_station_cnt+1,0,"","",0,"","","",0)
    );
    // std::cout<<"Tmp size = "<<tmp.size()<<std::endl;
    if(tmp.size()!=0) return 0;
    // std::cout<<"Adding"<<std::endl;

    Train *u=new Train(_train_id,_station_num,_seat_num,_stations,_prices,_departure,_travel_times
           ,_stop_times,_sale_date,_type);
    // std::cout<<"Constructed"<<std::endl;
    // Trains.debugPrint();
    // if(_train_id=="LeavesofGrass") std::cerr<<"Inserting "<<_train_id<<std::endl;
    // if(_train_id=="LeavesofGrass") Trains.debugPrint(std::cerr);
    Trains.insert(*u);
    // if(_train_id=="LeavesofGrass") std::cerr<<"Inserted"<<std::endl;
    // if(_train_id=="LeavesofGrass") Trains.debugPrint(std::cerr);
    delete u;
    return 1;
}
inline bool deleteTrain(const std::string &_train_id)
{
    // Timer timer(__func__);
    sjtu::vector<Train> tmp=Trains.find
    (
        Train(_train_id,0,0,"","",0,"","","",0),
        Train(_train_id,Train::max_station_cnt+1,0,"","",0,"","","",0)
    );
    if(tmp.size()!=1) return 0;

    Train u=tmp.front();
    if(u.is_released) return 0;
    Trains.remove(u);
    return 1;
}
inline bool releaseTrain(const std::string &_train_id)
{
    // Timer timer(__func__);
    Train u;
    int u_pos=Trains.findFirstGe(Train(_train_id,0,0,"","",0,"","","",0),u);
    if(u_pos==-1||u.train_id!=_train_id) return 0;
    if(u.is_released==1) return 0;

    int d=0,t=u.departure,sp=0,st=0;
    for(int i=0;i+1<u.station_num;i++)
    {
        for(int depar_date=u.sale_start;depar_date<=u.sale_end;depar_date++)
            Depar.insert(StationTimeTrain(u.stations[i],d+depar_date,t,u.train_id,depar_date,u.departure,i,st,sp));
        utils::forward(d,t,u.travel_times[i]),st+=u.travel_times[i],sp+=u.prices[i];
        
        for(int depar_date=u.sale_start;depar_date<=u.sale_end;depar_date++)
            Arriv.insert(StationTimeTrain(u.stations[i+1],d+depar_date,t,u.train_id,depar_date,u.departure,i+1,st,sp));
        utils::forward(d,t,u.stop_times[i+1]),st+=u.stop_times[i+1];
    }

    static bool one=1;
    Trains.dfo.write(u_pos+Train::memory_size-1,&one);
    return 1;
}
bool queryTrain(const std::string &_train_id,int date)
{
    // Timer timer(__func__);
    if(date<0) return 0;
    sjtu::vector<Train> tmp=Trains.find
    (
        Train(_train_id,0,0,"","",0,"","","",0),
        Train(_train_id,Train::max_station_cnt+1,0,"","",0,"","","",0)
    );
    if(tmp.size()!=1) return 0;

    Train u=tmp.front();
    if(date<u.sale_start||date>u.sale_end) return 0;

    std::cout<<u.train_id<<" "<<u.type<<'\n';
    int d=date,t=u.departure,sp=0;
    std::cout<<u.stations[0]<<" xx-xx xx:xx -> "<<utils::intToDate(d)<<" "<<utils::intToTime(t)
             <<" "<<sp<<" "<<u.seats[date][0]<<'\n';
    sp+=u.prices[0];
    utils::forward(d,t,u.travel_times[0]);
    for(int i=1;i+1<u.station_num;i++)
    {
        std::cout<<u.stations[i]<<" "<<utils::intToDate(d)<<" "<<utils::intToTime(t)<<" -> ";
        utils::forward(d,t,u.stop_times[i]);
        std::cout<<utils::intToDate(d)<<" "<<utils::intToTime(t)<<" "<<sp<<" "<<u.seats[date][i]<<'\n';
        utils::forward(d,t,u.travel_times[i]);
        if(i!=u.station_num-1) sp+=u.prices[i];
    }
    std::cout<<u.stations[u.station_num-1]<<" "<<utils::intToDate(d)<<" "<<utils::intToTime(t)<<" -> "
             <<"xx-xx xx:xx "<<sp<<" x"<<std::endl;
    return 1;
}
void queryTicket(const std::string &S,const std::string &T,int date,const std::string &cmp)
{
    // Timer timer(__func__);
    sjtu::vector<StationTimeTrain> tmpS=Depar.find
    (
        StationTimeTrain(S,date,0,"",0,0,0,0,0),
        StationTimeTrain(S,date,1440,"",0,0,0,0,0)
    );
    sjtu::vector<StationTimeTrain> tmpT=Arriv.find
    (
        StationTimeTrain(T,date,0,"",0,0,0,0,0),
        StationTimeTrain(T,date+3,1440,"",0,0,0,0,0)
    );

    static int pS[50010],pT[50010];
    for(int i=0;i<tmpS.size();i++) pS[i]=i;
    for(int i=0;i<tmpT.size();i++) pT[i]=i;
    utils::sort(pS,pS+tmpS.size(),[&](int a,int b)
    {
        if(tmpS[a].train_id_h!=tmpS[b].train_id_h) return tmpS[a].train_id_h<tmpS[b].train_id_h;
        if(tmpS[a].start_date!=tmpS[b].start_date) return tmpS[a].start_date<tmpS[b].start_date;
        return a<b;
    });
    utils::sort(pT,pT+tmpT.size(),[&](int a,int b)
    {
        if(tmpT[a].train_id_h!=tmpT[b].train_id_h) return tmpT[a].train_id_h<tmpT[b].train_id_h;
        if(tmpT[a].start_date!=tmpT[b].start_date) return tmpT[a].start_date<tmpT[b].start_date;
        return a<b;
    });

    sjtu::vector<QueryTicketResult> res;
    for(int i=0,j=0;i<tmpS.size();i++)
    {
        int cmp_res=(j==tmpT.size()?0:(tmpT[pT[j]].train_id_h==tmpS[pS[i]].train_id_h?0:(tmpT[pT[j]].train_id_h<tmpS[pS[i]].train_id_h?-1:1)));
        while(j<tmpT.size()&&(cmp_res<0||(cmp_res==0&&tmpT[pT[j]].start_date<tmpS[pS[i]].start_date)))
        {
            j++;
            cmp_res=(j==tmpT.size()?0:(tmpT[pT[j]].train_id_h==tmpS[pS[i]].train_id_h?0:(tmpT[pT[j]].train_id_h<tmpS[pS[i]].train_id_h?-1:1)));
        }
        if(j<tmpT.size()&&cmp_res==0&&tmpT[pT[j]].start_date==tmpS[pS[i]].start_date)
        {
            if(tmpS[pS[i]].sta_id>=tmpT[pT[j]].sta_id) continue;
            
            Train u;
            HashResult hr;hr.t[0]=tmpS[pS[i]].train_id_h.first,hr.t[1]=tmpS[pS[i]].train_id_h.second;
            Trains.findFirstGe(hr,u);

            int mnst=1e8;
            for(int k=tmpS[pS[i]].sta_id;k!=tmpT[pT[j]].sta_id;k++)
            {
                mnst=std::min(mnst,u.seats[tmpS[pS[i]].start_date][k]);
            }

            res.push_back(QueryTicketResult
            (
                u.train_id,S,T,tmpS[pS[i]].date,tmpS[pS[i]].time,tmpT[pT[j]].date,
                tmpT[pT[j]].time,tmpT[pT[j]].s_time-tmpS[pS[i]].s_time,tmpT[pT[j]].s_price-tmpS[pS[i]].s_price,mnst
            ));
        }
    }

    static int p[50010];
    for(int i=0;i<res.size();i++) p[i]=i;
    std::function<bool(int,int)> comp;
    if(cmp=="time") comp=[&](int a,int b)->bool {return res[a].time ==res[b].time ?strcmp(res[a].train_id,res[b].train_id)<0:res[a].time <res[b].time ;};
    else            comp=[&](int a,int b)->bool {return res[a].price==res[b].price?strcmp(res[a].train_id,res[b].train_id)<0:res[a].price<res[b].price;};
    utils::sort(p,p+res.size(),comp);

    std::cout<<res.size()<<'\n';
    for(int i=0;i<res.size();i++)
    {
        int x=p[i];
        std::cout<<res[x].train_id<<" "
                 <<res[x].s<<" "<<utils::intToDate(res[x].s_date)<<" "<<utils::intToTime(res[x].s_time)<<" -> "
                 <<res[x].t<<" "<<utils::intToDate(res[x].t_date)<<" "<<utils::intToTime(res[x].t_time)<<" "
                 <<res[x].price<<" "<<res[x].seat<<'\n';
    }
    std::cout.flush();
}
void queryTransfer(const std::string &S,const std::string &T,int date,const std::string &cmp)
{
    // Timer timer(__func__);
    // if(S=="广东省惠阳市"&&T=="江苏省高邮市") std::cerr<<"Get this transfer"<<std::endl;
    static sjtu::map<std::pair<int,int>,std::string> rhash;
    static sjtu::map<std::pair<int,int>,bool> S_sta,T_sta;
    static sjtu::map<std::pair<int,int>,int>  S_pri,T_pri,S_tme,T_tme,S_id,T_id;
    S_sta.clear(),S_pri.clear(),S_tme.clear(),S_id.clear();
    T_sta.clear(),T_pri.clear(),T_tme.clear(),T_id.clear();

    sjtu::vector<StationTimeTrain> tmp=Depar.find
    (
        StationTimeTrain(S,date,0,"",0,0,0,0,0),
        StationTimeTrain(S,date,1440,"",0,0,0,0,0)
    );
    // std::cout<<"Depar size : "<<tmp.size()<<std::endl;
    for(const StationTimeTrain &x:tmp)
    {
        if(S_id.count(x.train_id_h)) continue;

        Train u;
        HashResult hr;hr.t[0]=x.train_id_h.first,hr.t[1]=x.train_id_h.second;
        Trains.findFirstGe(hr,u);
        rhash[x.train_id_h]=u.train_id;

        int sn=0;
        while(strcmp(S.c_str(),u.stations[sn])) sn++;
        for(int i=sn+1;i<u.station_num;i++)
        {
            pair<int,int> hp(utils::stringHash(u.stations[i],0),utils::stringHash(u.stations[i],1));
            S_sta[hp]=1,rhash[hp]=u.stations[i];
        }
        S_pri[x.train_id_h]=x.s_price,S_tme[x.train_id_h]=x.s_time,S_id[x.train_id_h]=x.sta_id;
    }

    tmp=Arriv.find
    (
        StationTimeTrain(T,date,0,"",0,0,0,0,0),
        StationTimeTrain(T,200,0,"",0,0,0,0,0)
    );
    for(const StationTimeTrain &x:tmp)
    {
        if(T_id.count(x.train_id_h)) continue;
        Train u;
        HashResult hr;hr.t[0]=x.train_id_h.first,hr.t[1]=x.train_id_h.second;
        Trains.findFirstGe(hr,u);
        rhash[x.train_id_h]=u.train_id;

        for(int sn=0;strcmp(T.c_str(),u.stations[sn]);sn++)
        {
            pair<int,int> hp(utils::stringHash(u.stations[sn],0),utils::stringHash(u.stations[sn],1));
            T_sta[hp]=1,rhash[hp]=u.stations[sn];
        }
        T_pri[x.train_id_h]=x.s_price,T_tme[x.train_id_h]=x.s_time,T_id[x.train_id_h]=x.sta_id;
    }

    QueryTransferResult mnv
    (
        QueryTicketResult("","","",0,0,200,0,1e8,1e8,0),
        QueryTicketResult("","","",0,0,200,0,1e8,1e8,0)
    );
    std::function<bool(const QueryTransferResult &,const QueryTransferResult &)> cmp_res;
    std::function<bool(const StationTimeTrain &,const StationTimeTrain &)> cmp_trn;
    if(cmp=="time")
    {
        cmp_res=[&](const QueryTransferResult &a,const QueryTransferResult &b)->bool
        {
            int ta=utils::timeDelta(a.ta.s_date,a.ta.s_time,a.tb.t_date,a.tb.t_time);
            int tb=utils::timeDelta(b.ta.s_date,b.ta.s_time,b.tb.t_date,b.tb.t_time);
            if(ta!=tb) return ta<tb;

            int pa=a.ta.price+a.tb.price,pb=b.ta.price+b.tb.price;
            if(pa!=pb) return pa<pb;

            int c1=strcmp(a.ta.train_id,b.ta.train_id);
            if(c1!=0) return c1<0;

            int c2=strcmp(a.tb.train_id,b.tb.train_id);
            return c2<0;
        };
        cmp_trn=[&](const StationTimeTrain &a,const StationTimeTrain &b)->bool
        {
            int da=a.date,ta=a.time;utils::forward(da,ta,T_tme[a.train_id_h]-a.s_time);
            int db=b.date,tb=b.time;utils::forward(db,tb,T_tme[b.train_id_h]-b.s_time);
            if(da!=db) return da<db;
            if(ta!=tb) return ta<tb;

            int pa=T_pri[a.train_id_h]-a.s_price,pb=T_pri[b.train_id_h]-b.s_price;
            if(pa!=pb) return pa<pb;

            return rhash[a.train_id_h]<rhash[b.train_id_h];
        };
    }
    else
    {
        cmp_res=[&](const QueryTransferResult &a,const QueryTransferResult &b)->bool
        {
            int pa=a.ta.price+a.tb.price,pb=b.ta.price+b.tb.price;
            if(pa!=pb) return pa<pb;

            int ta=utils::timeDelta(a.ta.s_date,a.ta.s_time,a.tb.t_date,a.tb.t_time);
            int tb=utils::timeDelta(b.ta.s_date,b.ta.s_time,b.tb.t_date,b.tb.t_time);
            if(ta!=tb) return ta<tb;

            int c1=strcmp(a.ta.train_id,b.ta.train_id);
            if(c1!=0) return c1<0;

            int c2=strcmp(a.tb.train_id,b.tb.train_id);
            return c2<0;
        };
        cmp_trn=[&](const StationTimeTrain &a,const StationTimeTrain &b)->bool
        {
            int pa=T_pri[a.train_id_h]-a.s_price,pb=T_pri[b.train_id_h]-b.s_price;
            if(pa!=pb) return pa<pb;

            int da=a.date,ta=a.time;utils::forward(da,ta,T_tme[a.train_id_h]-a.s_time);
            int db=b.date,tb=b.time;utils::forward(db,tb,T_tme[b.train_id_h]-b.s_time);
            if(da!=db) return da<db;
            if(ta!=tb) return ta<tb;

            return rhash[a.train_id_h]<rhash[b.train_id_h];
        };
    }

    for(auto pr:S_sta)
    {
        if(pr.second==0||T_sta[pr.first]==0||(pr.first.first==utils::stringHash(S,0)&&pr.first.second==utils::stringHash(S,1))||(pr.first.first==utils::stringHash(T,0)&&pr.first.second==utils::stringHash(T,1))) continue;
        const std::string &sta_name=rhash[pr.first];

        sjtu::vector<StationTimeTrain> A,tmpA=Arriv.find
        (
            StationTimeTrain(sta_name,date,0,"",0,0,0,0,0),
            StationTimeTrain(sta_name,date+3,1440,"",0,0,0,0,0)
        );
        for(const StationTimeTrain &stt:tmpA)
        {
            int s_date=stt.date,s_time=stt.time;
            utils::forward(s_date,s_time,S_tme[stt.train_id_h]-stt.s_time);
            if(s_date!=date) continue;
            auto it=S_id.find(stt.train_id_h);
            if(it==S_id.end()||it->second>=stt.sta_id) continue;
            A.push_back(stt);
        }

        sjtu::vector<StationTimeTrain> B,tmpB=Depar.find
        (
            StationTimeTrain(sta_name,date,0,"",0,0,0,0,0),
            StationTimeTrain(sta_name,200,0,"",0,0,0,0,0)
        );
        for(const StationTimeTrain &stt:tmpB)
        {
            auto it=T_id.find(stt.train_id_h);
            if(it==T_id.end()||it->second<=stt.sta_id) continue;
            B.push_back(stt);
        }

        if(A.empty()||B.empty()) continue;

        static int pA[50010],pB[50010];
        for(int i=0;i<A.size();i++) pA[i]=i;
        for(int i=0;i<B.size();i++) pB[i]=i;
        utils::sort(pA,pA+A.size(),[&](int a,int b)
        {
            if(A[a].date!=A[b].date) return A[a].date<A[b].date;
            return A[a].time<A[b].time;
        });
        utils::sort(pB,pB+B.size(),[&](int a,int b)
        {
            if(B[a].date!=B[b].date) return B[a].date<B[b].date;
            return B[a].time<B[b].time;
        });

        static int suf_mnp[50010][2];
        suf_mnp[B.size()][0]=suf_mnp[B.size()][1]=B.size();
        for(int i=B.size()-1;i>=0;i--)
        {
            int add=pB[i];
            suf_mnp[i][0]=suf_mnp[i+1][0],suf_mnp[i][1]=suf_mnp[i+1][1];
            if(suf_mnp[i][0]==B.size()||cmp_trn(B[add],B[suf_mnp[i][0]]))
            {
                std::swap(add,suf_mnp[i][0]);
                if(add==B.size()||B[suf_mnp[i][0]].train_id_h==B[add].train_id_h) continue; 
            }
            if(suf_mnp[i][1]==B.size()||cmp_trn(B[add],B[suf_mnp[i][1]])) std::swap(add,suf_mnp[i][1]);
        }

        for(int ni=0,jp=0;ni<A.size();ni++)
        {
            int i=pA[ni];
            while(jp!=B.size()&&utils::timeDelta(A[i].date,A[i].time,B[jp].date,B[jp].time)<0) jp++;
            if(jp!=B.size())
            {
                int j=(A[i].train_id_h==B[suf_mnp[jp][0]].train_id_h?suf_mnp[jp][1]:suf_mnp[jp][0]);
                if(j==B.size()) continue;

                int s_date=A[i].date,s_time=A[i].time;utils::forward(s_date,s_time,S_tme[A[i].train_id_h]-A[i].s_time);
                int t_date=B[j].date,t_time=B[j].time;utils::forward(t_date,t_time,T_tme[B[j].train_id_h]-B[j].s_time);
                QueryTransferResult nw
                (
                    QueryTicketResult(rhash[A[i].train_id_h],S,rhash[A[i].station_h],s_date,s_time
                                     ,A[i].date,A[i].time,A[i].s_time-S_tme[A[i].train_id_h]
                                     ,A[i].s_price-S_pri[A[i].train_id_h],0),
                    QueryTicketResult(rhash[B[j].train_id_h],rhash[B[j].station_h],T,B[j].date,B[j].time
                                     ,t_date,t_time,T_tme[B[j].train_id_h]-B[j].s_time
                                     ,T_pri[B[j].train_id_h]-B[j].s_price,0)
                );
                // if(S=="广东省惠阳市"&&T=="江苏省高邮市")
                // {
                //     std::cerr<<"Train #1 depar at date "<<utils::intToDate(A[i].start_date)<<std::endl;
                //     std::cerr<<"Train #2 depar at date "<<utils::intToDate(B[j].start_date)<<std::endl;
                // }
                if(cmp_res(nw,mnv)) mnv=nw;
                // if(S=="广东省惠阳市"&&T=="江苏省高邮市") std::cerr<<utils::intToDate(mnv.ta.t_date)<<std::endl;
            }
        }
    }
    // if(S=="广东省惠阳市"&&T=="江苏省高邮市") std::cerr<<utils::intToDate(mnv.ta.t_date)<<std::endl;
    if(mnv.ta.price==(int)1e8)
    {
        std::cout<<0<<std::endl;
        return;
    }

    int mnst1=1e8,mnst2=1e8;
    sjtu::vector<Train> tmp1=Trains.find
    (
        Train(mnv.ta.train_id,0,0,"","",0,"","","",0),
        Train(mnv.ta.train_id,Train::max_station_cnt+1,0,"","",0,"","","",0)
    );
    assert(tmp1.size()==1);
    const Train &t1=tmp1.front();
    std::pair<int,int> mnv_ta_trnid_h(utils::stringHash(mnv.ta.train_id,0),utils::stringHash(mnv.ta.train_id,1));
    std::pair<int,int> mnv_tb_trnid_h(utils::stringHash(mnv.tb.train_id,0),utils::stringHash(mnv.tb.train_id,1));
    int start_date1=mnv.ta.s_date,start_time1=mnv.ta.s_time,id1=S_id[mnv_ta_trnid_h];
    utils::forward(start_date1,start_time1,-S_tme[mnv_ta_trnid_h]);
    for(int i=id1;strcmp(t1.stations[i],mnv.ta.t);i++) mnst1=std::min(mnst1,t1.seats[start_date1][i]);
    // if(S=="广东省惠阳市"&&T=="江苏省高邮市") std::cerr<<utils::intToDate(mnv.ta.t_date)<<std::endl;

    sjtu::vector<Train> tmp2=Trains.find
    (
        Train(mnv.tb.train_id,0,0,"","",0,"","","",0),
        Train(mnv.tb.train_id,Train::max_station_cnt+1,0,"","",0,"","","",0)
    );
    assert(tmp2.size()==1);
    const Train &t2=tmp2.front();
    int start_date2=t2.sale_start,start_time2=t2.departure,id2=T_id[mnv_tb_trnid_h],id2s=0;
    for(;strcmp(t2.stations[id2s],mnv.tb.s);id2s++)
    {
        utils::forward(start_date2,start_time2,t2.travel_times[id2s]);
        utils::forward(start_date2,start_time2,t2.stop_times[id2s+1]);
    }
    assert(start_time2==mnv.tb.s_time);
    start_date2=t2.sale_start+mnv.tb.s_date-start_date2;
    for(int i=id2s;i<id2;i++) mnst2=std::min(mnst2,t2.seats[start_date2][i]);
    // if(S=="广东省惠阳市"&&T=="江苏省高邮市")
    // {
    //     std::cerr<<"Train #2 interval [ "<<id2s<<" , "<<id2<<" ]"<<std::endl;
    //     std::cerr<<"Train #1 depar at date "<<utils::intToDate(start_date1)<<std::endl;
    //     std::cerr<<"Train #2 depar at date "<<utils::intToDate(start_date2)<<std::endl;
    // }

    std::cout<<mnv.ta.train_id<<" "
             <<mnv.ta.s<<" "<<utils::intToDate(mnv.ta.s_date)<<" "<<utils::intToTime(mnv.ta.s_time)<<" -> "
             <<mnv.ta.t<<" "<<utils::intToDate(mnv.ta.t_date)<<" "<<utils::intToTime(mnv.ta.t_time)<<" "
             <<mnv.ta.price<<" "<<mnst1<<'\n';
    std::cout<<mnv.tb.train_id<<" "
             <<mnv.tb.s<<" "<<utils::intToDate(mnv.tb.s_date)<<" "<<utils::intToTime(mnv.tb.s_time)<<" -> "
             <<mnv.tb.t<<" "<<utils::intToDate(mnv.tb.t_date)<<" "<<utils::intToTime(mnv.tb.t_time)<<" "
             <<mnv.tb.price<<" "<<mnst2<<'\n';
}
// Others ****************************************************************************