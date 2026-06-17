#pragma once

#include<User.hpp>
#pragma pack(1)
// User*******************************************************************************
User::User():username(""),passwd(""),chinese_name(""),mail(""),privilege(0){}
User::User(const std::string &_username,int _privilege,const std::string &_passwd
          ,const std::string &_chinese_name,const std::string &_mail)
{
    strcpy(username    ,_username    .c_str());
    strcpy(passwd      ,_passwd      .c_str());
    strcpy(chinese_name,_chinese_name.c_str());
    strcpy(mail        ,_mail        .c_str());
    privilege=_privilege;
}
User& User::operator = (const User &other)
{
    strcpy(username    ,other.username);
    strcpy(passwd      ,other.passwd);
    strcpy(chinese_name,other.chinese_name);
    strcpy(mail        ,other.mail);
    privilege=other.privilege;
    return *this;
}
User User::zero(){return User();}
User User::positiveInfinity(){return User("~~~~~~~~~~~~~~~~~~~~",0,"","","");}

bool UserCompareByUsernameAndPriv::operator() (const User &a,const User &b)
{
    // std::cout<<"Comparing user "<<a<<" and "<<b<<std::endl;
    int res=strcmp(a.username,b.username);
    if(res==0) return a.privilege<b.privilege;
    return res<0;
}
// User*******************************************************************************

// Others ****************************************************************************
inline bool addUser(const std::string &c,const std::string &u,const std::string &_passwd
                   ,const std::string &_chinese_name,const std::string &_mail,int _privilege)
{
    // std::cout<<"User count = "<<Users.size()<<std::endl;
    if(Users.size()==0)
    {
        // std::cout<<"This is the first user"<<std::endl;
        Users.insert(User(u,10,_passwd,_chinese_name,_mail));
        return 1;
    }

    if(login_list[c]==0) return 0;
    sjtu::vector<User> tmp=Users.find(User(c,-1),User(c,11));
    if(tmp.size()!=1) return 0;
    
    if(tmp.front().privilege<=_privilege) return 0;
    tmp=Users.find(User(u,-1),User(u,11));
    if(!tmp.empty()) return 0;

    Users.insert(User(u,_privilege,_passwd,_chinese_name,_mail));
    return 1;
}
inline bool login(const std::string &_username,const std::string &_passwd)
{
    if(login_list[_username]==1) return 0;
    sjtu::vector<User> tmp=Users.find(User(_username,-1),User(_username,11));
    // std::cout<<"User "<<_username<<" logging in, find "<<tmp.size()<<" users"<<std::endl;
    if(tmp.size()!=1) return 0;
    if(strcmp(tmp.front().passwd,_passwd.c_str())!=0)
    {
        // std::cout<<"Passwd wrong : should be "<<tmp.front().passwd<<" but now "<<_passwd<<" "<<strcmp(tmp.front().passwd,_passwd.c_str())<<std::endl;
        return 0;
    }

    login_list[_username]=1;
    return 1;
}
inline bool logout(const std::string &_username)
{
    if(login_list[_username]==0) return 0;
    login_list[_username]=0;
    return 1;
}
inline bool queryProfile(const std::string &c,const std::string &u)
{
    if(login_list[c]==0) return 0;
    User C,U;
    if(!Users.findFirstGe(User(c,-1),C)||C.username!=c) return 0;
    if(!Users.findFirstGe(User(u,-1),U)||U.username!=u) return 0;
    
    if(c!=u&&C.privilege<=U.privilege) return 0;
    std::cout<<U.username<<" ";
    std::cout<<U.chinese_name<<" ";
    std::cout<<U.mail<<" ";
    std::cout<<U.privilege<<"\n";
    return 1;
}
inline bool modifyProfile(const std::string &c,const std::string &u,const std::string &_passwd
                         ,const std::string &_chinese_name,const std::string &_mail,int _privilege)
{
    if(login_list[c]==0) return 0;
    User C,U;
    if(!Users.findFirstGe(User(c,-1),C)||C.username!=c) return 0;
    if(!Users.findFirstGe(User(u,-1),U)||U.username!=u) return 0;

    if(c!=u&&C.privilege<=U.privilege) return 0;
    if(_privilege!=-1&&C.privilege<=_privilege) return 0;
    Users.remove(U);
    if(!_passwd.empty()) strcpy(U.passwd,_passwd.c_str());
    if(!_chinese_name.empty()) strcpy(U.chinese_name,_chinese_name.c_str());
    if(!_mail.empty()) strcpy(U.mail,_mail.c_str());
    if(_privilege!=-1) U.privilege=_privilege;
    Users.insert(U);

    std::cout<<U.username<<" ";
    std::cout<<U.chinese_name<<" ";
    std::cout<<U.mail<<" ";
    std::cout<<U.privilege<<"\n";

    return 1;
}

// Others ****************************************************************************

