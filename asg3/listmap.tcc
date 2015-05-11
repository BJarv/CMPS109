//Brandon Jarvinen (bjarvine@ucsc.edu)
//Fan Zhang (fzhang12@ucsc.edu)

#include "listmap.h"
#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   TRACE ('l', (void*) this);
}

//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   TRACE ('l', &pair << "->" << pair);

   iterator i = begin();
   for(; i != end(); ++i){
      if(less(i->first, pair.first)) {
         break;
      }
   }
   
   if(i != end() && !less(pair.first, i->first) && !less(i->first, pair.first)){
      //cout << "replacing: " << i->second << " becomes ";
      i->second = pair.second;
      cout << i->second << endl;
      return i;
   }

   node* newNode = new node(i.where, i.where->prev, pair);
   i.where->prev->next = newNode;
   i.where->prev = newNode;
   //cout << "end of insert" << endl;
   return iterator(newNode);
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   TRACE ('l', that);
   for(iterator i = begin(); i != end(); ++i){
      if(!less(that, i->first) && !less(i->first, that)) {
         //cout << "key found" << endl;
         return i;
      }
   }
   return end();
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   TRACE ('l', &*position);

   position.where->next->prev = position.where->prev;
   position.where->prev->next = position.where->next;
   
   iterator temp = position.where->next;
   
   position.erase();
   
   return temp;
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   TRACE ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   TRACE ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   TRACE ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   TRACE ('l', where);
   where = where->prev;
   return *this;
}

//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

//
// listmap::void listmap::iterator::erase()
//
template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::iterator::erase() {
   TRACE ('l', where);

   delete where;
   //delete this; //Lovecraftian automagic
}

