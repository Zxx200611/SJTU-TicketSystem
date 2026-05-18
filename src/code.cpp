#include<BPlusTree.hpp>
#include<iostream>
#include<string>

#pragma pack(1)
struct Element
{
    static const int memory_size=64*sizeof(char)+sizeof(long long);

    char key[64];
    long long val;

    Element():key(""),val(0){}
    Element(const std::string &s,long long x) noexcept:val(x)
    {
        for(int i=0;i<s.size();i++) key[i]=s[i];
        key[s.size()]='\0';
    }
    Element& operator = (const Element &b)
    {
        strcpy(key,b.key);
        val=b.val;
        return *this;
    }
    static Element zero() {return Element();}
    static Element positiveInfinity() {return Element("~~~~~~~~~~~~~~~~~~~~",INT_MAX);}
};
struct ElementCompare
{
    inline
    bool operator () (const Element &a,const Element &b) const
    {
        int res=strcmp(a.key,b.key);
        if(res!=0) return res<0;
        return a.val<b.val;
    }
};
BPlusTree<Element,ElementCompare> bpt("test.msav");

inline
std::ostream& operator << (std::ostream &os,const Element &e)
{
    os<<"< "<<e.key<<" , "<<e.val<<" >";
    return os;
}

int main()
{
    int q;
    std::cin>>q;
    // bpt.debugPrint();
    while(q--)
    {
        std::string op,k;
        int v;

        std::cin>>op;
        if(op=="insert")
        {
            std::cin>>k>>v;
            bpt.insert(Element(k,v));
        }
        if(op=="delete")
        {
            std::cin>>k>>v;
            bpt.remove(Element(k,v));
        }
        if(op=="find")
        {
            std::cin>>k;
            Element L(k,INT_MIN),R(k,(long long)INT_MAX+1);
            sjtu::vector<Element> res=bpt.find(L,R);
            for(Element &e:res) std::cout<<e.val<<" ";
            if(res.empty()) std::cout<<"null";
            std::cout<<'\n';
        }
        // std::cout<<"Done"<<std::endl;
        // bpt.debugPrint();
    }
    std::cout.flush();
}
/*
14
delete a 1
insert b 1
insert b 3
insert c 2
find b
delete a 2
insert a 1
insert c 1
insert b 2
insert a 2
find a
insert a 3
find b
insert c 3


1 3 
1 2 
1 2 3 
*/