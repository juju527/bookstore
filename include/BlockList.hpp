#ifndef BLOCKLIST_HPP
#define BLOCKLIST_HPP

#include "MemoryRiver.hpp"
#include <cstring>
#include <string>
#include <vector>
using std::vector;

template <class Key, class Val, int B1, int B2, int maxb> class BlockList {
private:
  string file_name;

public:
  struct Data {
    Key key;
    Val val;
    Data() {}
    bool operator==(Data tmp) const {
      return (key == tmp.key) && (val == tmp.val);
    }
    bool operator<(Data tmp) const {
      if (key < tmp.key)
        return 1;
      if (tmp.key < key)
        return 0;
      return (val < tmp.val);
    }
  };
  struct node {
    int nxt, len;
    Data v[maxb];
    node() {
      nxt = len = 0;
      memset(v, 0, sizeof(Data));
    }
  };

private:
  MemoryRiver<node> m;

public:
  void init(string FN) {
    file_name = FN;
    if (std::filesystem::exists(file_name)) {
      m.initialise(file_name);
      return;
    }
    m.initialise(file_name);
    m.open();
    node aux;
    int index = m.write(aux);
    m.write_info(index, 2);
    m.close();
    return;
  }
  void split(int it) {
    node cur, aux;
    m.read(cur, it);
    int len = cur.len, l = len / 2;
    aux.nxt = cur.nxt;
    aux.len = len - l;
    for (int i = 0; i < aux.len; i++)
      aux.v[i] = cur.v[i + l];
    int index = m.write(aux);
    cur.nxt = index, cur.len = l;
    m.update(cur, it);
    return;
  }
  void merge(int it, int nxt) {
    node cur, aux;
    m.read(cur, it), m.read(aux, nxt);
    for (int i = 0; i < aux.len; i++)
      cur.v[i + cur.len] = aux.v[i];
    cur.len += aux.len;
    cur.nxt = aux.nxt;
    m.Delete(nxt);
    m.update(cur, it);
    return;
  }
  void chk(int it) {
    node aux;
    m.read(aux, it);
    int nxt = aux.nxt, l1 = aux.len, l2 = 0;
    if (l1 > B2) {
      split(it);
      return;
    }
    if (nxt) {
      m.read(aux, nxt);
      l2 = aux.len;
      if (l1 < B1 && l1 + l2 < B2)
        merge(it, nxt);
    }
    return;
  }
  void ins(int it, Data v) {
    node aux;
    m.read(aux, it);
    int len = aux.len;
    auto pos = std::lower_bound(aux.v, aux.v + len, v);
    int idx = pos - aux.v;
    std::copy_backward(pos, aux.v + len, aux.v + len + 1);
    aux.v[idx] = v;
    aux.len++;
    m.update(aux, it);
    chk(it);
    return;
  }
  void ins(Key key, Val val) {
    m.open();
    Data v;
    v.key = key, v.val = val;
    int it = 0;
    m.get_info(it, 2);
    while (1) {
      node cur;
      m.read(cur, it);
      int nxt = cur.nxt;
      if (!nxt) {
        ins(it, v);
        break;
      }
      Data aux = cur.v[cur.len - 1];
      if (aux < v) {
        it = nxt;
        continue;
      }
      ins(it, v);
      break;
    }
    m.close();
    return;
  }
  void del(int it, Data v) {
    node aux;
    m.read(aux, it);
    int len = aux.len, pos = -1;
    for (int i = 0; i < len; i++)
      if (aux.v[i] == v)
        pos = i;
    if (pos >= 0) {
      for (int i = pos; i < len - 1; i++)
        aux.v[i] = aux.v[i + 1];
      aux.len--;
      m.update(aux, it);
      chk(it);
    }
    return;
  }
  void del(Key key, Val val) {
    m.open();
    Data v;
    v.key = key, v.val = val;
    int it = 0;
    m.get_info(it, 2);
    while (1) {
      node cur;
      m.read(cur, it);
      int nxt = cur.nxt;
      if (!nxt) {
        del(it, v);
        break;
      }
      Data aux = cur.v[cur.len - 1];
      if (aux < v) {
        it = nxt;
        continue;
      }
      del(it, v);
      break;
    }
    m.close();
    return;
  }
  vector<Val> ans;
  void find(int it, Key key) {
    node cur;
    while (it) {
      m.read(cur, it);
      bool tag = 0, vis = 0;
      for (int i = 0; i < cur.len; i++) {
        if (!(cur.v[i].key == key)) {
          tag = 0;
          continue;
        }
        ans.emplace_back(cur.v[i].val);
        vis = 1;
        tag = 1;
      }
      if (!vis || !tag)
        return;
      it = cur.nxt;
    }
    return;
  }
  std::vector<Val> find(Key key) {
    m.open();
    int it = 0;
    m.get_info(it, 2);
    ans.clear();
    while (1) {
      node cur;
      m.read(cur, it);
      int nxt = cur.nxt;
      if (!nxt) {
        find(it, key);
        break;
      }
      Key aux = cur.v[cur.len - 1].key;
      if (aux < key) {
        it = nxt;
        continue;
      }
      find(it, key);
      break;
    }
    m.close();
    return ans;
  }
};
#endif