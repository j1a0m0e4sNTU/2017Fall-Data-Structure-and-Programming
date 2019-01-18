/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };
//
class HashKey
{
public:
  HashKey():key(0){}
  size_t operator()()const{ return key;}
  bool operator == (const HashKey& k)const {return key == k.key;}
  void contain(bool b1,unsigned x,bool b2,unsigned y){
    if(b1)x = 2*x+1;else x = 2*x;
    if(b2)y = 2*y+1;else y = 2*y; 
    size_t s = (x>y)? y:x, b = x+y-s;
    key = (s<<32)|b;
  }
  void setKey(size_t k){ key = k; }
private:
  size_t key;
};

template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   
   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

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
      friend class HashMap<HashKey,HashData>;
   public:
      iterator(HashMap<HashKey,HashData>* h = 0,HashNode* d = 0):dptr(d){
        hash = new HashMap<HashKey,HashData>(0);
        delete hash; hash = h;
      }
      ~iterator(){}
      const HashNode& operator * () const { return (*dptr); }
      HashNode& operator * () { return (*dptr); }
      iterator& operator ++ () {  //++iterator 
        if( hash->isLast(*dptr)){
          size_t  s = hash->bucketNum((*dptr).first)+1;
          while(hash->_buckets[s].empty()){ s++; }
          if(s == hash -> _numBuckets )dptr = 0;
          else dptr = & (hash->_buckets[s].front());
        }else dptr++;
        return (*this);
      }
      iterator  operator ++(int){ iterator temp(*this); ++(*this); return temp; } 
      
      void operator =  (const iterator& i){ hash = i.hash;dptr = i.dptr;}
      bool operator == (const iterator& i) const { return (dptr == i.dptr);}
      bool operator != (const iterator& i) const { return (dptr != i.dptr);}
   private:
      HashNode* dptr;
      HashMap<HashKey,HashData>* hash;
   };
   bool isLast(const HashNode& d){
     if(_buckets[bucketNum(d.first)].empty())return true;
     if(d == _buckets[bucketNum(d.first)].back())return true;
     else return false;
   }
   // Point to the first valid data
   iterator begin() const { 
     size_t s = 0;
     while (_buckets[s].empty())s++;
     HashNode* dptr = &(_buckets[s].front());
     iterator it(const_cast<HashMap<HashKey,HashData>*>(this),dptr);
     return it;
    }
   // Pass the end
   iterator end()  const { 
     iterator it(const_cast<HashMap<HashKey,HashData>*>(this),0);
     return it;
   }
   
   // TODO: implement these functions
   //
   // return true if no valid data
   bool empty() const {
     for(int i=0;i<_numBuckets;i++){
       if(!_buckets[i].empty())return false;
     }return true;
   }
   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const {
     size_t N = bucketNum(k);
     for(int i =0,size = _buckets[N].size();i<size;i++){
       if(k == _buckets[N][i].first)return true;
     }return false;
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const { 
     size_t N = bucketNum(k);
     for(int i =0,size = _buckets[N].size();i<size;i++){
       if(k == _buckets[N][i].first){
         d = _buckets[N][i].second;
         return true;
       }
     }return false;
  }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) {
     size_t N = bucketNum(k);     
     for(int i =0,size = _buckets[N].size();i<size;i++){
       if(k == _buckets[N][i].first){
         _buckets[N][i].second = d;
         return true;
       }
     }return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
     if(check(k))return false;
     HashNode n ;
     n.first = k; n.second = d;
     _buckets[bucketNum(k)].push_back(n);
     return true;
   }
   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) {
     size_t N = bucketNum(k);     
     for(size_t i =0,size = _buckets[N].size();i<size;i++){
       if(k == _buckets[N][i].first){
         _buckets[N].erase(_buckets[N].begin()+i);
         return true;
       }
     }return false;
   }

   size_t size() const{
     size_t size = 0;
     for(size_t  i = 0 ; i<_numBuckets;i++){
       if(!_buckets[i].empty())size+=_buckets[i].size();
     }
     return size ;
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
