/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
enum STATE { root , leftchild , rightchild };

template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   
   BSTreeNode(const T& x, STATE s = root ,  BSTreeNode<T>* P = 0 ,BSTreeNode<T>* L = 0, BSTreeNode<T>* R = 0,bool e = false)
              : _data(x),_state(s) ,_parent(P) , _left(L), _right(R) , _isEnd(e) {}

   T              _data;
   STATE          _state;          
   BSTreeNode<T>* _parent;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
   bool           _isEnd ;
};

template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree(){
      _root = new BSTreeNode<T>(T(),root,0,0,0,true);
      _root -> _right = new BSTreeNode<T>(T(), rightchild , _root,0,0,true); //set an endNode  
    }
   ~BSTree(){ clear(); }
  
   class iterator { 
     friend class BSTree;

   public:
     iterator(BSTreeNode<T>* n = 0):  _node(n) {}
     iterator(const iterator& i) :  _node(i._node){}
     ~iterator() {}

     const T& operator * () const { return (_node -> _data); }
     T& operator * ()             { return (_node -> _data); }
     iterator& operator ++ ()     { 
       if( _node -> _isEnd == false){
         if( _node -> _right != 0){ _node = Lmost( _node -> _right ); }
         else if( _node -> _state == leftchild ){ _node = _node -> _parent ;}
         else { // _node -> _state == rightchild 
           while ( _node -> _state == rightchild ) { _node = _node -> _parent;}
           _node = _node -> _parent; 
         }
       }
       return (*this);
     }
     iterator  operator ++ (int)  {
       iterator temp(*this);
       ++(*this);
       return temp;
     }
     iterator& operator -- ()     {
       if( _node -> _left != 0) { _node = Rmost( _node -> _left); }
       else if( _node -> _state == rightchild ) { _node = _node -> _parent ;}
       else { //_node -> _state == leftchild
         while( _node -> _state == leftchild ){ _node = _node -> _parent ; }
         _node = _node -> _parent;
       }
       return (*this);
     }
     iterator  operator -- (int)  {
       iterator temp(*this);
       --(*this);
       return temp;
     }
     iterator& operator = (iterator& i) { _node = i._node ; return (*this) ; }
     bool operator != ( const iterator & i) const { return ( _node != i._node); }
     bool operator == ( const iterator & i) const { return ( _node == i._node); }
     
     BSTreeNode<T>* Lmost (  BSTreeNode<T>* i) {
       if(i -> _left == 0 ) return i;
       else return Lmost( i -> _left); 
     }
     BSTreeNode<T>* Rmost (  BSTreeNode<T>* i) {
       if(i -> _right ==0 ) return i;
       else return Rmost( i-> _right);
     }    
   private:
     BSTreeNode<T>* _node;
   };
   
   iterator begin()const {
     iterator it( _root );
     while ( it._node-> _left != 0){ it._node = it._node -> _left;}
     return it ;
   }
   iterator end()const {
     iterator it( _root );
     if(empty()) return it;
     else it._node = _root -> _right;
     return it;
   }
   bool empty()const{ 
     if( _root -> _isEnd == true )return true; 
     else return false ;
     }
   size_t size()const{
     if(empty()) return 0;
     size_t count = 0;
     iterator it_1( begin() );
     iterator it_2( end() );
     while( it_1 != it_2){ it_1++; count++; }
     return count ; 
   }
   void insert(const T& x){
     if(empty()){ _root -> _data  = x; _root -> _isEnd = false;}
     else{  sink( _root, x); }
   }
   //helper functions
   void sink( BSTreeNode<T>*  n, const T& x ){
     if( x <= ( n -> _data) ||  (n -> _isEnd ) == true){
       if( (n -> _left) == 0) {
         n -> _left = new BSTreeNode<T>(x, leftchild, n);
       }else{ n = (n -> _left); sink(n,x);}
     }else{ // x > ( n -> _data)
       if( (n -> _right) ==0){
         n -> _right = new BSTreeNode<T>(x, rightchild, n);
       }else { n = (n -> _right); sink(n,x); }
     }
   }
   void pop_front(){
     if(!empty()){
       iterator it = begin() ; 
       erase( it );
     }  
   }
   void pop_back(){
     if(!empty()){
       iterator it = end(); it--;
       erase( it );
     }
   }
   bool erase(iterator pos){
     BSTreeNode<T>*  n = pos. _node;
     if ( empty() ||(n -> _isEnd == true) )return false ;
     if( (n -> _left ==0) && (n -> _right ==0) ){       // leaf
       if( n -> _state == leftchild ) n -> _parent -> _left = 0 ;
       else                         { n -> _parent -> _right = 0 ; }
       delete n ; n = 0 ; return true ;
     }
     else if( (n -> _left) == 0 && (n -> _right) != 0){ //nonleaf case 1
       if( n == _root ){
         if(  n -> _right -> _left == 0){  (_root -> _isEnd) = true; return true; }
         else{
           BSTreeNode<T>* temp = (_root -> _right);
           while ( temp -> _left != 0 ){ temp = (temp -> _left) ; }
           iterator it(temp);
           (_root -> _data) = (temp -> _data);
           return erase(it);
         }
       }else if( (n -> _state) == leftchild ){       //not _root  && n->_left ==0 && n ->_right != 0
         n -> _right -> _state = leftchild;
         n -> _parent -> _left = n -> _right ;
       } else { n -> _parent -> _right = n -> _right; }
       n -> _right -> _parent = n -> _parent ;
       delete n ; n = 0; return true ;
     }
     else if( (n -> _left != 0) && (n -> _right == 0) ){ //nonleaf case 2
       if( (n -> _state) == rightchild ){
         n -> _left -> _state = rightchild;
         n -> _parent -> _right = n -> _left ;
       }else { n -> _parent -> _left = n -> _left ; }
       n -> _left -> _parent = n -> _parent;
       delete n ; n = 0 ; return true ;
     } else{                                        //nonleaf case3 : have two children
       iterator temp(pos);
       if((++temp)!=end()){(*pos) = (*temp); }
       else{ temp = pos;  --temp ;  (*pos) = (*temp); }
       return erase(temp);
     }
   }
   bool erase(const T& x){
     if(empty())return false ;
     iterator it = begin();
     for(; it != end(); it++){
       if( *(it) == x ){ erase( it );return true ; }
     }return false ;
   }
   void clear(){
     size_t s = size();
     for(size_t i = 0; i<s; i++) pop_front();
   }
   void sort() const {}
   void print()const {}
private:
   //data members
   BSTreeNode<T>* _root;
};

#endif // BST_H
