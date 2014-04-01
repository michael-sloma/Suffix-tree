#include "suffix_tree.h"
#include<string>

int main(int argc,char** argv)
{
  if(argc==2){
    suffix_tree s = suffix_tree(std::string(argv[1]));
    s.show();
  }
  return 0;
}
