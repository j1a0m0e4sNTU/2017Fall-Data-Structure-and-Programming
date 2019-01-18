/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   Data& min()  { return _data[0]; }
   void insert(const Data& d) { 
     _data.push_back(d);
     int i = _data.size() - 1 ;
     up(i);
   }
   void delMin() { 
     _data[0] = _data[ _data.size()-1];
     _data.pop_back();
     down(0);
   }
   void delData(size_t i) { 
     Data temp(_data[i].getName(),_data[i].getLoad());
     _data[i] = _data.back();
     _data.pop_back();
     if(i>=_data.size())return;
     if(temp < _data[i])down(i);
     else if(_data[i] < temp  )up(i);
   }
   void up(int i){
     int c = (i+1)/2 -1;
     Data temp("",0);
     while ( true ){
       if( c < 0 || _data[c]<_data[i]|| _data[c]==_data[i]) break;
       temp = _data[c];
       _data[c] = _data[i];
       _data[i] = temp;
       i = c; c = (i+1)/2 -1;
     }
   }
   void down(int i){
     if( i < 0 ||(2*(i+1)-1)>=_data.size()) return;
     int c;
     if((2*(i+1))>=_data.size())c=(2*(i+1)-1);
     else c =( _data[2*(i+1)-1] < _data[2*(i+1)] )? (2*(i+1)-1) : (2*(i+1)) ;
     Data temp("",0);
     if(_data[c] < _data[i]){
       temp = _data[c];
       _data[c] = _data[i];
       _data[i] = temp;
       down(c);
     }
   }
   bool empty(){ return _data.empty();}
private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
