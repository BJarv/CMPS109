//Brandon Jarvinen (bjarvine@ucsc.edu)
//Stella Lee (stjilee@ucsc.edu)


#include "commands.h"
#include "debug.h"

commands::commands(): map ({
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
}){}

command_fn commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   command_map::const_iterator result = map.find (cmd);
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1){
      state.cdRoot();
   } else if(words.size() == 2){
      state.cd(words.at(1));
   } else {
      throw yshell_exn("Too many arguments to cd");
   }
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string echo = "";
   if(words.size() > 1) {
      for(unsigned int i = 1; i < words.size(); i++) {
         echo.append(words[i] + " ");
      }
   }
   cout << echo << endl;
}

void fn_exit (inode_state& state, const wordvec& words){
   //takes in a string and tokenizes it to a number to be set
   //as the exit status.
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   (void) state;
   auto exit_status = 0;
   if(words.size() == 2) {
      exit_status = stoi(words.at(1));
      exit_status::set(exit_status);
   } else if(words.size() > 2) {
      cerr << "Invalid Exit Status" << endl;
   }

   throw ysh_exit_exn();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   state.ls(words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   state.mkdir(words.at(1));
}

void fn_prompt (inode_state& state, const wordvec& words){
   //changes the prompt to new string specified
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string newPrompt = "";
   if(words.size() > 1) {
      for(unsigned int i = 1; i < words.size(); i++) {
         if(i != words.size() - 1) {
            newPrompt.append (words[i] + " ");
         } else {
            newPrompt.append(words[i]);
         }
      }
      state.setPrompt(newPrompt);
   }
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << state.getCwd() << endl; 
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

