#include "suffix_tree.h"
#include<string>
#include<iostream>
#include<fstream>
using std::cin;
using std::cout;
using std::string;

const char* usage = "This program will search for every instance of a substring in a larger text.\n\
Usage: substring <query string>\n\
The text should be provided from standard in\n\
 Example:\n    $echo mississippi | substring ssi \n\
to search for all instances of 'ssi' in 'mississippi'\n";

int main(int argc,char** argv)
{
  if(argc!=2){
    cout <<usage;
    return 1;
  }

  string line;
  string text;
  while(std::getline(cin,line)){
    text += line;
  }

  suffix_tree s = suffix_tree(text);
#ifdef DEBUG_MODE
  s.show();
#endif
  std::string input = string(argv[1]);
  input = string("_") + string(input);
  s.find_all_substrings_matching(input);
  return 0;
}
