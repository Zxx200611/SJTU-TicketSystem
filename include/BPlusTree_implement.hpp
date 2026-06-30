#include <BPlusTree.hpp>
#pragma GCC optimize(2)
#pragma pack(1)
// Node ***********************************************************
template <typename T, typename Hash,int N,int M> Node<T, Hash,N,M>::Node() noexcept {
  pos = 0, ch_cnt = 0;
  memset(ch_pos, 0, sizeof(ch_pos));
  memset(ch_dat, 0, sizeof(ch_dat));
  fth = nxt = 0;
}
template <typename T, typename Hash,int N,int M>
Node<T, Hash,N,M>::Node(int this_pos, FileOperator &fo,
                       MemoryRiver<T, Hash,N,M> &mr) noexcept {
  if (this_pos >= fo.size() || this_pos == 0) {
    // std::cout<<"Initializing node"<<std::endl;
    pos = this_pos, ch_cnt = 0;
    memset(ch_pos, 0, sizeof(ch_pos));
    memset(ch_dat, 0, sizeof(ch_dat));
    fth = nxt = 0;
  } else
    mr.readNode(this_pos, fo, *this);
}
template <typename T, typename Hash,int N,int M>
Node<T,Hash,N,M> &Node<T,Hash,N,M>::operator=(const Node<T,Hash,N,M> &b) {
  if (&b == this)
    return *this;

  memcpy(this, &b, sizeof(Node));
  pos = b.pos, ch_cnt = b.ch_cnt, fth = b.fth, nxt = b.nxt;
  for (int i = 0; i < ch_cnt; i++)
    ch_pos[i] = b.ch_pos[i];
  for (int i = 0; i < ch_cnt; i++)
    ch_dat[i] = b.ch_dat[i];
  return *this;
}
template <typename T, typename Hash,int N,int M>
void Node<T,Hash,N,M>::write(FileOperator &fo,
                             MemoryRiver<T,Hash,N,M> &mr) noexcept {
  mr.writeNode(*this, fo);
}
template <typename T, typename Hash,int N,int M>
constexpr HashResult Node<T,Hash,N,M>::maxElement() noexcept {
  return ch_dat[ch_cnt - 1];
}
template <typename T, typename Hash,int N,int M>
constexpr bool Node<T,Hash,N,M>::isLeaf() noexcept {
  return ch_pos[0] == 0;
}
template <typename T, typename Hash,int N,int M>
void Node<T,Hash,N,M>::insert(int p, const HashResult &v, int pos) noexcept {
    if (p < ch_cnt) {
        memmove(ch_dat + p + 1, ch_dat + p, (ch_cnt - p) * sizeof(HashResult));
        memmove(ch_pos + p + 1, ch_pos + p, (ch_cnt - p) * sizeof(int));
    }
    ch_dat[p] = v;
    ch_pos[p] = pos;
    ch_cnt++;
}
template <typename T, typename Hash,int N,int M>
void Node<T,Hash,N,M>::remove(int p) noexcept {
    if (p + 1 < ch_cnt) {
        memmove(ch_dat + p, ch_dat + p + 1, (ch_cnt - p - 1) * sizeof(HashResult));
        memmove(ch_pos + p, ch_pos + p + 1, (ch_cnt - p - 1) * sizeof(int));
    }
    ch_cnt--;
    memset(ch_dat + ch_cnt, 0, sizeof(HashResult));
    memset(ch_pos + ch_cnt, 0, sizeof(int));
}
template <typename T, typename Hash,int N,int M>
int Node<T,Hash,N,M>::findPos(int p) noexcept {
  for (int i = 0; i < ch_cnt; i++)
    if (ch_pos[i] == p)
      return i;
  assert(0);
  return -1;
}
// Node ***********************************************************

// MR *************************************************************
template <typename T, typename Hash,int N,int M>
MemoryRiver<T,Hash,N,M>::MemoryRiver() noexcept {
  // do nothing
}
template <typename T, typename Hash,int N,int M>
void MemoryRiver<T,Hash,N,M>::clear() noexcept {
  std::memset(mem,0,sizeof(mem));
}
template <typename T, typename Hash,int N,int M>
void MemoryRiver<T,Hash,N,M>::readNode(int pos, FileOperator &fo,
                                       Node<T,Hash,N,M> &dst) noexcept {
  if (mem[pos % mr_siz].pos != pos) {
    if (mem[pos % mr_siz].pos != 0)
      fo.write(mem[pos % mr_siz].pos, &mem[pos % mr_siz]);
    fo.read(pos, &mem[pos % mr_siz]);
  }
  dst = mem[pos % mr_siz];
}
template <typename T, typename Hash,int N,int M>
void MemoryRiver<T,Hash,N,M>::writeNode(Node<T,Hash,N,M> u,
                                        FileOperator &fo) noexcept {
  // std::cout<<"Write an node at pos "<<u.pos<<std::endl;
  if (mem[u.pos % mr_siz].pos != u.pos && mem[u.pos % mr_siz].pos != 0) {
    fo.write(mem[u.pos % mr_siz].pos, &mem[u.pos % mr_siz]);
  }
  if(u.pos>=fo.size()) fo.write(u.pos, &u);
  mem[u.pos % mr_siz] = u;
}
template <typename T, typename Hash,int N,int M>
void MemoryRiver<T,Hash,N,M>::invalidate(int pos) noexcept {
  mem[pos%mr_siz].pos=0;
}
// MR *************************************************************

// HashResult *****************************************************
HashResult::HashResult() noexcept:pos(0)
{
  memset(t,0,sizeof(t));
}
bool HashResult::comp(const HashResult &a,const HashResult &b) noexcept
{
  for(int i=0;i<HashResult::hash_siz;i++)
  {
    if(a.t[i]!=b.t[i]) return a.t[i]<b.t[i];
  }
  return 0;
}
inline HashResult HashResult::zero()noexcept{return HashResult();}
inline HashResult HashResult::positiveInfinity()noexcept
{
  HashResult res;
  for(int i=0;i<HashResult::hash_siz;i++) res.t[i]=INT_MAX;
  res.pos=INT_MAX;
  return res;
}
// HashResult *****************************************************

// BPT  ***********************************************************

template <typename T, typename Hash,int N,int M>
void BPlusTree<T,Hash,N,M>::updateAncestors(int uid, int vp) noexcept {
  Node<T,Hash,N,M> v(vp, fo, mr);

  while (uid != 0) {
    Node<T,Hash,N,M> u(uid, fo, mr);
    int i = u.findPos(v.pos);
    u.ch_dat[i] = v.maxElement();
    u.write(fo, mr);

    if (i != u.ch_cnt - 1)
      break;
    v = u, uid = u.fth;
  }
}
template <typename T, typename Hash,int N,int M>
std::pair<int, int> BPlusTree<T,Hash,N,M>::innerInsert(int uid,
                                                       const HashResult &t) noexcept {
  Node<T,Hash,N,M> u(uid, fo, mr);
  int ips =
      utils::lowerBound(u.ch_dat, u.ch_dat + u.ch_cnt, t, HashResult::comp) - u.ch_dat;
  assert(ips != u.ch_cnt);

  if (u.isLeaf())
    u.insert(ips, t, 0);
  else {
    std::pair<int, int> pr = innerInsert(u.ch_pos[ips], t);
    if (pr.first != 0) {
      u.ch_pos[ips] = pr.second;
      u.insert(ips, Node<T,Hash,N,M>(pr.first, fo, mr).maxElement(), pr.first);
    }
  }

  if (u.ch_cnt == Node<T,Hash,N,M>::max_ch_cnt + 1) {
    Node<T,Hash,N,M> &L = u, R;
    R.pos=fo.allocate();
    if (L.isLeaf())
      R.nxt = L.nxt, L.nxt = R.pos;
    R.fth = L.fth;

    int lsz = (u.ch_cnt + 1) / 2, rsz = u.ch_cnt - lsz;
    for (int i = lsz; i < u.ch_cnt; i++)
      R.ch_pos[i - lsz] = L.ch_pos[i], L.ch_pos[i] = 0;
    for (int i = lsz; i < u.ch_cnt; i++)
      R.ch_dat[i - lsz] = L.ch_dat[i], L.ch_dat[i] = HashResult::zero();
    L.ch_cnt = lsz, R.ch_cnt = rsz;

    L.write(fo, mr), R.write(fo, mr);
    if (!R.isLeaf()) {
      for (int i = 0; i < R.ch_cnt; i++) {
        Node<T,Hash,N,M> tmp(R.ch_pos[i], fo, mr);
        tmp.fth = R.pos;
        tmp.write(fo, mr);
      }
    }
    return std::pair<int, int>(L.pos, R.pos);
  }
  u.write(fo, mr);
  return std::pair<int, int>(0, 0);
}
template <typename T, typename Hash,int N,int M>
void BPlusTree<T,Hash,N,M>::innerRemove(int uid, int vid, const HashResult &t,bool ulev) noexcept {
  Node<T,Hash,N,M> u(uid, fo, mr);
  int ips =
      utils::lowerBound(u.ch_dat, u.ch_dat + u.ch_cnt, t, HashResult::comp) - u.ch_dat;
  assert(ips != u.ch_cnt);

  if (u.isLeaf()) {
    // std::cout<<"Find deleting element at Node #"<<uid<<" 's "<<ips<<"th
    // pos."<<std::endl;
    u.remove(ips);
    u.write(fo, mr);
    if (ips == u.ch_cnt && u.ch_cnt > 0)
      updateAncestors(u.fth, u.pos);
    if (u.ch_cnt >= (Node<T,Hash,N,M>::max_ch_cnt) / 2)
      return;
  } else {
    innerRemove(u.ch_pos[ips],
                u.ch_pos[ips == u.ch_cnt - 1 ? ips - 1 : ips + 1], t,
                ips != u.ch_cnt - 1);

    u = Node<T,Hash,N,M>(uid, fo, mr);
    if (u.ch_cnt >= (Node<T,Hash,N,M>::max_ch_cnt) / 2)
      return;
  }

  if (u.pos == rt_pos) {
    if (u.ch_cnt == 1 && u.ch_pos[0] != 0) {
      Node<T,Hash,N,M> tmp(u.ch_pos[0], fo, mr);
      tmp.fth = 0;
      tmp.write(fo, mr);
      mr.invalidate(rt_pos);
      fo.deallocate(rt_pos);
      rt_pos = u.ch_pos[0];
    }
    return;
  }

  Node<T,Hash,N,M> v(vid, fo, mr);
  if (v.ch_cnt + u.ch_cnt <= Node<T,Hash,N,M>::max_ch_cnt) {
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
        Node<T,Hash,N,M> tmp(v.ch_pos[i], fo, mr);
        tmp.fth = u.pos;
        tmp.write(fo, mr);
      }
    }
    u.ch_cnt += v.ch_cnt;
    u.write(fo, mr);

    updateAncestors(u.fth, u.pos);

    if (u.fth != 0) {
      Node<T,Hash,N,M> f(u.fth, fo, mr);
      f.remove(f.findPos(v.pos));
      f.write(fo, mr);
    }
    mr.invalidate(v.pos);
    fo.deallocate(v.pos);
    // std::cout<<"Merged. Below is an testing debug."<<std::endl;
    // debugPrint();
  } else {
    if (ulev) {
      u.insert(u.ch_cnt, v.ch_dat[0], v.ch_pos[0]);

      if (v.ch_pos[0] != 0) {
        Node<T,Hash,N,M> tmp(v.ch_pos[0], fo, mr);
        tmp.fth = u.pos;
        tmp.write(fo, mr);
      }

      v.remove(0);
      u.write(fo, mr), v.write(fo, mr);

      updateAncestors(u.fth, u.pos);
    } else {
      u.insert(0, v.ch_dat[v.ch_cnt - 1], v.ch_pos[v.ch_cnt - 1]);

      if (v.ch_pos[v.ch_cnt - 1] != 0) {
        Node<T,Hash,N,M> tmp(v.ch_pos[v.ch_cnt - 1], fo, mr);
        tmp.fth = u.pos;
        tmp.write(fo, mr);
      }

      v.remove(v.ch_cnt - 1);
      u.write(fo, mr), v.write(fo, mr);

      updateAncestors(v.fth, v.pos);
    }
  }
}
template <typename T, typename Hash,int N,int M>
std::pair<int, int> BPlusTree<T,Hash,N,M>::lowerBound(const HashResult &t) noexcept {
  Node<T,Hash,N,M> u(rt_pos, fo, mr);
  while (!u.isLeaf()) {
    int v_id =
        utils::lowerBound(u.ch_dat, u.ch_dat + u.ch_cnt, t, HashResult::comp) - u.ch_dat;
    assert(v_id < u.ch_cnt);
    u = Node<T,Hash,N,M>(u.ch_pos[v_id], fo, mr);
  }
  return std::make_pair(
      u.pos,
      utils::lowerBound(u.ch_dat, u.ch_dat + u.ch_cnt, t, HashResult::comp) - u.ch_dat);
}

template <typename T, typename Hash,int N,int M>
BPlusTree<T,Hash,N,M>::BPlusTree(const std::string &name) noexcept: hash(), fo(name+"_node.dat"), dfo(name+"_data.dat"), siz(0), mr() {
  if (fo.size() != 0) {
    fo.read(0, &rt_pos);
    fo.read(sizeof(int), &siz);
    return;
  }

  // std::cerr<<"Initializing BPT "<<name<<std::endl;
  Node<T,Hash,N,M> u(sizeof(int) * 2, fo, mr);
  // std::cout<<"Node constructed"<<std::endl;
  u.ch_cnt = 1, u.ch_dat[0] = HashResult::positiveInfinity();
  // std::cout<<"dat0 constructed"<<std::endl;
  fo.write(0, &u.pos);
  fo.write(sizeof(int), &siz);
  u.write(fo, mr);
  rt_pos = u.pos;

  // u=Node<T,Hash>(sizeof(int)*2,fo,mr);
  // debugPrint(std::cerr);
  // std::cerr<<"root's ch_cnt = "<<u.ch_cnt<<std::endl;
}
template <typename T, typename Hash,int N,int M>
BPlusTree<T,Hash,N,M>::~BPlusTree() noexcept {
  fo.write(0, &rt_pos);
  fo.write(sizeof(int), &siz);

  for (int i = 0; i < MemoryRiver<T,Hash,N,M>::mr_siz; i++) {
    if (mr.mem[i].pos != 0)
      fo.write(mr.mem[i].pos, &mr.mem[i]);
  }
}
template <typename T, typename Hash,int N,int M>
void BPlusTree<T,Hash,N,M>::clear() noexcept {
  fo.clear(),siz=0,mr.clear(),dfo.clear();
  Node<T,Hash,N,M> u(sizeof(int) * 2, fo, mr);
  u.ch_cnt = 1, u.ch_dat[0] = HashResult::positiveInfinity();
  fo.write(0, &u.pos);
  fo.write(sizeof(int), &siz);
  u.write(fo, mr);
  rt_pos = u.pos;
}

template <typename T, typename Hash,int N,int M>
void BPlusTree<T,Hash,N,M>::insert(const T &t) noexcept {
  siz++;
  HashResult h=hash(t);h.pos=dfo.allocate();
  dfo.write(h.pos,&t);
  std::pair<int, int> pr = innerInsert(rt_pos, h);
  if (pr.first == 0)
    return;

  Node<T,Hash,N,M> u(pr.first, fo, mr), v(pr.second, fo, mr),w;
  w.pos=fo.allocate();

  rt_pos = u.fth = v.fth = w.pos;
  w.ch_cnt = 2;
  w.ch_pos[0] = u.pos, w.ch_dat[0] = u.maxElement();
  w.ch_pos[1] = v.pos, w.ch_dat[1] = v.maxElement();
  u.write(fo, mr), v.write(fo, mr), w.write(fo, mr);
}
template <typename T, typename Hash,int N,int M>
bool BPlusTree<T,Hash,N,M>::remove(const T &t) noexcept {
  HashResult h=hash(t);
  std::pair<int, int> pr = lowerBound(h);
  if (HashResult::comp(h, Node<T,Hash,N,M>(pr.first, fo, mr).ch_dat[pr.second]))
    return 0;
  dfo.deallocate(Node<T,Hash,N,M>(pr.first, fo, mr).ch_dat[pr.second].pos);

  innerRemove(rt_pos, 0, h, 0);
  siz--;
  return 1;
}
template <typename T, typename Hash,int N,int M>
sjtu::vector<T> BPlusTree<T,Hash,N,M>::find(const HashResult &lh, const HashResult &rh) noexcept {
  std::pair<int, int> pr = lowerBound(lh);
  // std::cerr<<"l at Node#"<<pr.first<<" 's "<<pr.second<<" th child"<<std::endl;

  sjtu::vector<T> res;
  for (Node<T,Hash,N,M> u(pr.first, fo, mr); u.pos; u = Node<T,Hash,N,M>(u.nxt, fo, mr)) {
    for (int i = (u.pos == pr.first ? pr.second : 0); i < u.ch_cnt; i++) {
      if (!HashResult::comp(u.ch_dat[i], rh))
      {
        // std::cerr<<"The first ge is Node #"<<u.pos<<"'s "<<i<<"th element"<<std::endl;
        // std::cerr<<u.ch_dat[i].t[0]<<" "<<u.ch_dat[i].t[1]<<std::endl;
        // std::cerr<<rh.t[0]<<" "<<rh.t[1]<<std::endl;
        return res;
      }

      static T tmp;
      // std::cout<<"read at data pos "<<u.ch_dat[i].pos<<std::endl;
      dfo.read(u.ch_dat[i].pos,&tmp);
      res.push_back(tmp);
    }
  }
  return res;
}
template <typename T, typename Hash,int N,int M>
sjtu::vector<T> BPlusTree<T,Hash,N,M>::find(const T &l, const T &r) noexcept {
  HashResult lh=hash(l),rh=hash(r);
  return find(lh,rh);
}
template <typename T, typename Hash,int N,int M>
int BPlusTree<T,Hash,N,M>::findFirstGe(const HashResult &h,T &res) noexcept {
  std::pair<int, int> pr = lowerBound(h);
  // std::cout<<"l at Node#"<<pr.first<<" 's "<<pr.second<<" th child"<<std::endl;

  Node<T,Hash,N,M> u(pr.first, fo, mr);
  if(!HashResult::comp(u.ch_dat[pr.second],HashResult::positiveInfinity())) return -1;
  dfo.read(u.ch_dat[pr.second].pos,&res);
  return u.ch_dat[pr.second].pos;
}
template <typename T, typename Hash,int N,int M>
int BPlusTree<T,Hash,N,M>::findFirstGe(const T &t,T &res) noexcept {
  std::pair<int, int> pr = lowerBound(hash(t));
  // std::cout<<"l at Node#"<<pr.first<<" 's "<<pr.second<<" th child"<<std::endl;

  Node<T,Hash,N,M> u(pr.first, fo, mr);
  if(!HashResult::comp(u.ch_dat[pr.second],HashResult::positiveInfinity())) return -1;
  dfo.read(u.ch_dat[pr.second].pos,&res);
  return u.ch_dat[pr.second].pos;
}
template <typename T, typename Hash,int N,int M>
int BPlusTree<T,Hash,N,M>::size() noexcept {
  return siz;
}

template <typename T, typename Hash,int N,int M>
void BPlusTree<T,Hash,N,M>::innerDebugPrint(int uid,std::ostream &os) noexcept {
  Node<T,Hash,N,M> u(uid, fo, mr);
  os << "Node #" << uid << " : pos = " << u.pos
            << " , ch_cnt = " << u.ch_cnt << " , fth = " << u.fth
            << " , nxt = " << u.nxt << std::endl;
  os << "     Child pos : ";
  for (int i = 0; i < u.ch_cnt; i++)
    os << u.ch_pos[i] << " ";
  os << std::endl;
  os << "     Child mxv : ";
  for (int i = 0; i < u.ch_cnt; i++)
    os << u.ch_dat[i] << " ";
  os << std::endl;

  if (u.ch_pos[0] == 0)
    return;
  for (int i = 0; i < u.ch_cnt; i++)
    innerDebugPrint(u.ch_pos[i],os);
}
template <typename T, typename Hash,int N,int M>
int BPlusTree<T,Hash,N,M>::locate(const HashResult &h) noexcept
{
  std::pair<int, int> pr = lowerBound(h);
  Node<T,Hash,N,M> u(pr.first, fo, mr);
  if(HashResult::comp(h,u.ch_dat[pr.second])) return -1;
  return u.ch_dat[pr.second].pos;
}

template <typename T, typename Hash,int N,int M>
void BPlusTree<T,Hash,N,M>::debugPrint(std::ostream &os) noexcept {
  os << "BPT ***************************" << std::endl;

  innerDebugPrint(rt_pos,os);

  os << "*******************************" << std::endl;
}
// BPT  ***********************************************************