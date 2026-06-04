#pragma once

#include<User.hpp>

// User*******************************************************************************
User::User():username(""),passwd(""),chinese_name(""),mail(""),privilege(0){}
User::User(const std::string &_username,const std::string &_passwd
          ,const std::string &_chinese_name,const std::string &_mail,int _privilege)
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
}
User User::zero(){return User();}
User User::positiveInfinity(){return User("~~~~~~~~~~~~~~~~~~~~","","","",0);}

bool UserCompareByUsername::operator() (const User &a,const User &b)
{
    return strcmp(a.username,b.username)<0;
}
// User*******************************************************************************
