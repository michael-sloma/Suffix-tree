#include "suffix_tree.h"
#include<iostream>
#include<assert.h>
using std::vector;
using std::string;
using std::cout;
/*
THE RULES:

Rule 1
    If after an insertion from the active node = root, the active length is greater than 0, then:

        active node is not changed
        active length is decremented
        active edge is shifted right (to the first character of the next suffix we must insert)

Rule 2
    If we create a new internal node OR make an inserter from an internal node, 
    and this is not the first SUCH internal node at current step, then we link 
    the previous SUCH node with THIS one through a suffix link.

Rule 3
    After an insert from the active node which is not the root node, we must 
    follow the suffix link and set the active node to the node it points to. 
    If there is no a suffix link, set the active node to the root node. Either 
    way, active edge and active length stay unchanged.
*/

edge::edge(int start,int end,int origin,int destination)
{
  start_index = start;
  end_index = end;
  origin_node = origin;
  destination_node = destination;
}

//node class
node::node() : value(-1),suffix_link(-1){}

//active point class
active_point::active_point()
{
  active_node = 0;
  active_length = 0;
  active_edge = 0;
}

suffix_tree::suffix_tree(const std::string& s) : text(string(string(" ")+s+string("$"))),
                                                 a(active_point())
{
  nodes.push_back(node());//the root node
  int remainder = 1;
// -- main loop of algorithm -- 
  for(unsigned int i=1;i<text.length()-1;i++){
    if(suffix_already_exists(i)){
      remainder += 1;
      if(a.active_edge==0) a.active_edge = i;
      a.active_length += 1;
      canonize();
      continue;
    }
    int last_inserted = -1;
    int new_inserted = -1;
    bool first = true;
    while(!suffix_already_exists(i)){
      if(a.active_edge==0){
        add_leaf_node(i);
        remainder -= 1;
      }
      else{
        last_inserted = new_inserted;
        new_inserted = split_edge(active_edge(),
                                   active_edge().start_index+a.active_length,
                                   i,i-remainder+1);
        remainder -= 1;
        if(!first){ //rule 2
          nodes[last_inserted].suffix_link = new_inserted;
        }
        if(a.active_node==0 && a.active_length > 0){//rule 1
          a.active_length -= 1;
          if (a.active_length == 0) a.active_edge = 0;
          else a.active_edge += 1;
        }
        if(a.active_node != 0){//rule 3
          a.active_node = active_node().suffix_link==-1?0:active_node().suffix_link;
          canonize();
        }
        first = false;
      }
    }
    remainder += 1;
  }//end main loop
}

void suffix_tree::show()
{
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


void suffix_tree::add_leaf_node(int position)//add a new leaf node to the active node
{
  nodes.push_back(node());
  nodes[a.active_node].edges.insert(std::make_pair(
                                      text[position],
                                      edge(position,CURRENT_END,
                                           a.active_node,nodes.size()-1)));
  nodes[nodes.size()-1].value = position;//add index where the suffix represented by this leaf started
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
                                      edge(current_position,old_edge.end_index,
                                           nodes.size()-2,nodes.size()-1)));
  nodes[nodes.size()-1].value = suffix_start;
  return nodes.size()-2;//the address of the internal node
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
{
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
    if (active_edge().end_index == CURRENT_END) return;
    assert(a.active_edge >= 0);
  }
}



