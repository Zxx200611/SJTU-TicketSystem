#pragma once

#include<Timer.hpp>

#include<Vector.hpp>
#include<Map.hpp>
#include<string>
#include<cassert>
#include<cctype>
#include<functional>
#include<utility>

const int chinese_character_size=3;
#pragma pack(1)
namespace utils {

const int day_count[4]={30,31,31,30};
const int day_countS[4]={30,61,92,122};

const int hash_primes[2]={1000000007,1000000009};
const int hash_base=263;

template <typename T, typename Compare>
T *lowerBound(T *s, T *t,const T &v, Compare comp) {
  int l = 0, r = t - s - 1, m = l, res = r + 1;
  while (l <= r) {
    m = l + (r - l) / 2;
    if (!comp(s[m], v))
      res = m, r = m - 1;
    else
      l = m + 1;
  }
  return s + res;
}
template<typename T,typename Compare>
void sort(T *s,T *t,const Compare &comp)
{
    if(s+1==t||s==t) return;

    std::function<void(T*,T*)> reverse=[&](T *l,T *r)
    {
        r--;
        for(;l<r;l++,r--) std::swap(*l,*r);
    };

    T *m=s+(t-s+1)/2;
    sort(s,m,comp),sort(m,t,comp);
    while(m!=t&&s!=m)
    {
        T *tm=m;
        while(s!=m&&!comp(*m,*s)) s++;
        while(m!=t&&comp(*m,*s)) m++;
        reverse(s,tm),reverse(tm,m);
        reverse(s,m);
        s+=m-tm;
    }
}
inline
int stringHash(const std::string &s,int pid)
{
    // Timer timer(__func__);
    int res=0;
    for(int i=0,w=1;i<s.size();i++,w=1ll*w*hash_base%hash_primes[pid])
    {
        res=(res+1ll*w*(s[i]+1)%hash_primes[pid])%hash_primes[pid];
    }
    return res;
}
inline
std::pair<int,int> stringHashPair(const std::string &s)
{
    return std::make_pair(stringHash(s,0),stringHash(s,1));
}

inline
std::string intToString(int x,int w)
{
    std::string s="";
    for(int i=0;i<w;i++)
    {
        s+=(char)('0'+x%10);
        x/=10;
    }
    for(int i=0;2*i<w;i++) std::swap(s[i],s[w-1-i]);
    return s;
}
inline
int stringToInt(const std::string &s)
{
    int res=0;
    for(int i=0;i<s.size();i++)
    {
        res*=10;
        res+=s[i]-'0';
    }
    return res;
}
inline
int stringToDate(const std::string &s)
{
    if(s.size()!=5||s[0]!='0'||s[2]!='-') return -1;
    int m=stringToInt(s.substr(0,2))-6,d=stringToInt(s.substr(3,2));
    if(m<0||m>3) return -1;
    return d+(m==0?0:day_countS[m-1]);
}
inline
std::string intToDate(int x)
{
    assert(x>0&&x<=day_countS[3]);
    int m=0;
    while(m<=3&&day_count[m]<x) x-=day_count[m],m++;
    return intToString(6+m,2)+'-'+intToString(x,2);
}
inline
int stringToTime(const std::string &s)
{
    assert(s.size()==5&&s[2]==':');
    return stringToInt(s.substr(0,2))*60+stringToInt(s.substr(3,2));
}
inline
std::string intToTime(int x)
{
    return intToString(x/60,2)+":"+intToString(x%60,2);
}
inline
int timeDelta(int s_date,int s_time,int t_date,int t_time)
{
    return 1440*(t_date-s_date)+t_time-s_time;
}

inline
std::string parse(const std::string &s,sjtu::map<std::string,std::string> &M)
{
    // Timer timer(__func__);
    // std::cout<<"Parsing string "<<s<<std::endl;
    M.clear();

    int i=0;
    std::function<std::string(char)> getWord=[&](char ign)->std::string
    {
        std::string res="";
        while(i<s.size()&&(s[i]==ign||s[i]==' ')) i++;
        while(i<s.size()&&s[i]!=' ') res+=s[i],i++;
        while(!res.empty()&&(res.back()=='\r'||res.back()=='\n')) res.pop_back();
        return res;
    };

    std::string tag=getWord('[');
    M["T"]=tag.substr(0,tag.size()-1);

    std::string cmd=getWord('-');
    while(i<s.size())
    {
        std::string argn=getWord('-');
        M[argn]=getWord('-');
        // std::cout<<"Get "<<argn<<" -> "<<M[argn]<<std::endl;
    }
    // std::cout<<"Head = "<<cmd<<std::endl;
    return cmd;
}
inline
sjtu::vector<std::string> splitBy(const std::string &s,char c)
{
    // std::cout<<"Spliting "<<s<<" by "<<c<<" : ";
    sjtu::vector<std::string> res;
    std::string tmp="";
    for(int i=0;i<s.size();i++)
    {
        if(s[i]==c)
        {
            if(tmp.size()!=0) res.push_back(tmp);
            tmp="";
        }
        else tmp+=s[i];
    }
    if(tmp.size()!=0) res.push_back(tmp);
    // std::cout<<"Get "<<res.size()<<" elements"<<std::endl;
    return res;
}
inline
void forward(int &date,int &time,int delta)
{
    long long real_time=1ll*date*1440+time+delta;
    date=real_time/1440,time=real_time%1440;
}

} // namespace utils