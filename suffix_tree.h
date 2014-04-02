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
  int length();
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
  const std::string text;
  active_point a;
//private methods for tree construction
  int add_leaf_node(int,int);
  int split_edge(edge&,int,int,int);
  bool suffix_already_exists(int);
  char active_point_character();
//functions to use the active point
  edge& active_edge();
  node& active_node();
  void canonize();
//functions to search in the tree
  void find_all_substrings_matching(const std::string& query);
  int search_for_substring(const std::string& query);
  std::vector<int> find_leaves(int);
};

#endif
