#pragma once
#pragma GCC optimize(2)

#include <BPlusTree.hpp>

#pragma pack(1)

template <typename T, typename Comp, int N, int M> class MemoryRiver_old;

template <typename T, typename Comp, int N, int M> class Node_old {
public:
  static constexpr int max_ch_cnt = N; // 60
  int pos, ch_cnt, fth, nxt;
  int ch_pos[max_ch_cnt + 1];
  T ch_dat[max_ch_cnt + 1];

  static constexpr int memory_size =
      4 + 4 + 4 + 4 + (max_ch_cnt + 1) * 4 + (max_ch_cnt + 1) * T::memory_size;

  explicit Node_old() noexcept;
  explicit Node_old(int this_pos, FileOperator &fo,
                MemoryRiver_old<T, Comp,N,M> &mr) noexcept;
  inline Node_old &operator=(const Node_old &b);
  inline void write(FileOperator &fo, MemoryRiver_old<T, Comp,N,M> &mr) noexcept;
  inline constexpr T maxElement() noexcept;
  inline constexpr bool isLeaf() noexcept;
  inline void insert(int p, const T &v, int pos) noexcept; // insert before p
  inline void remove(int p) noexcept;
  inline int findPos(int p) noexcept;
};

template <typename T, typename Comp, int N, int M> class MemoryRiver_old {
public:
  static constexpr int mr_siz = M; // 60
  Node_old<T, Comp, N,M> mem[mr_siz];

  inline MemoryRiver_old() noexcept;
  inline void clear() noexcept;
  inline void readNode(int pos, FileOperator &fo,
                       Node_old<T,Comp,N,M> &dst) noexcept;
  inline void writeNode(Node_old<T,Comp,N,M> u, FileOperator &fo) noexcept;
  inline void invalidate(int pos) noexcept;
};

template <typename T, typename Comp,int N,int M> class BPlusTree_old {
public:
  Comp comp;
private:
  int rt_pos, siz;
  FileOperator fo;
  MemoryRiver_old<T, Comp, N, M> mr;

  inline void updateAncestors(int uid, int vp) noexcept;
  inline std::pair<int, int> innerInsert(int uid, const T &t) noexcept;
  inline void innerRemove(int uid, int vid, const T &t, bool ulev) noexcept;
  inline std::pair<int, int> lowerBound(const T &t) noexcept;

  inline void innerDebugPrint(int uid,std::ostream &os) noexcept;

public:
  explicit BPlusTree_old(const std::string &name) noexcept;
  ~BPlusTree_old() noexcept;
  inline void clear() noexcept;

  inline void insert(const T &t) noexcept;
  inline bool remove(const T &t) noexcept;
  inline sjtu::vector<T> find(const T &l, const T &r) noexcept; // find [l,r)
  inline void find(const T &l, const T &r,sjtu::vector<T> &res) noexcept; // find [l,r)
  inline int size() noexcept;

  inline void debugPrint(std::ostream &os) noexcept;
};

#include <BPlusTree_old_implement.hpp>