#pragma once

#include<string>
#include<cassert>

namespace utils {

const int day_count[4]={30,31,31,30};
const int day_countS[4]={30,61,92,122};

template <typename T, typename Compare>
T *lowerBound(T *s, T *t, T v, Compare comp) {
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
    assert(s.size()==5&&s[0]=='0'&&s[2]=='-');
    int m=stringToInt(s.substr(0,2)),d=stringToInt(s.substr(3,2));
    assert(0<=m&&m<=3);
    return d+(m==0?0:s[m-1]);
}
inline
std::string intToDate(int x)
{
    assert(x>0&&x<=day_countS[3]);
    int m=0;
    while(m<=3&&day_count[m]<x) x-=day_count[m],m++;
    return intToString(m,2)+'-'+intToString(x,2);
}
inline
int stringToTime(const std::string &s)
{
    assert(s.size()==5&&s[2]==':');
    return stringToInt(s.substr(0,2))*60+stringToInt(s.substr(3,2));
}


} // namespace utils