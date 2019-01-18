/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;
   public:
      iterator(HashSet<Data>* h,Data* d):hash(h),dptr(d){}
      iterator(const HashSet<Data>::iterator& i):hash(i.hash),dptr(i.dptr){}
      ~iterator(){}
      const Data& operator * () const { return *dptr; }
      Data& operator * () { return *dptr; }
      iterator& operator ++ () {  //++iterator 
        if(hash->lastData(*dptr)){dptr = 0;}
        else if( hash->isLast(*dptr)){
          size_t  s = hash->bucketNum(*dptr)+1;
          while(hash->_buckets[s].empty()){ s++; }
          dptr = & (hash->_buckets[s].front());
        }else dptr++;
        return (*this);
      }
      iterator  operator ++(int){ iterator temp(*this); ++(*this); return temp; } 
      iterator& operator -- () {
        if(hash->isFirst(*dptr)){
          size_t s = hash->bucketNum(*dptr)-1;
          while(hash->_buckets[s].empty())s--;
          dptr = _buckets[s].back();
        }else dptr--;
        return (*this);
      }
      iterator  operator --(int){ iterator temp(*this); --(*this); return temp;}
      void operator =  (const iterator& i){ hash = i.hash;dptr = i.dptr;}
      bool operator == (const iterator& i) const { return (dptr == i.dptr);}
      bool operator != (const iterator& i) const { return (dptr != i.dptr);}
   private:
      Data* dptr;
      HashSet<Data>* hash;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //helper functions
   bool isFirst(const Data& d){
     if(d == _buckets[bucketNum(d)].front())return true;
     else return false;
   }
   bool isLast(const Data& d){
     if(d == _buckets[bucketNum(d)].back())return true;
     else return false;
   }
   bool lastData(const Data& d){
     size_t s = _numBuckets-1;
     while (_buckets[s].empty())s--;
     return (d == _buckets[s].back());
   }
   // Point to the first valid data
   iterator begin() const { 
     size_t s = 0;
     while (_buckets[s].empty())s++;
     Data* dptr = &(_buckets[s].front());
     iterator it(const_cast<HashSet<Data>*>(this),dptr);
     return it;
    }
   // Pass the end
   iterator end()  const { 
     iterator it(const_cast<HashSet<Data>*>(this),0);
     return it;
   }
   // return true if no valid data
   bool empty() const {
     for(size_t i=0; i<_numBuckets; i++){
       if(!(_buckets[i].empty()))return false;
     }return true;
   }
   // number of valid data
   size_t size() const {
     size_t s = 0;
     for(size_t i=0; i<_numBuckets; i++)s+=_buckets[i].size();
     return s; 
     }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
     if(empty())return false;
     vector<Data>& v = _buckets[bucketNum(d)];
     for(size_t i = 0, s= v.size();i<s;i++){
       if(v[i]==d)return true;
     }return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d)  { 
     if(empty())return false;
     vector<Data>& v = _buckets[bucketNum(d)];
     for(size_t i = 0, s= v.size();i<s;i++){
       if(v[i]==d){ d = v[i];return true; }
     }return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
     if(empty())return false;
     vector<Data>& v = _buckets[bucketNum(d)];
     for(size_t i = 0, s= v.size();i<s;i++){
       if(v[i]==d){ v[i] = d; return true;}
     }return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
     if(check(d))return false;
     _buckets[bucketNum(d)].push_back(d);
     return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
     if(!check(d)||empty())return false;
     vector<Data>& v = _buckets[bucketNum(d)] ;
     for(size_t i =0,end = v.size();i<end;i++){
       if(v[i] == d){
         for(size_t j =i;j<end-1;j++)v[j]=v[j+1];
         v.pop_back();
         return true;
       }
     }
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
