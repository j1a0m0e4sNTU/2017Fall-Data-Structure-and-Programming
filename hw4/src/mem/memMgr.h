/****************************************************************************
  FileName     [ memMgr.h ]
  PackageName  [ cmd ]
  Synopsis     [ Define Memory Manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MEM_MGR_H
#define MEM_MGR_H

#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <new>

using namespace std;

// Turn this on for debugging
// #define MEM_DEBUG

//--------------------------------------------------------------------------
// Define MACROs
//--------------------------------------------------------------------------
#define MEM_MGR_INIT(T) \
MemMgr<T>* const T::_memMgr = new MemMgr<T>

#define USE_MEM_MGR(T)                                                      \
public:                                                                     \
   void* operator new(size_t t) { return (void*)(_memMgr->alloc(t)); }      \
   void* operator new[](size_t t) { return (void*)(_memMgr->allocArr(t)); } \
   void  operator delete(void* p) { _memMgr->free((T*)p); }                 \
   void  operator delete[](void* p) { _memMgr->freeArr((T*)p); }            \
   static void memReset(size_t b = 0) { _memMgr->reset(b); }                \
   static void memPrint() { _memMgr->print(); }                             \
private:                                                                    \
   static MemMgr<T>* const _memMgr

// You should use the following two MACROs whenever possible to 
// make your code 64/32-bit platform independent.
// DO NOT use 4 or 8 for sizeof(size_t) in your code
//
#define SIZE_T      sizeof(size_t)
#define SIZE_T_1    (sizeof(size_t) - 1)

// TODO: Define them by SIZE_T and/or SIZE_T_1 MACROs.
//
// To promote 't' to the nearest multiple of SIZE_T; 
// e.g. Let SIZE_T = 8;  toSizeT(7) = 8, toSizeT(12) = 16
#define toSizeT(t)      (t%SIZE_T==0)?  t : (t+SIZE_T-(t%SIZE_T)) // TODO
//
// To demote 't' to the nearest multiple of SIZE_T
// e.g. Let SIZE_T = 8;  downtoSizeT(9) = 8, downtoSizeT(100) = 96
#define downtoSizeT(t)  (t%SIZE_T==0)?  t : (t-(t%SIZE_T)) // TODO

// R_SIZE is the size of the recycle list
#define R_SIZE 256

//--------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------
template <class T> class MemMgr;


//--------------------------------------------------------------------------
// Class Definitions
//--------------------------------------------------------------------------
// T is the class that use this memory manager
//
// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemBlock
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemBlock(MemBlock<T>* n, size_t b) : _nextBlock(n) {
      _begin = _ptr = new char[b]; _end = _begin + b; }
   ~MemBlock() { delete [] _begin; }

   // Member functions
   void reset() { _ptr = _begin; }
   // 1. Get (at least) 't' bytes memory from current block
   //    Promote 't' to a multiple of SIZE_T
   // 2. Update "_ptr" accordingly
   // 3. The return memory address is stored in "ret"
   // 4. Return false if not enough memory
   bool getMem(size_t t, T*& ret) {
      // TODO
      #ifdef MEM_DEBUG
      cout << "Calling  getMem(size_t t, T*& ret) ~" << endl;
      #endif // MEM_DEBUG
      t = toSizeT(t);
      if( t > (size_t)(_end - _ptr))return false;
      ret = (T*) _ptr;
      _ptr += t;
      return true;
   }
   size_t getRemainSize() const { return size_t(_end - _ptr); }
      
   MemBlock<T>* getNextBlock() const { return _nextBlock; }

   // Data members
   char*             _begin;
   char*             _ptr;
   char*             _end;
   MemBlock<T>*      _nextBlock;
};

// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemRecycleList
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemRecycleList(size_t a = 0) : _arrSize(a), _first(0), _nextList(0) {}
   ~MemRecycleList() { reset(); }

   // Member functions
   // ----------------
   size_t getArrSize() const { return _arrSize; }
   MemRecycleList<T>* getNextList() const { return _nextList; }
   void setNextList(MemRecycleList<T>* l) { _nextList = l; }
   // pop out the first element in the recycle list
   T* popFront() {
      // TODO
      #ifdef MEM_DEBUG
      cout << "Calling   MemRecycleList::popFront()~" << endl;
      #endif // MEM_DEBUG
      T* temp = _first;
      T** tpptr=(T**) _first;
      _first = *tpptr;
      *tpptr = 0;
      return temp;
   }
   // push the element 'p' to the beginning of the recycle list
   void  pushFront(T* p) {
      // TODO
      #ifdef MEM_DEBUG
      cout << "Calling   MemRecycleList::pushFront(T* p)~" << endl;
      #endif // MEM_DEBUG 
      T** tpptr= (T**) p;
      *tpptr = _first;
      _first = p;
   }
   // Release the memory occupied by the recycle list(s)
   // DO NOT release the memory occupied by MemMgr/MemBlock
   void reset() {
      // TODO
      #ifdef MEM_DEBUG
      cout << "Calling   MemRecycleList::reset()~" << endl;
      #endif // MEM_DEBUG
      while(_first != 0){popFront();}
      if(_nextList != 0){
        MemRecycleList<T>* mptr;
        while(_nextList->_nextList!=0){
          mptr=_nextList;
          while(mptr->_nextList->_nextList!=0){
            mptr =mptr ->_nextList;
          }delete mptr->_nextList;
          mptr->_nextList=0;
        }delete _nextList;
        _nextList = 0;
      }
      
      /*
      while( _first!= 0){ popFront();}
      if( _nextList != 0){ 
        _nextList -> reset();
        delete _nextList;
        _nextList = 0; 
      } */
   }

   // Helper functions
   // ----------------
   // Iterate to the next element after 'p' in the recycle list
   T* getNext(T* p) const {
      // TODO
      T** tpptr = (T**) p;
      return *tpptr;
   }
   //
   // count the number of elements in the recycle list
   size_t numElm() const {
      size_t count = 0;
      T* p = _first;
      while (p) {
         p = getNext(p);
         ++count;
      }
      return count;
   }

   // Data members
   size_t              _arrSize;   // the array size of the recycled data
   T*                  _first;     // the first recycled data
   MemRecycleList<T>*  _nextList;  // next MemRecycleList
                                   //      with _arrSize + x*R_SIZE
};

template <class T>
class MemMgr
{
   #define S sizeof(T)

public:
   MemMgr(size_t b = 65536) : _blockSize(b) {
      assert(b % SIZE_T == 0);
      _activeBlock = new MemBlock<T>(0, _blockSize);
      for (int i = 0; i < R_SIZE; ++i)
         _recycleList[i]._arrSize = i;
   }
   ~MemMgr() { reset(); delete _activeBlock; }

   // 1. Remove the memory of all but the firstly allocated MemBlocks
   //    That is, the last MemBlock searchd from _activeBlock.
   //    reset its _ptr = _begin (by calling MemBlock::reset())
   // 2. reset _recycleList[]
   // 3. 'b' is the new _blockSize; "b = 0" means _blockSize does not change
   //    if (b != _blockSize) reallocate the memory for the first MemBlock
   // 4. Update the _activeBlock pointer
   void reset(size_t b = 0) {
      assert(b % SIZE_T == 0);
      #ifdef MEM_DEBUG
      cout << "Resetting memMgr...(" << b << ")" << endl;
      #endif // MEM_DEBUG
      // TODO
      MemBlock<T>* temp;
      while(_activeBlock -> _nextBlock != 0){
        temp = _activeBlock;
        _activeBlock = _activeBlock -> _nextBlock;
        delete temp;
      }if( b == 0){
        _activeBlock -> _ptr = _activeBlock -> _begin;
      }else{
        delete _activeBlock;
        _blockSize = b ;
        _activeBlock = new MemBlock<T>( 0, _blockSize );
      }
      for(size_t i=0 ; i < R_SIZE ; i++ ){
        _recycleList[i].reset();
      }
   }
   // Called by new
   T* alloc(size_t t) {
      assert(t == S);
      #ifdef MEM_DEBUG
      cout << "Calling alloc...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      return getMem(t);
   }
   // Called by new[]
   T* allocArr(size_t t) {
      #ifdef MEM_DEBUG
      cout << "Calling allocArr...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      // Note: no need to record the size of the array == > system will do
      return getMem(t);
   }
   // Called by delete
   void  free(T* p) {
      #ifdef MEM_DEBUG
      cout << "Calling free...(" << p << ")" << endl;
      #endif // MEM_DEBUG
      getMemRecycleList(0)->pushFront(p);
   }
   // Called by delete[]
   void  freeArr(T* p) {
      #ifdef MEM_DEBUG
      cout << "Calling freeArr...(" << p << ")" << endl;
      #endif // MEM_DEBUG
      // TODO
      // Get the array size 'n' stored by system,
      // which is also the _recycleList index
      size_t n = 0;
      size_t* sptr = (size_t*) p;
      n = *sptr ;
      #ifdef MEM_DEBUG
      cout << ">> Array size = " << n << endl;
      cout << "Recycling " << p << " to _recycleList[" << n << "]" << endl;
      #endif // MEM_DEBUG
      // add to recycle list...
      getMemRecycleList(n) -> pushFront( p );
   }
   void print() const {
      cout << "=========================================" << endl
           << "=              Memory Manager           =" << endl
           << "=========================================" << endl
           << "* Block size            : " << _blockSize << " Bytes" << endl
           << "* Number of blocks      : " << getNumBlocks() << endl
           << "* Free mem in last block: " << _activeBlock->getRemainSize()
           << endl
           << "* Recycle list          : " << endl;
      int i = 0, count = 0;
      while (i < R_SIZE) {
         const MemRecycleList<T>* ll = &(_recycleList[i]);
         while (ll != 0) {
            size_t s = ll->numElm();
            if (s) {
               cout << "[" << setw(3) << right << ll->_arrSize << "] = "
                    << setw(10) << left << s;
               if (++count % 4 == 0) cout << endl;
            }
            ll = ll->_nextList;
         }
         ++i;
      }
      cout << endl;
   }

private:
   size_t                     _blockSize;
   MemBlock<T>*               _activeBlock;
   MemRecycleList<T>          _recycleList[R_SIZE];

   // Private member functions
   //
   // t: #Bytes; MUST be a multiple of SIZE_T
   // return the size of the array with respect to memory size t
   // [Note] t must >= S
   // [NOTE] Use this function in (at least) getMem() to get the size of array
   //        and call getMemRecycleList() later to get the index for
   //        the _recycleList[]
   size_t getArraySize(size_t t) const {
      assert(t % SIZE_T == 0);
      assert(t >= S);
      // TODO
      size_t s= toSizeT(S) ;
      size_t count = 0;
      while( t != s){
        count++;
        s=SIZE_T + count * S;
        s=toSizeT(s);
      }
      return count;
   }
   // Go through _recycleList[m], its _nextList, and _nexList->_nextList, etc,
   //    to find a recycle list whose "_arrSize" == "n"
   // If not found, create a new MemRecycleList with _arrSize = n
   //    and add to the last MemRecycleList
   // So, should never return NULL
   // [Note]: This function will be called by MemMgr->getMem() to get the
   //         recycle list. Therefore, the recycle list is first created
   //         by the MTNew command, not MTDelete.
   MemRecycleList<T>* getMemRecycleList(size_t n) {
      size_t m = n % R_SIZE;
      // TODO
      MemRecycleList<T>* mptr = & _recycleList[m];
      while( mptr -> _arrSize != n && mptr-> getNextList()!=0 ){ mptr = mptr -> getNextList(); }
      if( mptr -> _arrSize != n){
        mptr -> _nextList = new MemRecycleList<T>(n);
        mptr = mptr -> getNextList();
      }return mptr;
   }
   // t is the #Bytes requested from new or new[]
   // Note: Make sure the returned memory is a multiple of SIZE_T
   T* getMem(size_t t) {
      T* ret = 0;
      #ifdef MEM_DEBUG
      cout << "Calling MemMgr::getMem...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      // TODO ---
      // 1. Make sure to promote t to a multiple of SIZE_T
      // 2. Check if the requested memory is greater than the block size.
      //    If so, throw a "bad_alloc()" exception.
      //    cerr << "Requested memory (" << t << ") is greater than block size"
      //         << "(" << _blockSize << "). " << "Exception raised...\n";
      // 3. Check the _recycleList first...
      //    #ifdef MEM_DEBUG
      //    cout << "Recycled from _recycleList[" << n << "]..." << ret << endl;
      //    #endif // MEM_DEBUG
      //    => 'n' is the size of array
      //    => "ret" is the return address

      // If no match from recycle list...
      // 4. Get the memory from _activeBlock
      // 5. If not enough, recycle the remained memory and print out ---
      //    Note: recycle to the as biggest array index as possible
      //    Note: rn is the array size
      //    #ifdef MEM_DEBUG
      //    cout << "Recycling " << ret << " to _recycleList[" << rn << "]\n";
      //    #endif // MEM_DEBUG
      //    ==> allocate a new memory block, and print out ---
      //    #ifdef MEM_DEBUG
      //    cout << "New MemBlock... " << _activeBlock << endl;
      //    #endif // MEM_DEBUG
      // 6. At the end, print out the acquired memory address
      //    #ifdef MEM_DEBUG
      //    cout << "Memory acquired... " << ret << endl;
      //    #endif // MEM_DEBUG
      t = toSizeT(t) ; //1
      if(t > _blockSize){
        cerr << "Requested memory (" << t << ") is greater than block size"
             << "(" << _blockSize << "). " << "Exception raised...\n";
        throw bad_alloc();
      } //2
      size_t size = getArraySize(t);
      if(getMemRecycleList(size) -> _first != 0){
        #ifdef MEM_DEBUG
        cout << "Recycled from _recycleList[" <<size<< "]..." << ret << endl;
        #endif // MEM_DEBUG
        return getMemRecycleList(size) -> popFront();
      } //3
      if(!( _activeBlock -> getMem(t,ret))){
        size_t remain = (size_t)(_activeBlock->_end - _activeBlock->_ptr);
        remain = downtoSizeT(remain);
        size_t tos = toSizeT(S), size = 0  ;
        if( remain >= tos){ //5 
          while(remain >= tos ){ 
            size++ ;
            tos = SIZE_T + size * S;
            tos =toSizeT(tos);
          }size --;
          ret = (T*) ( _activeBlock -> _ptr );
          getMemRecycleList(size) -> pushFront(ret);
          #ifdef MEM_DEBUG
          cout << "Recycling " << ret << " to _recycleList[" << size<< "]\n";
          #endif // MEM_DEBUG
        }
        MemBlock<T>* temp = new MemBlock<T>(_activeBlock,_blockSize);
        _activeBlock = temp ;
        _activeBlock -> getMem(t,ret);
      }
      #ifdef MEM_DEBUG
      cout << "Memory acquired... " << ret << endl;
      #endif // MEM_DEBUG
      return ret;
   }
   // Get the currently allocated number of MemBlock's
   size_t getNumBlocks() const {
      // TODO
      size_t count = 1;
      MemBlock<T>* mptr = _activeBlock;
      while( mptr -> _nextBlock != 0){
        mptr = mptr -> _nextBlock ;
        ++count;
      }return count;
   }

};

#endif // MEM_MGR_H
