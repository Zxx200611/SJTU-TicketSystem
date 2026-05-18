#include<BPlusTree.hpp>

// Node ***********************************************************
template<typename T,typename Compare>
Node<T,Compare>::Node(int this_pos,FileOperator &fo) noexcept
{
    if(this_pos>=fo.size())
    {
        // std::cout<<"Initializing node"<<std::endl;
        pos=this_pos,ch_cnt=0;
        memset(ch_pos,0,sizeof(ch_pos));
        for(int i=0;i<=max_ch_cnt;i++) ch_dat[i]=T::zero();
        fth=nxt=0;
    }
    else fo.read(this_pos,this);
}
template<typename T,typename Compare>
Node<T,Compare>& Node<T,Compare>::operator = (const Node<T,Compare> &b)
{
    if(&b==this) return *this;

    pos=b.pos,ch_cnt=b.ch_cnt,fth=b.fth,nxt=b.nxt;
    for(int i=0;i<ch_cnt;i++) ch_pos[i]=b.ch_pos[i];
    for(int i=0;i<ch_cnt;i++) ch_dat[i]=b.ch_dat[i];
    return *this;
}
template<typename T,typename Compare>
void Node<T,Compare>::write(FileOperator &fo) noexcept
{
    fo.write(pos,this);
}
template<typename T,typename Compare>
T Node<T,Compare>::maxElement() noexcept
{
    return ch_dat[ch_cnt-1];
}
template<typename T,typename Compare>
bool Node<T,Compare>::isLeaf() noexcept
{
    return ch_pos[0]==0;
}
template<typename T,typename Compare>
void Node<T,Compare>::insert(int p,const T &v,int pos) noexcept
{
    for(int i=ch_cnt;i>p;i--) ch_dat[i]=ch_dat[i-1];
    for(int i=ch_cnt;i>p;i--) ch_pos[i]=ch_pos[i-1];
    ch_dat[p]=v,ch_pos[p]=pos;
    ch_cnt++;
}
template<typename T,typename Compare>
void Node<T,Compare>::remove(int p) noexcept
{
    for(int i=p;i+1<ch_cnt;i++) ch_dat[i]=ch_dat[i+1];
    for(int i=p;i+1<ch_cnt;i++) ch_pos[i]=ch_pos[i+1];
    ch_dat[ch_cnt-1]=T::zero(),ch_pos[ch_cnt-1]=0;
    ch_cnt--;
}
template<typename T,typename Compare>
int Node<T,Compare>::findPos(int p) noexcept
{
    for(int i=0;i<ch_cnt;i++) if(ch_pos[i]==p) return i;
    assert(0);
    return -1;
}
// Node ***********************************************************

// BPT  ***********************************************************

template<typename T,typename Compare>
void BPlusTree<T,Compare>::updateAncestors(int uid,int vp) noexcept
{
    if(uid==0) return;

    Node<T,Compare> u(uid,fo),v(vp,fo);
    int i=u.findPos(vp);
    u.ch_dat[i]=v.maxElement();
    u.write(fo);

    if(i==u.ch_cnt-1) updateAncestors(u.fth,uid);
}
template<typename T,typename Compare>
std::pair<int,int> BPlusTree<T,Compare>::innerInsert(int uid,const T &t) noexcept
{
    Node<T,Compare> u(uid,fo);
    int ips=utils::lowerBound(u.ch_dat,u.ch_dat+u.ch_cnt,t,comp)-u.ch_dat;
    assert(ips!=u.ch_cnt);

    if(u.isLeaf()) u.insert(ips,t,0);
    else
    {
        std::pair<int,int> pr=innerInsert(u.ch_pos[ips],t);
        if(pr.first!=0)
        {
            u.ch_pos[ips]=pr.second;
            u.insert(ips,Node<T,Compare>(pr.first,fo).maxElement(),pr.first);
        }
    }

    if(u.ch_cnt==Node<T,Compare>::max_ch_cnt+1)
    {
        Node<T,Compare> &L=u,R(fo.size(),fo);
        if(L.isLeaf()) R.nxt=L.nxt,L.nxt=R.pos; 
        R.fth=L.fth;

        int lsz=(u.ch_cnt+1)/2,rsz=u.ch_cnt-lsz;
        for(int i=lsz;i<u.ch_cnt;i++) R.ch_pos[i-lsz]=L.ch_pos[i],L.ch_pos[i]=0;
        for(int i=lsz;i<u.ch_cnt;i++) R.ch_dat[i-lsz]=L.ch_dat[i],L.ch_dat[i]=T::zero();
        L.ch_cnt=lsz,R.ch_cnt=rsz;

        L.write(fo),R.write(fo);
        if(!R.isLeaf())
        {
            for(int i=0;i<R.ch_cnt;i++)
            {
                Node<T,Compare> tmp(R.ch_pos[i],fo);
                tmp.fth=R.pos;
                tmp.write(fo);
            }
        }
        return std::pair<int,int>(L.pos,R.pos);
    }
    u.write(fo);
    return std::pair<int,int>(0,0);
}
template<typename T,typename Compare>
void BPlusTree<T,Compare>::innerRemove(int uid,int vid,const T &t,bool ulev) noexcept
{
    Node<T,Compare> u(uid,fo);
    int ips=utils::lowerBound(u.ch_dat,u.ch_dat+u.ch_cnt,t,comp)-u.ch_dat;
    assert(ips!=u.ch_cnt);

    if(u.isLeaf())
    {
        // std::cout<<"Find deleting element at Node #"<<uid<<" 's "<<ips<<"th pos."<<std::endl;
        u.remove(ips);
        u.write(fo);
        if(ips==u.ch_cnt&&u.ch_cnt>0) updateAncestors(u.fth,u.pos);
    }
    else
    {
        innerRemove(u.ch_pos[ips],u.ch_pos[ips==u.ch_cnt-1?ips-1:ips+1],t,ips!=u.ch_cnt-1);
        u=Node<T,Compare>(uid,fo);
    }

    if(u.ch_cnt>=(Node<T,Compare>::max_ch_cnt)/2) return;
    if(u.pos==rt_pos)
    {
        if(u.ch_cnt==1&&u.ch_pos[0]!=0)
        {
            Node<T,Compare> tmp(u.ch_pos[0],fo);
            tmp.fth=0;
            tmp.write(fo);
            rt_pos=u.ch_pos[0];
        }
        return;
    }

    Node<T,Compare> v(vid,fo);
    if(v.ch_cnt+u.ch_cnt<=Node<T,Compare>::max_ch_cnt)
    {
        if(!ulev)    //make sure u<v
        {
            std::swap(u,v);
        }
        // std::cout<<"Merge it with Node #"<<vid<<std::endl;

        if(u.isLeaf()) u.nxt=v.nxt;
        for(int i=0;i<v.ch_cnt;i++)
        {
            u.ch_pos[u.ch_cnt+i]=v.ch_pos[i];
            u.ch_dat[u.ch_cnt+i]=v.ch_dat[i];

            if(!u.isLeaf())
            {
                Node<T,Compare> tmp(v.ch_pos[i],fo);
                tmp.fth=u.pos;
                tmp.write(fo);
            }
        }
        u.ch_cnt+=v.ch_cnt;
        u.write(fo);

        updateAncestors(u.fth,u.pos);
        
        if(u.fth!=0)
        {
            Node<T,Compare> f(u.fth,fo);
            f.remove(f.findPos(v.pos));
            f.write(fo);
        }
        // std::cout<<"Merged. Below is an testing debug."<<std::endl;
        // debugPrint();
    }
    else
    {
        if(ulev)
        {
            u.insert(u.ch_cnt,v.ch_dat[0],v.ch_pos[0]);

            if(v.ch_pos[0]!=0)
            {
                Node<T,Compare> tmp(v.ch_pos[0],fo);
                tmp.fth=u.pos;
                tmp.write(fo);
            }

            v.remove(0);
            u.write(fo),v.write(fo);

            updateAncestors(u.fth,u.pos);
        }
        else
        {
            u.insert(0,v.ch_dat[v.ch_cnt-1],v.ch_pos[v.ch_cnt-1]);

            if(v.ch_pos[v.ch_cnt-1]!=0)
            {
                Node<T,Compare> tmp(v.ch_pos[v.ch_cnt-1],fo);
                tmp.fth=u.pos;
                tmp.write(fo);
            }

            v.remove(v.ch_cnt-1);
            u.write(fo),v.write(fo);

            updateAncestors(v.fth,v.pos);
        }
    }
}
template<typename T,typename Compare>
std::pair<int,int> BPlusTree<T,Compare>::lowerBound(const T &t) noexcept
{
    Node<T,Compare> u(rt_pos,fo);
    while(!u.isLeaf())
    {
        int v_id=utils::lowerBound(u.ch_dat,u.ch_dat+u.ch_cnt,t,comp)-u.ch_dat;
        assert(v_id<u.ch_cnt);
        u=Node<T,Compare>(u.ch_pos[v_id],fo);
    }
    return std::make_pair(u.pos,utils::lowerBound(u.ch_dat,u.ch_dat+u.ch_cnt,t,comp)-u.ch_dat);
}


template<typename T,typename Compare>
BPlusTree<T,Compare>::BPlusTree(const std::string &file_name) noexcept:comp(),fo(file_name),siz(0)
{
    if(fo.size()!=0)
    {
        fo.read(0,&rt_pos);
        fo.read(sizeof(int),&siz);
        return;
    }

    // std::cout<<"Initializing BPT"<<std::endl;
    Node<T,Compare> u(sizeof(int)*2,fo);
    u.ch_cnt=1,u.ch_dat[0]=T::positiveInfinity();
    fo.write(0,&u.pos);
    fo.write(sizeof(int),&siz);
    u.write(fo);
    rt_pos=u.pos;

    u=Node<T,Compare>(sizeof(int)*2,fo);
    // std::cout<<"root's ch_cnt = "<<u.ch_cnt<<std::endl;
}
template<typename T,typename Compare>
BPlusTree<T,Compare>::~BPlusTree() noexcept
{
    fo.write(0,&rt_pos);
    fo.write(sizeof(int),&siz);
}

template<typename T,typename Compare>
void BPlusTree<T,Compare>::insert(const T &t) noexcept
{
    siz++;
    std::pair<int,int> pr=innerInsert(rt_pos,t);
    if(pr.first==0) return;

    Node<T,Compare> u(pr.first,fo),v(pr.second,fo),w(fo.size(),fo);
    rt_pos=u.fth=v.fth=w.pos;
    w.ch_cnt=2;
    w.ch_pos[0]=u.pos,w.ch_dat[0]=u.maxElement();
    w.ch_pos[1]=v.pos,w.ch_dat[1]=v.maxElement();
    u.write(fo),v.write(fo),w.write(fo);
}
template<typename T,typename Compare>
bool BPlusTree<T,Compare>::remove(const T &t) noexcept
{
    std::pair<int,int> pr=lowerBound(t);
    if(comp(t,Node<T,Compare>(pr.first,fo).ch_dat[pr.second])) return 0;

    innerRemove(rt_pos,0,t,0);
    siz--;
    return 1;
}
template<typename T,typename Compare>
sjtu::vector<T> BPlusTree<T,Compare>::find(const T &l,const T&r) noexcept
{
    std::pair<int,int> pr=lowerBound(l);

    sjtu::vector<T> res;
    for(Node<T,Compare> u(pr.first,fo);u.pos;u=Node<T,Compare>(u.nxt,fo))
    {
        for(int i=(u.pos==pr.first?pr.second:0);i<u.ch_cnt;i++)
        {
            if(!comp(u.ch_dat[i],r)) return res;
            res.push_back(u.ch_dat[i]);
        }
    }
    return res;
}
template<typename T,typename Compare>
int BPlusTree<T,Compare>::size() noexcept
{
    return siz;
}

template<typename T,typename Compare>
void BPlusTree<T,Compare>::innerDebugPrint(int uid) noexcept
{
    Node<T,Compare> u(uid,fo);
    std::cout<<"Node #"<<uid<<" : pos = "<<u.pos<<" , ch_cnt = "<<u.ch_cnt<<" , fth = "<<u.fth<<" , nxt = "<<u.nxt<<std::endl;
    std::cout<<"     Child pos : ";
    for(int i=0;i<u.ch_cnt;i++) std::cout<<u.ch_pos[i]<<" ";
    std::cout<<std::endl;
    std::cout<<"     Child mxv : ";
    for(int i=0;i<u.ch_cnt;i++) std::cout<<u.ch_dat[i]<<" ";
    std::cout<<std::endl;

    if(u.ch_pos[0]==0) return;
    for(int i=0;i<u.ch_cnt;i++) innerDebugPrint(u.ch_pos[i]);
}
template<typename T,typename Compare>
void BPlusTree<T,Compare>::debugPrint() noexcept
{
    std::cout<<"BPT ***************************"<<std::endl;

    innerDebugPrint(rt_pos);
    
    std::cout<<"*******************************"<<std::endl;
}
// BPT  ***********************************************************