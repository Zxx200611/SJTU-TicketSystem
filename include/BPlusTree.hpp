#pragma once

#pragma GCC optimize(2)

#include<FileOperator.hpp>
#include<Vector.hpp>
#include<Utilities.hpp>
#include<string>
#include<utility>
#include<cassert>

#pragma pack(1)
template<typename T,typename Compare>
class Node
{
public:
    static constexpr int max_ch_cnt=60;    //180
    int pos,ch_cnt,fth,nxt;
    int ch_pos[max_ch_cnt+1];
    T ch_dat[max_ch_cnt+1];

    static constexpr int memory_size=4+4+4+4+(max_ch_cnt+1)*4+(max_ch_cnt+1)*T::memory_size;

    explicit Node(int this_pos,FileOperator &fo) noexcept;
    inline Node& operator = (const Node &b);
    inline void write(FileOperator &fo) noexcept;
    inline constexpr T maxElement() noexcept;
    inline constexpr bool isLeaf() noexcept;
    inline void insert(int p,const T &v,int pos) noexcept;   // insert before p
    inline void remove(int p) noexcept;
    inline int findPos(int p) noexcept;
};
template<typename T,typename Compare>
class SemiNode
{
public:
    int pos,ch_cnt,fth,nxt;
    static constexpr int memory_size=4+4+4+4;

    explicit SemiNode(int this_pos,FileOperator &fo) noexcept;
    inline void write(FileOperator &fo) noexcept;
};

template<typename T,typename Compare>
class BPlusTree
{
private:

    int rt_pos,siz;
    FileOperator fo;
    Compare comp;

    inline void updateAncestors(int uid,int vp) noexcept;
    inline std::pair<int,int> innerInsert(int uid,const T &t) noexcept;
    inline void innerRemove(int uid,int vid,const T &t,bool ulev) noexcept;
    inline std::pair<int,int> lowerBound(const T &t) noexcept;

    inline void innerDebugPrint(int uid) noexcept;

public:
    explicit BPlusTree(const std::string &file_name) noexcept;
    ~BPlusTree() noexcept;

    inline void insert(const T &t) noexcept;
    inline bool remove(const T &t) noexcept;
    inline sjtu::vector<T> find(const T &l,const T&r) noexcept; // find [l,r)
    inline int size() noexcept;

    inline void debugPrint() noexcept;
};

#include<BPlusTree_implement.hpp>