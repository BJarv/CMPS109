//Brandon Jarvinen (bjarvine@ucsc.edu)
//Fan Zhang (fzhang12@ucsc.edu)

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unistd.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}

string trim(string line) {
   size_t first = line.find_first_not_of(' ');
   size_t last = line.find_last_not_of(' ');
   if(first == string::npos || last == string::npos) {
      return "";
   }
   string result = line.substr(first, last - first + 1);
   return result;
}

void parse(string line, str_str_map& map) {
   line = trim(line);
   string key = "";
   string value = "";

   if(line[0] == '#' || line.length() == 0) {
      //cout << "comment line" << endl;
      return;
   }
   size_t pos = line.find_first_of('=');
   bool atFirst = pos == 0;
   bool atLast = pos == line.length() - 1;
   if(pos == string::npos) {
      //'key' case
      key = line;
      str_str_map::iterator i = map.find(key);
      if(i != map.end()) {
         cout << i->first << " = " << i->second << endl;
      } else {
         cout << key << ": key not found" << endl;
      }
   } else if(line.length() == 1){
      //'=' case
      for(auto i = map.begin(); i != map.end(); ++i) {
         cout << i->first << " = " << i->second << endl;
      }
   } else if(!atFirst && atLast){
      //'key =' case
      key = trim(line.substr(0, line.length()-1));
      for(auto i = map.begin(); i != map.end(); ++i) {
         if(key == i->first){
            map.erase(i);
            break;
         }
      }
   } else if(!atFirst && !atLast){
      //'key = value' case
      key = trim(line.substr(0, pos));
      value = trim(line.substr(pos + 1));
      //cout << "key: " << key << endl;
      //cout << "value: " << value << endl;
      str_str_pair newPair(key, value);

      map.insert(newPair);
      cout << key << " = " << value << endl;
   } else if (atFirst && !atLast) {
      //'= value' case   
      value = trim(line.substr(1));
      for(auto i = map.begin(); i != map.end(); ++i) {
         if(value == i->second){         
            cout << i->first << " = " << i->second << endl;
         }
      }
   } else {
      cout << "error in parse!" << endl;
   }

}



int main (int argc, char** argv) {
   //sys_info::set_execname (argv[0]);
   scan_options (argc, argv); 
   str_str_map map;
   string line = "";
   int lineNum = 0;
   if(argc == 1) {
      while(getline (cin, line)) {
         if(cin.eof()){
            break;
         }         
         ++lineNum;
         cout << "-: " << lineNum << ": " << line << endl;
         parse(line, map);
      }      
      
      
   } 
   for(int i = 1; i < argc; ++i){
      if(argv[i] == std::string("-")) {
         while(getline (cin, line)) {
            if(cin.eof()){
               break;
            }         
            ++lineNum;
            cout << "-: " << lineNum << ": " << line << endl;
            parse(line, map);
         }
      } else {     
         ifstream f (argv[i]);
         if (f.fail()) cout << "no such file" << endl;
         while(getline (f, line)) {
            ++lineNum;
            cout << argv[i] << ": " << lineNum << ": " << line << endl;
            parse(line, map);
         }
      }
   }
   
   
   

//   string line;
//   if (*.is_open())
//   {
//      while ( getline (myfile,line) )
//      {
//         cout << line << '\n';
//      }
//      myfile.close();
//   }

   return EXIT_SUCCESS;
}

