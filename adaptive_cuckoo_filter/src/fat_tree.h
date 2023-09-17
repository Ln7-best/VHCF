#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>

struct node {
  // uint64_t id;
  //  std::vector<uint64_t> ports_id;
  std::vector<uint64_t> nxt_node_id;
  std::vector<bool> is_choosen;
  bool is_path = false;
};
class fat_tree {
private:
  uint64_t k;
  struct node *nodes;
  uint64_t tot_node_num;
  uint64_t src;

public:
  fat_tree(uint64_t n_k) : k(n_k) {
    tot_node_num = (k / 2) * (k / 2) + k * k + k * k * k / 4;
    nodes = new node[tot_node_num + 10];
  }
  // id of core 0 -- k*k/4-1
  // id of aggregation k*k/4 -- k*k/4+k*k/2-1
  // id of edge k*k/4+k*k/2 -- k*k/4+k*k-1
  // id of hosts k*k/4+k*k -- k*k/4+k*k+k*k*k/4-1
  void build_tree() {
    // build core
    for (uint64_t i = 0; i < k / 2; i++) {
      for (uint64_t j = 0; j < k / 2; j++) {
        for (uint64_t pod = 0; pod < k; pod++) {
          // nodes[i * k / 2 + j].id = i * k / 2 + j;
          nodes[i * k / 2 + j].is_choosen.push_back(false);
          // nodes[i * k / 2 + j].ports_id.push_back(pod);
          nodes[i * k / 2 + j].nxt_node_id.push_back((k / 2) * (k / 2) +
                                                     pod * k / 2 + i);
          nodes[(k / 2) * (k / 2) + pod * k / 2 + i].nxt_node_id.push_back(
              i * k / 2 + j);
          nodes[(k / 2) * (k / 2) + pod * k / 2 + i].is_choosen.push_back(
              false);
        }
      }
    }
    // build aggregation
    for (uint64_t pod = 0; pod < k; pod++) {
      for (uint64_t i = 0; i < k / 2; i++) {
        for (uint64_t j = 0; j < k / 2; j++) {
          nodes[k * k / 4 + pod * k / 2 + i].is_choosen.push_back(false);
          nodes[k * k / 4 + pod * k / 2 + i].nxt_node_id.push_back(
              k * k / 4 + k * k / 2 + pod * k / 2 + j);
          nodes[k * k / 4 + k * k / 2 + pod * k / 2 + j].is_choosen.push_back(
              false);
          nodes[k * k / 4 + k * k / 2 + pod * k / 2 + j].nxt_node_id.push_back(
              k * k / 4 + pod * k / 2 + i);
        }
      }
    }
    // build edge
    for (uint64_t pod = 0; pod < k; pod++) {
      for (uint64_t i = 0; i < k / 2; i++) {
        for (uint64_t j = 0; j < k / 2; j++) {
          nodes[k * k / 4 + k * k / 2 + pod * k / 2 + i].is_choosen.push_back(
              false);
          nodes[k * k / 4 + k * k / 2 + pod * k / 2 + i].nxt_node_id.push_back(
              k * k / 4 + k * k + pod * k * k / 4 + i * k / 2 + j);
          nodes[k * k / 4 + k * k + pod * k * k / 4 + i * k / 2 + j]
              .is_choosen.push_back(false);
          nodes[k * k / 4 + k * k + pod * k * k / 4 + i * k / 2 + j]
              .nxt_node_id.push_back(k * k / 4 + k * k / 2 + pod * k / 2 + i);
        }
      }
    }
  }
  void build_multicast_path(std::vector<uint64_t> &positives,
                            std::vector<uint64_t> &negatives) {
    uint64_t src = rand() % (k * k * k / 4) + k * k / 4 + k * k;
    long long dis[100000];
    // long long *dis = new long long[tot_node_num];
    bool *vis = new bool[tot_node_num];
    int *from = new int[tot_node_num];
    uint64_t cur_node = src;
    for (uint64_t i = 0; i < tot_node_num; i++) {
      dis[i] = 0x3f3f3f3f;
      vis[i] = false;
    }
    from[src] = -1;
    dis[cur_node] = 0;
    vis[cur_node] = true;
    for (uint64_t i = 0; i < tot_node_num; i++) {
      for (uint64_t j = 0; j < nodes[cur_node].nxt_node_id.size(); j++) {
        uint64_t nxt_node = nodes[cur_node].nxt_node_id[j];
        if (dis[nxt_node] > dis[cur_node] + 1) {
          dis[nxt_node] = dis[cur_node] + 1;
          from[nxt_node] = cur_node;
        }
      }
      uint64_t min_dis = 0x3f3f3f3f;
      for (uint64_t j = 0; j < tot_node_num; j++) {
        if (min_dis > dis[j] && !vis[j]) {
          min_dis = dis[j];
          cur_node = j;
        }
      }
      vis[cur_node] = true;
    }
    std::unordered_set<uint64_t> dst;
    while (dst.size() <= 0.4 * k * k * k / 4) {
      uint64_t rand_val = rand() % (k * k * k / 4) + k * k / 4 + k * k;
      if (rand_val == src)
        continue;
      dst.insert(rand_val);
    }
    for (auto d : dst) {
      uint64_t pre_node = d;
      cur_node = from[d];
      while (cur_node != -1) {
        nodes[cur_node].is_path = true;
        for (uint64_t i = 0; i < nodes[cur_node].nxt_node_id.size(); i++) {
          if (nodes[cur_node].nxt_node_id[i] == pre_node) {
            nodes[cur_node].is_choosen[i] = true;
            break;
          }
        }
        pre_node = cur_node;
        cur_node = from[cur_node];
      }
    }
    for (uint64_t i = 0; i < tot_node_num; i++) {
      // if (!nodes[i].is_path)
      //   continue;
      for (uint64_t j = 0; j < nodes[i].is_choosen.size(); j++) {
        if (nodes[i].is_choosen[j])
          positives.push_back(i * k + j);
        else
          negatives.push_back(i * k + j);
      }
    }
    delete[] vis;
    delete[] from;
  }
  void print_tree() {
    std::cout << "print core:" << std::endl;
    for (uint64_t i = 0; i < k * k / 4; i++) {
      std::cout << i << ":";
      for (uint64_t j = 0; j < nodes[i].nxt_node_id.size(); j++) {
        std::cout << " " << nodes[i].nxt_node_id[j];
      }
      std::cout << std::endl;
    }
    std::cout << "print aggregation:" << std::endl;
    for (uint64_t i = k * k / 4; i < k * k / 4 + k * k / 2; i++) {
      std::cout << i << ":";
      for (uint64_t j = 0; j < nodes[i].nxt_node_id.size(); j++) {
        std::cout << " " << nodes[i].nxt_node_id[j];
      }
      std::cout << std::endl;
    }
    std::cout << "print edge:" << std::endl;
    for (uint64_t i = k * k / 4 + k * k / 2; i < k * k / 4 + k * k; i++) {
      std::cout << i << ":";
      for (uint64_t j = 0; j < nodes[i].nxt_node_id.size(); j++) {
        std::cout << " " << nodes[i].nxt_node_id[j];
      }
      std::cout << std::endl;
    }
    std::cout << "print hosts:" << std::endl;
    for (uint64_t i = k * k / 4 + k * k; i < k * k / 4 + k * k + k * k * k / 4;
         i++) {
      std::cout << i << ":";
      for (uint64_t j = 0; j < nodes[i].nxt_node_id.size(); j++) {
        std::cout << " " << nodes[i].nxt_node_id[j];
      }
      std::cout << std::endl;
    }
  }
};
