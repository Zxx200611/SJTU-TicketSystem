#include<BPlusTree.hpp>

const int chinese_character_size=3;

class User
{
public:
    const static int memory_size=20+30+5*chinese_character_size+30+4;

    char username[20],passwd[30],chinese_name[5*chinese_character_size],mail[30];
    int privilege;

    User();
    explicit User(const std::string &_username,const std::string &_passwd
                 ,const std::string &_chinese_name,const std::string &_mail,int _privilege);
    inline User& operator = (const User &other);
    static User zero();
    static User positiveInfinity();
};
class UserCompareByUsername
{
public:
    bool operator () (const User &a,const User &b);
};

#include<User_implement.hpp>

BPlusTree<User,UserCompareByUsername> Users("Users.dat");
sjtu::map<std::string,bool> login_list;