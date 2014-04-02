#include "suffix_tree.h"
#include<string>
#include<iostream>
using std::cin;
using std::cout;
using std::string;

int main(int argc,char** argv)
{
  if(argc==2){
    suffix_tree s = suffix_tree(std::string(argv[1]));
    s.show();
    cout <<">";
    std::string input;
    getline(cin,input);
    cout<<std::endl;
    input = string("_") + string(input);
    s.find_all_substrings_matching(input);
  }
  return 0;
}
