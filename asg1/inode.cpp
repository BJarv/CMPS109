//Brandon Jarvinen (bjarvine@ucsc.edu)
//Stella Lee (stjilee@ucsc.edu)

#include <iostream>
#include <stdexcept>
#include <iomanip>

using namespace std;

#include "debug.h"
#include "inode.h"

int inode::next_inode_nr {1};

inode::inode(inode_t init_type):
   inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case PLAIN_INODE:
           contents = make_shared<plain_file>();
           break;
      case DIR_INODE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

plain_file_ptr plain_file_ptr_of (file_base_ptr ptr) {
   plain_file_ptr pfptr = dynamic_pointer_cast<plain_file> (ptr);
   if (pfptr == nullptr) throw invalid_argument ("plain_file_ptr_of");
   return pfptr;
}

directory_ptr directory_ptr_of (file_base_ptr ptr) {
   directory_ptr dirptr = dynamic_pointer_cast<directory> (ptr);
   if (dirptr == nullptr) throw invalid_argument ("directory_ptr_of");
   return dirptr;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}




////////////////////////////////
//Constructor
////////////////////////////////
inode_state::inode_state() { //sets up default values for an inode_state
   root = make_shared<inode>(DIR_INODE);
   cwd = root;
   directory_ptr_of(root->contents)->setRoot(root);
   prompt = "%";
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");
}


////////////////////////////////
//Get Functions
////////////////////////////////
string inode_state::getPrompt() const {
   return prompt;
}

int inode::getType() const {
   return type;
}

size_t inode::size() const {
   if(type == DIR_INODE){
      return directory_ptr_of(contents)->size();
   } else {
      //nothing else with plain_file implemented so this shouldnt
      //be called at any point
      return plain_file_ptr_of(contents)->size();
   }
}

size_t directory::size() const {
   return dirents.size();
}

size_t plain_file::size() const {
   size_t size = 0;
   for(auto i = data.begin(); i != data.end(); i++){
      size += i->size();
   }
   return size;
}

inode_ptr inode_state::findPath(const string& path){
   //returns node at end of path when ends with a /
   //otherwise returns second to last node to help 
   //mkdir be able to create nodes from paths
   //ex: findPath on /a/b/c returns a pointer to b
   //ex2: findPath on /a/b/c/ returns a pointer to c
   inode_ptr curr;
   directory_ptr dir;
   string pathCopy = path;

   if(path.at(0) == '/') {
      pathCopy = path.substr(1);
      curr = root;
   } else {
      curr = cwd;
   }
   wordvec p = split(pathCopy, "/");
   
   for(string word : p) {
      if(p.at(p.size()-1) == word && path.back() != '/') {
         break;
      } 
      dir = directory_ptr_of(curr->getContents());
      curr = dir->getChild(word);
      if(curr == nullptr) {
         return curr;
      }      
   }
   return curr;
}

inode_ptr directory::getChild(const string& name) {
   //get node from dirents in directory
   auto entry = dirents.find(name);
   if(entry == dirents.end()){
      return nullptr;
   }
   return entry->second;
}

string inode::getName() const {
   return name;
}

file_base_ptr inode::getContents() const {
   return contents;
}

void inode_state::ls(const wordvec& words) {
   //helper function to directory::ls
   //gets references to DIR_INODEs and calls ls
   //on each. PLAIN_INODE not implemented
   if(words.size() == 1) {
      directory_ptr dir = directory_ptr_of(cwd->getContents());
      cout << getCwd() << ":" << endl;
      dir->ls();
   } else {
      for(unsigned int i = 1; i < words.size(); i++){
         string path = words.at(i);
         inode_ptr node = findPath(path);
         if(node == nullptr){
            throw yshell_exn("path given to ls not found");
         }  else if(node->getType() == PLAIN_INODE) {
            throw yshell_exn("ls on plain inode not implemented");
         }
         directory_ptr dir = directory_ptr_of(node->getContents());
         cout << path << ":" << endl;
         dir->ls();
      }
   }
   
}

void directory::ls() {
   //prints formatted list of inodes in the directory called on
   for(auto i = dirents.begin(); i != dirents.end(); i++){
      string slash = "";
      if(i->second->getType() == DIR_INODE && 
            i->first != "." && 
            i->first != "..") {
         slash = "/";
      } else {
         slash = "";
      }
      (cout << setw(6) << i->second->get_inode_nr() << setw(6) <<
            i->second->size() << "\t" << i->first << slash << endl);
   }
}

string inode_state::getCwd() const {
   //builds a string from working backward up the
   //chain of inodes starting from cwd to root by using
   //the ".." dirent present in each directory.
   inode_ptr curr = cwd;
   directory_ptr currContents;
   wordvec path;
   string name = "/";
   while(curr != root) {
      path.push_back(curr->getName());
      currContents = directory_ptr_of(curr->contents);
      curr = currContents->getChild("..");
   }
   while(path.size() > 0) {
      name.append(path.back());
      name.append("/");
      path.pop_back();
   }
   return name;
}

////////////////////////////////
//Set Functions
////////////////////////////////
void inode_state::setPrompt(string newPrompt) {
   //changes prompt display
   prompt = newPrompt;
}

void inode::setName(const string& newName) {
   name = newName;
}

void directory::setRoot(inode_ptr root) {
   //sets up root inode with default dirents and name
   dirents.insert(make_pair(".", root));
   dirents.insert(make_pair("..", root));
   root->setName("/");
}

void directory::setParentChild(inode_ptr parent, inode_ptr child) { 
   //sets up a new DIR_INODE with default dirents
   dirents.insert(make_pair(".", child));
   dirents.insert(make_pair("..", parent));
}

void inode_state::cdRoot() {
   cwd = root;
}

void inode_state::cd(const string& path) {
   //changes current directory to the specified path
   string newPath = path;
   if(newPath.back() != '/') {
      newPath.append("/");
   }
   inode_ptr setDir = findPath(newPath);
   if(setDir == nullptr) {
      throw yshell_exn("Directory not found");
   } else {
      cwd = setDir;
   }
}

void inode_state::mkdir(const string& path) {
   string name = path;
   if (path.back() == '/') { //trim ending '/' for findpath
      name = path.substr(0, path.size() - 1);
   }
   inode_ptr p = findPath(name);
   if(p == nullptr) {
      throw yshell_exn("Invalid path");
   }
   directory_ptr dir = directory_ptr_of(p->getContents());
   size_t index = name.find_last_of("/");
   if (index == string::npos){  //if no '/' is found
      dir->mkdir(name);
   } else {
      dir->mkdir(name.substr(index+1));
   }
}
inode_ptr directory::mkdir(const string& name) {
   //creates a new node and adds it to the parent node's directory.
   if(dirents.find(name) != dirents.end()) {
      throw yshell_exn("trying to create existing directory");
   }
   inode_ptr parent = dirents.at(".");
   //new dir_inode creation
   inode_ptr node = make_shared<inode>(DIR_INODE);
   node->setName(name);
   directory_ptr dir = directory_ptr_of(node->getContents());
   dir->setParentChild(parent, node); 
   dirents.insert(make_pair(name, node));
   return node;
}


ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

