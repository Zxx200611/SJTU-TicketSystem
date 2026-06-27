#include <BPlusTree.hpp>
#include <iostream>
#include <string>

#pragma pack(1)
struct Element {
  static constexpr int memory_size = 64 * sizeof(char) + sizeof(long long);

  char key[64];
  long long val;

  Element() : key(""), val(0) {}
  Element(const std::string &s, long long x) noexcept : val(x) {
    for (int i = 0; i < s.size(); i++)
      key[i] = s[i];
    key[s.size()] = '\0';
  }
  Element &operator=(const Element &b) {
    strcpy(key, b.key);
    val = b.val;
    return *this;
  }
};
struct ElementHash {
  HashResult operator () (const Element &e) const
  {
    HashResult res;
    res.t[0]=utils::stringHash(e.key,0);
    res.t[1]=utils::stringHash(e.key,1);
    res.t[2]=e.val;
    return res;
  }
};
BPlusTree<Element, ElementHash, 80, 2000> bpt("test");

inline std::ostream &operator<<(std::ostream &os, const Element &e) {
  os << "< " << e.key << " , " << e.val << " >";
  return os;
}

int main() {
  std::ios::sync_with_stdio(0);
  std::cin.tie(0), std::cout.tie(0);

  // std::cout<<"Initialized"<<std::endl;
  int q;
  std::cin >> q;
  // bpt.debugPrint();
  while (q--) {
    std::string op, k;
    int v;

    std::cin >> op;
    if (op == "insert") {
      std::cin >> k >> v;
      bpt.insert(Element(k, v));
    }
    if (op == "delete") {
      std::cin >> k >> v;
      bpt.remove(Element(k, v));
    }
    if (op == "find") {
      std::cin >> k;
      Element L(k, INT_MIN), R(k, (long long)INT_MAX+1);
      sjtu::vector<Element> res = bpt.find(L, R);
      for (Element &e : res)
        std::cout << e.val << " ";
      if (res.empty())
        std::cout << "null";
      std::cout << '\n';
    }
    // std::cout<<"Done"<<std::endl;
    // bpt.debugPrint();
  }
  std::cout.flush();
}
/*
23
insert bc 2
insert bc 3
insert ca 1
insert bc 1
find cc
find ab
find cb
insert ac 3
insert aa 1
insert bb 2
insert bb 1
insert ba 1
find ac
insert ab 3
insert bb 3
insert cb 1
delete aa 1
delete ca 1
find bb
delete bc 3
insert aa 3
delete cc 2
find bc

null
null
null
3
1 2 3
1 2

*/