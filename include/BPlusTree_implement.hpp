#include <BPlusTree.hpp>
#pragma GCC optimize(2)
#pragma pack(1)
// Node ***********************************************************
template <typename T, typename Compare,int N,int M> Node<T, Compare,N,M>::Node() noexcept {
  pos = 0, ch_cnt = 0;
  memset(ch_pos, 0, sizeof(ch_pos));
  for (int i = 0; i <= max_ch_cnt; i++)
    ch_dat[i] = T::zero();
  fth = nxt = 0;
}
template <typename T, typename Compare,int N,int M>
Node<T, Compare,N,M>::Node(int this_pos, FileOperator &fo,
                       MemoryRiver<T, Compare,N,M> &mr) noexcept {
  if (this_pos >= fo.size() || this_pos == 0) {
    // std::cout<<"Initializing node"<<std::endl;
    pos = this_pos, ch_cnt = 0;
    memset(ch_pos, 0, sizeof(ch_pos));
    for (int i = 0; i <= max_ch_cnt; i++)
      ch_dat[i] = T::zero();
    fth = nxt = 0;
  } else
    mr.readNode(this_pos, fo, *this);
}
template <typename T, typename Compare,int N,int M>
Node<T,Compare,N,M> &Node<T,Compare,N,M>::operator=(const Node<T,Compare,N,M> &b) {
  if (&b == this)
    return *this;

  pos = b.pos, ch_cnt = b.ch_cnt, fth = b.fth, nxt = b.nxt;
  for (int i = 0; i < ch_cnt; i++)
    ch_pos[i] = b.ch_pos[i];
  for (int i = 0; i < ch_cnt; i++)
    ch_dat[i] = b.ch_dat[i];
  return *this;
}
template <typename T, typename Compare,int N,int M>
void Node<T,Compare,N,M>::write(FileOperator &fo,
                             MemoryRiver<T,Compare,N,M> &mr) noexcept {
  mr.writeNode(*this, fo);
}
template <typename T, typename Compare,int N,int M>
constexpr T Node<T,Compare,N,M>::maxElement() noexcept {
  return ch_dat[ch_cnt - 1];
}
template <typename T, typename Compare,int N,int M>
constexpr bool Node<T,Compare,N,M>::isLeaf() noexcept {
  return ch_pos[0] == 0;
}
template <typename T, typename Compare,int N,int M>
void Node<T,Compare,N,M>::insert(int p, const T &v, int pos) noexcept {
  for (int i = ch_cnt; i > p; i--)
    ch_dat[i] = ch_dat[i - 1];
  for (int i = ch_cnt; i > p; i--)
    ch_pos[i] = ch_pos[i - 1];
  ch_dat[p] = v, ch_pos[p] = pos;
  ch_cnt++;
}
template <typename T, typename Compare,int N,int M>
void Node<T,Compare,N,M>::remove(int p) noexcept {
  for (int i = p; i + 1 < ch_cnt; i++)
    ch_dat[i] = ch_dat[i + 1];
  for (int i = p; i + 1 < ch_cnt; i++)
    ch_pos[i] = ch_pos[i + 1];
  ch_dat[ch_cnt - 1] = T::zero(), ch_pos[ch_cnt - 1] = 0;
  ch_cnt--;
}
template <typename T, typename Compare,int N,int M>
int Node<T,Compare,N,M>::findPos(int p) noexcept {
  for (int i = 0; i < ch_cnt; i++)
    if (ch_pos[i] == p)
      return i;
  assert(0);
  return -1;
}
// Node ***********************************************************

// MR *************************************************************
template <typename T, typename Compare,int N,int M>
MemoryRiver<T,Compare,N,M>::MemoryRiver() noexcept {
  // do nothing
}
template <typename T, typename Compare,int N,int M>
void MemoryRiver<T,Compare,N,M>::clear() noexcept {
  std::memset(mem,0,sizeof(mem));
}
template <typename T, typename Compare,int N,int M>
void MemoryRiver<T,Compare,N,M>::readNode(int pos, FileOperator &fo,
                                       Node<T,Compare,N,M> &dst) noexcept {
  if (mem[pos % mr_siz].pos != pos) {
    if (mem[pos % mr_siz].pos != 0)
      fo.write(mem[pos % mr_siz].pos, &mem[pos % mr_siz]);
    fo.read(pos, &mem[pos % mr_siz]);
  }
  dst = mem[pos % mr_siz];
}
template <typename T, typename Compare,int N,int M>
void MemoryRiver<T,Compare,N,M>::writeNode(Node<T,Compare,N,M> u,
                                        FileOperator &fo) noexcept {
  // std::cout<<"Write an node at pos "<<u.pos<<std::endl;
  if (mem[u.pos % mr_siz].pos != u.pos && mem[u.pos % mr_siz].pos != 0) {
    fo.write(mem[u.pos % mr_siz].pos, &mem[u.pos % mr_siz]);
  }
  if (u.pos >= fo.size())
    fo.write(u.pos, &u);
  mem[u.pos % mr_siz] = u;
}
// MR *************************************************************

// BPT  ***********************************************************

template <typename T, typename Compare,int N,int M>
void BPlusTree<T,Compare,N,M>::updateAncestors(int uid, int vp) noexcept {
  Node<T,Compare,N,M> v(vp, fo, mr);

  while (uid != 0) {
    Node<T,Compare,N,M> u(uid, fo, mr);
    int i = u.findPos(v.pos);
    u.ch_dat[i] = v.maxElement();
    u.write(fo, mr);

    if (i != u.ch_cnt - 1)
      break;
    v = u, uid = u.fth;
  }
}
template <typename T, typename Compare,int N,int M>
std::pair<int, int> BPlusTree<T,Compare,N,M>::innerInsert(int uid,
                                                       const T &t) noexcept {
  Node<T,Compare,N,M> u(uid, fo, mr);
  int ips =
      utils::lowerBound(u.ch_dat, u.ch_dat + u.ch_cnt, t, comp) - u.ch_dat;
  assert(ips != u.ch_cnt);

  if (u.isLeaf())
    u.insert(ips, t, 0);
  else {
    std::pair<int, int> pr = innerInsert(u.ch_pos[ips], t);
    if (pr.first != 0) {
      u.ch_pos[ips] = pr.second;
      u.insert(ips, Node<T,Compare,N,M>(pr.first, fo, mr).maxElement(), pr.first);
    }
  }

  if (u.ch_cnt == Node<T,Compare,N,M>::max_ch_cnt + 1) {
    Node<T,Compare,N,M> &L = u, R(fo.size(), fo, mr);
    if (L.isLeaf())
      R.nxt = L.nxt, L.nxt = R.pos;
    R.fth = L.fth;

    int lsz = (u.ch_cnt + 1) / 2, rsz = u.ch_cnt - lsz;
    for (int i = lsz; i < u.ch_cnt; i++)
      R.ch_pos[i - lsz] = L.ch_pos[i], L.ch_pos[i] = 0;
    for (int i = lsz; i < u.ch_cnt; i++)
      R.ch_dat[i - lsz] = L.ch_dat[i], L.ch_dat[i] = T::zero();
    L.ch_cnt = lsz, R.ch_cnt = rsz;

    L.write(fo, mr), R.write(fo, mr);
    if (!R.isLeaf()) {
      for (int i = 0; i < R.ch_cnt; i++) {
        Node<T,Compare,N,M> tmp(R.ch_pos[i], fo, mr);
        tmp.fth = R.pos;
        tmp.write(fo, mr);
      }
    }
    return std::pair<int, int>(L.pos, R.pos);
  }
  u.write(fo, mr);
  return std::pair<int, int>(0, 0);
}
template <typename T, typename Compare,int N,int M>
void BPlusTree<T,Compare,N,M>::innerRemove(int uid, int vid, const T &t,
                                        bool ulev) noexcept {
  Node<T,Compare,N,M> u(uid, fo, mr);
  int ips =
      utils::lowerBound(u.ch_dat, u.ch_dat + u.ch_cnt, t, comp) - u.ch_dat;
  assert(ips != u.ch_cnt);

  if (u.isLeaf()) {
    // std::cout<<"Find deleting element at Node #"<<uid<<" 's "<<ips<<"th
    // pos."<<std::endl;
    u.remove(ips);
    u.write(fo, mr);
    if (ips == u.ch_cnt && u.ch_cnt > 0)
      updateAncestors(u.fth, u.pos);
    if (u.ch_cnt >= (Node<T,Compare,N,M>::max_ch_cnt) / 2)
      return;
  } else {
    innerRemove(u.ch_pos[ips],
                u.ch_pos[ips == u.ch_cnt - 1 ? ips - 1 : ips + 1], t,
                ips != u.ch_cnt - 1);

    u = Node<T,Compare,N,M>(uid, fo, mr);
    if (u.ch_cnt >= (Node<T,Compare,N,M>::max_ch_cnt) / 2)
      return;
  }

  if (u.pos == rt_pos) {
    if (u.ch_cnt == 1 && u.ch_pos[0] != 0) {
      Node<T,Compare,N,M> tmp(u.ch_pos[0], fo, mr);
      tmp.fth = 0;
      tmp.write(fo, mr);
      rt_pos = u.ch_pos[0];
    }
    return;
  }

  Node<T,Compare,N,M> v(vid, fo, mr);
  if (v.ch_cnt + u.ch_cnt <= Node<T,Compare,N,M>::max_ch_cnt) {
    if (!ulev) // make sure u<v
    {
      std::swap(u, v);
    }
    // std::cout<<"Merge it with Node #"<<vid<<std::endl;

    if (u.isLeaf())
      u.nxt = v.nxt;
    for (int i = 0; i < v.ch_cnt; i++) {
      u.ch_pos[u.ch_cnt + i] = v.ch_pos[i];
      u.ch_dat[u.ch_cnt + i] = v.ch_dat[i];

      if (!u.isLeaf()) {
        Node<T,Compare,N,M> tmp(v.ch_pos[i], fo, mr);
        tmp.fth = u.pos;
        tmp.write(fo, mr);
      }
    }
    u.ch_cnt += v.ch_cnt;
    u.write(fo, mr);

    updateAncestors(u.fth, u.pos);

    if (u.fth != 0) {
      Node<T,Compare,N,M> f(u.fth, fo, mr);
      f.remove(f.findPos(v.pos));
      f.write(fo, mr);
    }
    // std::cout<<"Merged. Below is an testing debug."<<std::endl;
    // debugPrint();
  } else {
    if (ulev) {
      u.insert(u.ch_cnt, v.ch_dat[0], v.ch_pos[0]);

      if (v.ch_pos[0] != 0) {
        Node<T,Compare,N,M> tmp(v.ch_pos[0], fo, mr);
        tmp.fth = u.pos;
        tmp.write(fo, mr);
      }

      v.remove(0);
      u.write(fo, mr), v.write(fo, mr);

      updateAncestors(u.fth, u.pos);
    } else {
      u.insert(0, v.ch_dat[v.ch_cnt - 1], v.ch_pos[v.ch_cnt - 1]);

      if (v.ch_pos[v.ch_cnt - 1] != 0) {
        Node<T,Compare,N,M> tmp(v.ch_pos[v.ch_cnt - 1], fo, mr);
        tmp.fth = u.pos;
        tmp.write(fo, mr);
      }

      v.remove(v.ch_cnt - 1);
      u.write(fo, mr), v.write(fo, mr);

      updateAncestors(v.fth, v.pos);
    }
  }
}
template <typename T, typename Compare,int N,int M>
std::pair<int, int> BPlusTree<T,Compare,N,M>::lowerBound(const T &t) noexcept {
  Node<T,Compare,N,M> u(rt_pos, fo, mr);
  while (!u.isLeaf()) {
    int v_id =
        utils::lowerBound(u.ch_dat, u.ch_dat + u.ch_cnt, t, comp) - u.ch_dat;
    assert(v_id < u.ch_cnt);
    u = Node<T,Compare,N,M>(u.ch_pos[v_id], fo, mr);
  }
  return std::make_pair(
      u.pos,
      utils::lowerBound(u.ch_dat, u.ch_dat + u.ch_cnt, t, comp) - u.ch_dat);
}

template <typename T, typename Compare,int N,int M>
BPlusTree<T,Compare,N,M>::BPlusTree(const std::string &file_name) noexcept
    : comp(), fo(file_name), siz(0), mr() {
  if (fo.size() != 0) {
    fo.read(0, &rt_pos);
    fo.read(sizeof(int), &siz);
    return;
  }

  // std::cout<<"Initializing BPT "<<file_name<<std::endl;
  Node<T,Compare,N,M> u(sizeof(int) * 2, fo, mr);
  // std::cout<<"Node constructed"<<std::endl;
  u.ch_cnt = 1, u.ch_dat[0] = T::positiveInfinity();
  // std::cout<<"dat0 constructed"<<std::endl;
  fo.write(0, &u.pos);
  fo.write(sizeof(int), &siz);
  u.write(fo, mr);
  rt_pos = u.pos;

  // u=Node<T,Compare>(sizeof(int)*2,fo,mr);
  // std::cout<<"root's ch_cnt = "<<u.ch_cnt<<std::endl;
}
template <typename T, typename Compare,int N,int M>
BPlusTree<T,Compare,N,M>::~BPlusTree() noexcept {
  fo.write(0, &rt_pos);
  fo.write(sizeof(int), &siz);

  for (int i = 0; i < MemoryRiver<T,Compare,N,M>::mr_siz; i++) {
    if (mr.mem[i].pos != 0)
      fo.write(mr.mem[i].pos, &mr.mem[i]);
  }
}
template <typename T, typename Compare,int N,int M>
void BPlusTree<T,Compare,N,M>::clear() noexcept {
  fo.clear(),siz=0,mr.clear();
  Node<T,Compare,N,M> u(sizeof(int) * 2, fo, mr);
  u.ch_cnt = 1, u.ch_dat[0] = T::positiveInfinity();
  fo.write(0, &u.pos);
  fo.write(sizeof(int), &siz);
  u.write(fo, mr);
  rt_pos = u.pos;
}

template <typename T, typename Compare,int N,int M>
void BPlusTree<T,Compare,N,M>::insert(const T &t) noexcept {
  // std::cout<<"Inserting"<<std::endl;
  siz++;
  // std::cout<<"inner insert start"<<std::endl;
  std::pair<int, int> pr = innerInsert(rt_pos, t);
  // std::cout<<"inner insert done"<<std::endl;
  if (pr.first == 0)
    return;

  Node<T,Compare,N,M> u(pr.first, fo, mr), v(pr.second, fo, mr),
      w(fo.size(), fo, mr);
  rt_pos = u.fth = v.fth = w.pos;
  w.ch_cnt = 2;
  w.ch_pos[0] = u.pos, w.ch_dat[0] = u.maxElement();
  w.ch_pos[1] = v.pos, w.ch_dat[1] = v.maxElement();
  u.write(fo, mr), v.write(fo, mr), w.write(fo, mr);
}
template <typename T, typename Compare,int N,int M>
bool BPlusTree<T,Compare,N,M>::remove(const T &t) noexcept {
  std::pair<int, int> pr = lowerBound(t);
  if (comp(t, Node<T,Compare,N,M>(pr.first, fo, mr).ch_dat[pr.second]))
    return 0;

  innerRemove(rt_pos, 0, t, 0);
  siz--;
  return 1;
}
template <typename T, typename Compare,int N,int M>
sjtu::vector<T> BPlusTree<T,Compare,N,M>::find(const T &l, const T &r) noexcept {
  std::pair<int, int> pr = lowerBound(l);
  // std::cout<<"l at Node#"<<pr.first<<" 's "<<pr.second<<" th child"<<std::endl;

  sjtu::vector<T> res;
  for (Node<T,Compare,N,M> u(pr.first, fo, mr); u.pos;
       u = Node<T,Compare,N,M>(u.nxt, fo, mr)) {
    for (int i = (u.pos == pr.first ? pr.second : 0); i < u.ch_cnt; i++) {
      if (!comp(u.ch_dat[i], r))
      {
        // std::cout<<"The first ge is Node #"<<u.pos<<"'s "<<i<<"th element"<<std::endl;
        return res;
      }
      res.push_back(u.ch_dat[i]);
    }
  }
  return res;
}
template <typename T, typename Compare,int N,int M>
int BPlusTree<T,Compare,N,M>::size() noexcept {
  return siz;
}

template <typename T, typename Compare,int N,int M>
void BPlusTree<T,Compare,N,M>::innerDebugPrint(int uid) noexcept {
  Node<T,Compare,N,M> u(uid, fo, mr);
  std::cout << "Node #" << uid << " : pos = " << u.pos
            << " , ch_cnt = " << u.ch_cnt << " , fth = " << u.fth
            << " , nxt = " << u.nxt << std::endl;
  std::cout << "     Child pos : ";
  for (int i = 0; i < u.ch_cnt; i++)
    std::cout << u.ch_pos[i] << " ";
  std::cout << std::endl;
  std::cout << "     Child mxv : ";
  for (int i = 0; i < u.ch_cnt; i++)
    std::cout << u.ch_dat[i] << " ";
  std::cout << std::endl;

  if (u.ch_pos[0] == 0)
    return;
  for (int i = 0; i < u.ch_cnt; i++)
    innerDebugPrint(u.ch_pos[i]);
}
template <typename T, typename Compare,int N,int M>
void BPlusTree<T,Compare,N,M>::debugPrint() noexcept {
  std::cout << "BPT ***************************" << std::endl;

  innerDebugPrint(rt_pos);

  std::cout << "*******************************" << std::endl;
}
// BPT  ***********************************************************