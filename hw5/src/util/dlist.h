/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node -> _next; return *(this); }//++iterator先加再return
      iterator operator ++ (int) { iterator temp(*(this));  _node = _node -> _next; return temp; }//iterator++先return再加
      iterator& operator -- () {  _node = _node -> _prev; return *(this); }//--iterator
      iterator operator -- (int) { iterator temp(*(this)); _node = _node -> _prev; return temp; }//iterator--
      iterator& operator = (const iterator& i) { _node = i. _node; return *(this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const {
     iterator it( _head );
     return it; 
   }
   iterator end() const { //dummy node
     iterator it( _head -> _prev) ; 
     return it ;
   }
   bool empty() const {
     if( _head -> _next == _head){ return true;}
     else return false;
   }
   size_t size() const {
     if(empty())return 0;
     size_t count = 1;
     iterator it(_head);
     while( true ){
       it++;
       if(it == end())break;
       count++;
      }
      return count;
   }
   void push_back(const T& x) {  //performance is bad
     if(empty()){
       DListNode<T>* dln = new DListNode<T>( x, _head->_prev->_prev, _head->_prev);
      _head->_prev->_prev->_next = dln;
      _head->_prev->_prev = dln;
      _head = _head -> _next;
      _isSorted = false;
     }else{
       DListNode<T>* dln = new DListNode<T>( x, _head->_prev->_prev, _head->_prev);
      _head->_prev->_prev->_next = dln;
      _head->_prev->_prev = dln;
      _isSorted = false;
     } 
   }
   void pop_front() { 
     if(!empty()){
       DListNode<T>* temp = _head ; 
       _head->_prev->_next = _head->_next;
       _head->_next->_prev = _head->_prev;
       _head = _head->_next;
       delete temp;
     }
   }
   void pop_back() { 
     if(size()==1) pop_front();
     else if(size()>1){
       DListNode<T>* temp = _head-> _prev -> _prev ;
       _head-> _prev -> _prev -> _prev -> _next = _head -> _prev;
       _head-> _prev -> _prev = _head-> _prev -> _prev -> _prev ;
       delete temp;
     }
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
     if(empty())return false;
     DListNode<T>* temp = pos._node;
     temp -> _prev -> _next = temp -> _next;
     temp -> _next -> _prev = temp -> _prev;
     if(pos._node == _head) _head = _head -> _next ; 
     delete temp;
     return true;
   }
   bool erase(const T& x) { 
     if(empty())return false;
     iterator it(0);
     for(it = begin() ; it != end(); it++){
       if(it._node->_data == x){
         erase(it);
         return true;
       }
     }return false;
   }
// /*  */
   void clear() { // delete all nodes except for the dummy node
    while(!empty())pop_front();
    _head -> _next = _head;
    _head -> _prev = _head; 
  }  
   void sort() const { 
     size_t s = size();
     if(s>1 && _isSorted == false ){
       DListNode<T> *dptr = _head , *comp;
       T temp ;
       for( size_t i = 1 ; i < s ; i++){
         dptr = dptr -> _next; comp = dptr ; 
         for( size_t j = i ; j > 0 ; j--){
           if( comp -> _data >= comp -> _prev -> _data)break;
           temp = comp -> _prev -> _data;
           comp -> _prev -> _data = comp -> _data;
           comp -> _data = temp;
           if(comp -> _prev != _head ) comp = comp -> _prev ;
         }
       }_isSorted = true ;
     }
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
