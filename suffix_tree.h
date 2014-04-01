#ifndef SUFFIX_TREE
#define SUFFIX_TREE
#include <vector>
#include <unordered_map>
#include <string>

const static int CURRENT_END = -1;
class active_point;

class edge{
 public:
  edge(int,int,int,int);
  int start_index,end_index;
  int origin_node, destination_node;
};

class node{
 public:
  node();
  int value;
  int suffix_link;//points to the longest proper suffix of this node
  std::unordered_map <char,edge>edges;
};

class active_point{
 public:
  active_point();
  int active_node,active_edge,active_length;
};

class suffix_tree{
 public:
  suffix_tree(const std::string&);
  void show();
 private:
  std::vector<node> nodes;
//  std::unordered_map<edge_key,edge> edges;
  const std::string text;
  active_point a;
//private methods for tree construction
  void add_leaf_node(int);
  int split_edge(edge&,int,int);
  bool suffix_already_exists(int);
  bool suffix_is_implicit(int);
  char active_point_character();
//convenience functions to use the active point
  edge& active_edge();
  node& active_node();
  void canonize();

//these construct a tree using the naive O(N^2) algorithm
  void naive_construction();
  void walk_tree(const std::string&);

};

#endif
