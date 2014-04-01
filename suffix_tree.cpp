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
node::node() : suffix_link(-1){}

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
#ifdef UKKONEN
  cout <<"UKKOKONEN\n";
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
      }
      else{
        last_inserted = new_inserted;
        new_inserted = split_edge(active_edge(),
                                   active_edge().start_index+a.active_length,
                                   i);
        if(!first){ //rule 2
          nodes[last_inserted].suffix_link = new_inserted;
          cout<<"Setting suffix link of node "<<last_inserted<<" to "<<new_inserted<<std::endl;
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
  }//end main loop
#else
  naive_construction();
#endif
}

bool suffix_tree::suffix_is_implicit(int position)
{
 return false;
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
}

int suffix_tree::split_edge(edge& e,int position_to_split,int current_position)
{
  cout<<"splitting edge "<<e.origin_node<<"->"<<e.destination_node<<"("<<text.substr(e.start_index,e.end_index-e.start_index+1)<<") at location "<<position_to_split<<std::endl;
  cout<<"current position "<< current_position<<" \n";
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
  

//  nodes[a.active_node].edges.insert(std::make_pair(a.active_edge,edge(e.start_index,position_to_split,e.origin_node,nodes.size()-1)));//add an edge from active node to internal node 
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
    cout<<"we canonized something!\n";
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



#ifndef UKKONEN

void suffix_tree::naive_construction()
{
  cout << "performing naive construction of suffix tree\n";
  nodes.push_back(node());
  nodes[0].edges.insert(std::make_pair(text[1],edge(1,text.size()-1,0,1)));
  for(unsigned int i=2;i<text.size();i++){
    string suffix = string(" ")+text.substr(i);
    walk_tree(suffix);
  }
}

void suffix_tree::walk_tree(const string& suffix)
{
  a = active_point();
  int end_point=-1;
  int final_i=text.size()-1;
  for(int i = 1;i<(int)suffix.size()-1;i++){
    final_i = text.size()-1;
    char test_char = suffix[i];
    cout<<"looking for "<<test_char<<"\n";
    if(a.active_edge==0){//if we're at a node, jump on an edge if possible
      bool edge_found = nodes[a.active_node].edges.count(test_char);
      if (nodes[a.active_node].edges.count(test_char)>0){
        show();
        a.active_edge = i;
        cout<<"set active edge to "<<i<<"\n";
      }
      else{ //if there's no matching edge at the start, make a new edge from root
//        nodes.push_back(node());
//        nodes[a.active_node].edges.insert(std::make_pair(text[text.size()-suffix.size()],
//                                            edge(text.size()-suffix.size(),
//                                            text.size()-1,a.active_node,nodes.size()-1)));
        end_point = i;
        break;
      }
    }
    cout<<"active point is " <<active_point_character()<<"\n";
    if(active_point_character() != test_char)
      cout<<"mismatched suffix!\n";
      cout<<active_point_character();
      end_point = a.active_length;
      final_i = i;
      break;
    if(i < nodes[a.active_node].edges.at(a.active_edge).end_index){
      a.active_length += 1;
    }
    else{
      a.active_node = nodes[a.active_node].edges.at(a.active_edge).destination_node;
      a.active_edge = 0;
      a.active_length = 0;
    }
  }
  if(a.active_edge==0){
    cout<<"adding edge starting with "<<text[text.size()-suffix.size()+end_point]<<"\n";
    nodes.push_back(node());
    nodes[a.active_node].edges.insert(std::make_pair(text[text.size()-suffix.size()+end_point],
                                        edge(text.size()-suffix.size()+end_point,
                                        text.size()-1,a.active_node,nodes.size()-1)));
  }else{
    cout<<"edge split, final i "<<final_i<< " "<<text.size()<<" "<<suffix.size()<<"\n";
    cout<<suffix<<std::endl;
    cout<<"tree before split:\n";
    show();
    split_edge(nodes[a.active_node].edges.at(text[a.active_edge]),
               nodes[a.active_node].edges.at(text[a.active_edge]).start_index+end_point,
               final_i+text.size()-suffix.size()+1 
               );
    cout<<"tree after split:\n";
    show();
  }
}
#endif
