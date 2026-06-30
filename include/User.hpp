#pragma once

#include<Timer.hpp>

#include<BPlusTree.hpp>
#include<Utilities.hpp>
#include<Vector.hpp>
#include<Map.hpp>
#include<iostream>

#pragma pack(1)
class User
{
public:
    const static int memory_size=25+35+6*chinese_character_size+35+4;

    char username[25],passwd[35],chinese_name[6*chinese_character_size],mail[35];
    int privilege;

    inline User();
    inline explicit User(const std::string &_username,int _privilege,const std::string &_passwd=""
                 ,const std::string &_chinese_name="",const std::string &_mail="");
    inline User& operator = (const User &other);
};
class UserHashByUsernameAndPriv
{
public:
    inline HashResult operator () (const User &a);
};

BPlusTree<User,UserHashByUsernameAndPriv,80,200> Users("Users");
sjtu::map<std::string,bool> login_list;

inline bool addUser(const std::string &c,const std::string &u,const std::string &_passwd
                   ,const std::string &_chinese_name,const std::string &_mail,int _privilege);
inline bool login (const std::string &_username,const std::string &_passwd);
inline bool logout(const std::string &_username);
inline bool queryProfile(const std::string &c,const std::string &u);
inline bool modifyProfile(const std::string &c,const std::string &u,const std::string &_passwd
                         ,const std::string &_chinese_name,const std::string &_mail,int _privilege);

inline std::ostream& operator << (std::ostream &os,const User &u)
{
    return os<<"("<<u.username<<","<<u.privilege<<")";
}


#include<User_implement.hpp>