#include "suffix_tree.h"
#include<iostream>
#include<assert.h>
#include<stack>
#include <algorithm>
using std::vector;
using std::string;
using std::cout;

edge::edge(int start,int end,int origin,int destination)
{
  start_index = start;
  end_index = end;
  origin_node = origin;
  destination_node = destination;
  if(end_index != CURRENT_END) assert(end_index >= start_index);
}

int edge::length(){ return end_index-start_index+1; }

//node class
node::node() : value(-1),suffix_link(-1){}

//active point class
active_point::active_point()
{
  active_node = 0;
  active_length = 0;
  active_edge = 0;
}

/*
    this constructor for the suffix tree uses ukkonen's algorithm to create the tree
    in linear time with respect to the sequence length
*/
suffix_tree::suffix_tree(const std::string& s) : text(string(string(" ")+s+string("$"))),
                                                 a(active_point())
{
  nodes.push_back(node());//the root node
  int remainder = 0;
// -- main loop of algorithm -- 
  for(unsigned int i=1;i<text.length();i++){
    remainder += 1;
    int last_inserted = -1;
    int new_inserted = -1;
    bool first = true;
//try to add suffixes while there's still suffixes to add at the current position i in the string
    while(remainder>0){
//check if the suffix is already contained in the tree implicitly by checking if the new character
//appears immediately after the active point. 
//if it does, move the active point up 1 and end current step
//if there was already an internal node marked this step as 
//needing a suffix link, link it to the active node
      if(suffix_already_exists(i)){
        if(a.active_node !=0 && !first){
          nodes[new_inserted].suffix_link = a.active_node;
        }
        if(a.active_edge==0) a.active_edge = i;
        a.active_length += 1;
        canonize();
        break;
      }
//if we get to here, we didn't find the suffix so it's time to update the tree.
//  we add a new leaf to the tree
//if the leaf is added to an internal node, the internal node gets a suffix link
//  to the last internal node that got a leaf added (rule 2)
//insert a leaf node directly if active point is at a node. 
      if(a.active_edge==0){
        add_leaf_node(i,remainder);
        if(a.active_node !=0){
          last_inserted = new_inserted;
          new_inserted = a.active_node;
          if(!first){ //rule 2
            nodes[last_inserted].suffix_link = a.active_node;
          }
        }
      }
//if the active point is on an edge, split the edge and add a leaf to the new internal node
      else{
        assert(a.active_edge!=0 && a.active_length>0);//sanity check
        last_inserted = new_inserted;
        new_inserted = split_edge(active_edge(),
                                   active_edge().start_index+a.active_length,
                                   i,i-remainder+1);
        if(!first){ //rule 2
          nodes[last_inserted].suffix_link = new_inserted;
        }
      }
//now that we've added a new node, move the active point to the next shorter suffix
//if we're at root, this is done by decrementing active length (rule 1)
//we we're at a non-root node, this is done by following a suffix link (rule 3)
      if(a.active_node==0 && a.active_length > 0){//rule 1
        a.active_length -= 1;
        if (a.active_length == 0) a.active_edge = 0;
        else a.active_edge += 1;
        canonize();
      }
      else if(a.active_node!=0){//rule 3
        a.active_node = active_node().suffix_link==-1?0:active_node().suffix_link;
        canonize();
      }
      first = false;
      remainder -= 1;
    }
  }//end main loop
}

void suffix_tree::show()
{//print a representation of the suffix tree for debugging
  for(unsigned int i=0;i<nodes.size();i++){
    if(!nodes[i].edges.empty()){ 
      cout<<"node "<<i;
      if(nodes[i].suffix_link != -1) cout <<" - - - "<<nodes[i].suffix_link;
      cout<<std::endl;
      for (auto it=nodes[i].edges.begin();it!=nodes[i].edges.end();++it){
        int start = it->second.start_index;
        int end = it->second.end_index;
        if(end==CURRENT_END) end = text.size()-1;
        cout<<it->first<<" "<<text.substr(start,end-start+1);
        if(it->second.end_index != CURRENT_END)//edge does not point to leaf node
          cout<<" -> "<<it->second.destination_node;
        else cout <<"("<<nodes[it->second.destination_node].value<<")";
        cout<<std::endl;
      }
    } 
  }
}

bool suffix_tree::suffix_already_exists(int i)
{
  if (a.active_edge==0) return nodes[a.active_node].edges.count(text[i])==1;
  else return text[i] == active_point_character();
}


int suffix_tree::add_leaf_node(int position,int remainder)//add a new leaf node to the active node
{
  nodes.push_back(node());
  nodes[a.active_node].edges.insert(std::make_pair(
                                      text[position],
                                      edge(position,CURRENT_END,
                                           a.active_node,nodes.size()-1)));
  nodes[nodes.size()-1].value = position-remainder+1;//add index where the suffix represented by this leaf started
  return nodes.size()-1;
}

int suffix_tree::split_edge(edge& e,int position_to_split,int current_position,int suffix_start)
{
  nodes.push_back(node());//make a new internal node
  edge old_edge = e;
  //edit the edge to be split to go from active node to new internal node
  e.destination_node = nodes.size()-1;
  e.end_index = position_to_split-1;
  //add an edge from the internal node to the orphan leaf node
  nodes[nodes.size()-1].edges.insert(std::make_pair(
                                      text[position_to_split],
                                      edge(position_to_split,old_edge.end_index,
                                           nodes.size()-1,old_edge.destination_node)));
  //add a new leaf node to the internal node representing the repeated character
  nodes.push_back(node());
  nodes[nodes.size()-2].edges.insert(std::make_pair(
                                      text[current_position],
                                      edge(current_position,CURRENT_END,
                                           nodes.size()-2,nodes.size()-1)));
  nodes[nodes.size()-1].value = suffix_start;
  return e.destination_node;//the address of the internal node
}

edge& suffix_tree::active_edge(){
  return nodes[a.active_node].edges.at(text[a.active_edge]);
}
node& suffix_tree::active_node(){
  return nodes[a.active_node];
}

char suffix_tree::active_point_character()//character immediately AFTER the active point
{
//  assert (a.active_edge !=0);
  if(a.active_edge==0) return 0;
  return text[ nodes[a.active_node].edges.at(text[a.active_edge]).start_index 
             + a.active_length];
}

void suffix_tree::canonize()
{//fixes the active point when active_length grows beyond the bounds of the active_edge
  if (a.active_edge==0)return;
  if (active_edge().end_index == CURRENT_END) return;
  while(active_edge().start_index+a.active_length > active_edge().end_index){//maybe should be >=
    int increment = active_edge().end_index - active_edge().start_index+1;
    a.active_node = active_edge().destination_node;
    a.active_length -= increment;
    if(a.active_length>0) a.active_edge += increment;
    else{ 
      a.active_edge = 0;
      return;
    }
    if (a.active_edge != 0) 
      if( active_edge().end_index == CURRENT_END) return;
    assert(a.active_edge >= 0);
  }
}


//returns the root node of the sub-tree which represents all the suffixes of the query
//or -1 if query not found
int suffix_tree::search_for_substring(const string& query)
{
  a = active_point();
  for(unsigned int i=1;i<query.size();i++){
    if(a.active_edge==0){//we are on a node
      if(nodes[a.active_node].edges.count(query[i])==0)
        return -1;
      else
        a.active_edge = i;
        a.active_length += 1;
    }
    else{//we are on an edge
      if(!(query[i]==text[nodes[a.active_node].edges.at(query[a.active_edge])
                                              .start_index+a.active_length])){
        return -1;
      }
      else{
        a.active_length += 1;
      }
    }
//if we reached the end of an edge, hop to the node
    if(a.active_length >= nodes[a.active_node].edges.at(query[a.active_edge]).length()){
      a.active_node = nodes[a.active_node].edges.at(query[a.active_edge]).destination_node;
      a.active_length = a.active_edge = 0;
    }
  }
  if (a.active_edge==0) return a.active_node;
  else return nodes[a.active_node].edges.at(a.active_edge).destination_node;
}


vector<int> suffix_tree::find_leaves(int start)
{//once we locate the root of the subtree which contains suffixes of the query
//traverse the sub tree to find the leaves 
//their labels give the positions of the query in the full string
  std::stack<int>stack;
  vector<int>values;
  stack.push(start);
  while(!stack.empty()){
    int current_node =  stack.top();
    stack.pop();
//if node has no children it is a leaf. save its start position.
    if(nodes[current_node].edges.empty()){
      assert(nodes[current_node].value > 0);//sanity check, if its a leaf it has a positive value
      values.push_back(nodes[current_node].value);
    }
//otherwise, put its children on the stack
    for (auto it=nodes[current_node].edges.begin();it!=nodes[current_node].edges.end();++it){
        stack.push(it->second.destination_node);
    }
  }
  return values;
}


void suffix_tree::find_all_substrings_matching(const string& query)
{
  int subtree_root = search_for_substring(query);
  if (subtree_root == -1) {
    cout << "no matches found!\n";
    return;
  }
  vector<int> start_positions = find_leaves(subtree_root);
  std::sort(start_positions.begin(),start_positions.end());
  cout << "start positions of query string "<<query.substr(1)<<":\n";
  for(int p : start_positions){
    cout << p << std::endl;
  }
}
