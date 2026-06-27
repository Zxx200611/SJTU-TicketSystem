#pragma once
#pragma GCC optimize(2)

#include <FileOperator.hpp>
#include <Utilities.hpp>
#include <Vector.hpp>
#include <cassert>
#include <string>
#include <utility>
#include<cstring>

#pragma pack(1)
template <typename T, typename Hash, int N, int M> class MemoryRiver;
class HashResult;

template <typename T, typename Hash, int N, int M> class Node {
public:
  static constexpr int max_ch_cnt = N; // 60
  int pos, ch_cnt, fth, nxt;
  int ch_pos[max_ch_cnt + 1];
  HashResult ch_dat[max_ch_cnt + 1];

  static constexpr int memory_size =
      4 + 4 + 4 + 4 + (max_ch_cnt + 1) * 4 + (max_ch_cnt + 1) * T::memory_size;

  explicit Node() noexcept;
  explicit Node(int this_pos, FileOperator &fo,
                MemoryRiver<T, Hash,N,M> &mr) noexcept;
  inline Node &operator=(const Node &b);
  inline void write(FileOperator &fo, MemoryRiver<T, Hash,N,M> &mr) noexcept;
  inline constexpr HashResult maxElement() noexcept;
  inline constexpr bool isLeaf() noexcept;
  inline void insert(int p, const HashResult &v, int pos) noexcept; // insert before p
  inline void remove(int p) noexcept;
  inline int findPos(int p) noexcept;
};

template <typename T, typename Hash, int N, int M> class MemoryRiver {
public:
  static constexpr int mr_siz = M; // 60
  Node<T, Hash, N,M> mem[mr_siz];

  inline MemoryRiver() noexcept;
  inline void clear() noexcept;
  inline void readNode(int pos, FileOperator &fo,
                       Node<T,Hash,N,M> &dst) noexcept;
  inline void writeNode(Node<T,Hash,N,M> u, FileOperator &fo) noexcept;
};

class HashResult
{
public:
  static constexpr int hash_siz=6;
  static constexpr int memory_size=(hash_siz+1)*4;
  int t[hash_siz];
  int pos;

  inline HashResult() noexcept;
  static bool comp(const HashResult &a,const HashResult &b) noexcept;
  inline static HashResult zero() noexcept;
  inline static HashResult positiveInfinity() noexcept;
};
inline std::ostream &operator<<(std::ostream &os, const HashResult &e) {
  os << "< " << e.t[0] << " , " << e.t[1] << " , " << e.t[2] << " , " << e.t[3] << " , pos = "<<e.pos<<" >";
  return os;
}

template <typename T, typename Hash,int N,int M> class BPlusTree {
private:
  int rt_pos, siz;
  FileOperator fo,dfo;
  Hash hash;
  MemoryRiver<T, Hash, N, M> mr;

  inline void updateAncestors(int uid, int vp) noexcept;
  inline std::pair<int, int> innerInsert(int uid, const HashResult &t) noexcept;
  inline void innerRemove(int uid, int vid, const HashResult &t, bool ulev) noexcept;
  inline std::pair<int, int> lowerBound(const HashResult &t) noexcept;

  inline void innerDebugPrint(int uid) noexcept;

public:
  explicit BPlusTree(const std::string &name) noexcept;
  ~BPlusTree() noexcept;
  inline void clear() noexcept;

  inline void insert(const T &t) noexcept;
  inline bool remove(const T &t) noexcept;
  inline sjtu::vector<T> find(const T &l, const T &r) noexcept; // find [l,r)
  inline bool findFirstGe(const T &t,T &res) noexcept;
  inline int size() noexcept;


  inline void debugPrint() noexcept;
};

#include <BPlusTree_implement.hpp>