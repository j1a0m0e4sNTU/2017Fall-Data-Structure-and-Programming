/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();
   int count = 0, t = 0;
   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      count=t=0;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : /* TODO */
               if( _readBufPtr == _readBuf ) mybeep();
               else{
               moveBufPtr( _readBufPtr -1 );
               deleteChar();
               }
               break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: /* TODO */moveBufPtr(_readBufPtr + 1); break;
         case ARROW_LEFT_KEY : /* TODO */moveBufPtr(_readBufPtr - 1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : /* TODO */ 
              for(; & _readBuf[count] != _readBufPtr ; count++);
              t = 8 - (count % 8);
              insertChar(' ',t);
              break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...
   for(char* c=_readBuf;c!=_readBufEnd+1;c++){
      if(c==ptr){
      int count=0;
         for(char* p=_readBuf;p!=_readBufPtr;p++)count++;
         if(ptr==_readBuf)for(int i=0;i<count;i++)cout<<char(8);
         else if(ptr==_readBufEnd)for(char* p=_readBufPtr;p!=_readBufEnd;p++)cout<<*p<<flush;//
         else if(ptr==_readBufPtr-1)cout<<char(8);
         else if(ptr==_readBufPtr+1)cout<<*_readBufPtr<<flush;
      _readBufPtr=ptr;
      return true;
      }
   }
   mybeep();
   return false;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   if(_readBufPtr==_readBufEnd){mybeep();return false;}
   *_readBufEnd=' ';
   for(char* c=_readBufPtr;c!=_readBufEnd;c++)*c=*(c+1);
   for(char* c=_readBufPtr;c!=_readBufEnd;c++)cout<<*c<<flush;
   char* c=_readBufEnd;
   while(c!=_readBufPtr){cout<<char(8);c--;}
   *_readBufEnd=*(_readBufEnd-1)=0;
   _readBufEnd=_readBufEnd-1;
   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);
   if( _readBuf==_readBufEnd){
       for(int i=0;i<repeat;i++) {_readBuf[i]=ch; cout<<ch<<flush;}
       _readBufEnd=_readBufPtr=  _readBuf + repeat;    
   }else{
       for(char* c=_readBufEnd-1;c!=_readBufPtr-1;c--)*(c+repeat)=*c;
       for(int i=0;i<repeat;i++)*(_readBufPtr+i)=ch;
       _readBufEnd=_readBufEnd+repeat;
       for(char* c=_readBufPtr;c!=_readBufEnd;c++)cout<<*c<<flush;
       _readBufPtr=_readBufPtr+repeat;
       char* c = _readBufEnd;
       while(c!=_readBufPtr){cout<<char(8); c--;}
   }
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
   moveBufPtr( _readBuf );
   int count=0;
   for(char* c=_readBuf ; c != _readBufEnd ; c++){ cout<<' '<<flush; *c=0 ; count++; }
   for(int i = 0; i < count; i++)cout<<char(8);
   _readBufPtr = _readBufEnd = _readBuf;
}

// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
   int size= _history.size();
   if ( index < 0 ) index = 0;
   if ( index == _historyIdx ) mybeep();
   else if (index < _historyIdx){ //moving up
       if (  _historyIdx == 0 ) mybeep();
       else if(( _tempCmdStored == true  && _historyIdx == size -1) ||
               ( _tempCmdStored == false && _historyIdx == size ) ){
          if(_readBuf == _readBufEnd) _history.push_back("");
          else _history.push_back( _readBuf );
          _tempCmdStored = true;
          deleteLine();
          string::iterator it=_history[ index ].begin();
          for(int i =0 ;it != _history[ index ].end();it++, i++) {
          _readBuf[i] = _history[ index ][i]; 
          _readBufEnd = _readBufEnd + 1 ; 
          _readBufPtr = _readBufPtr + 1 ;
          }
          cout<<  _readBuf << flush;
          _historyIdx = index;
       }
       else{
       deleteLine();
       string::iterator it=_history[ index ].begin();
       for(int i =0 ;it != _history[ index ].end();it++, i++) {
       _readBuf[i] = _history[ index ][i]; 
       _readBufEnd = _readBufEnd + 1 ; 
       _readBufPtr = _readBufPtr + 1 ;
       }
       cout<<  _readBuf << flush;
       _historyIdx = index;
       }
   }else if( index > _historyIdx ){ //moving down
      if(( _tempCmdStored == true  && _historyIdx == size -1) ||
         ( _tempCmdStored == false && _historyIdx == size ) ) mybeep();
      else{
       if( _tempCmdStored == true && index > size -1) index = size -1 ;
       else if( _tempCmdStored == false && index > size ) index = size ;  
       deleteLine();
       string::iterator it=_history[ index ].begin();
       for(int i =0 ;it != _history[ index ].end();it++, i++) {
       _readBuf[i] = _history[ index ][i]; 
       _readBufEnd = _readBufEnd + 1 ; 
       _readBufPtr = _readBufPtr + 1 ;
       }
       cout<<  _readBuf << flush;
       if( _tempCmdStored == true  && index == size -1 ) {
       _tempCmdStored = false;
       _history.pop_back();
       } 
       _historyIdx = index;
       }
   }  
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
   moveBufPtr( _readBuf );
   while( _readBuf[0]==' ' ) deleteChar();
   if( _readBuf == _readBufEnd  && _tempCmdStored == true ){ _history.pop_back(); _tempCmdStored = false; }
   else if( _readBuf != _readBufEnd ){
   moveBufPtr( _readBufEnd );
   while( *(_readBufPtr-1)==' ' )moveBufPtr( _readBufPtr -1 );
   while( * _readBufPtr   ==' ' ) deleteChar();        //1.done
   if( _history.size()==0 ) {
   _history.push_back( _readBuf );
   for(char* c = _readBuf; c != _readBufEnd; c++) *c = 0;
   }
   else if( _tempCmdStored == true ) {
   _tempCmdStored = false ;
   _history[_history.size()-1] = _readBuf ;
   for(char* c = _readBuf; c != _readBufEnd; c++) *c = 0;
   }else { 
    _history.push_back( _readBuf ) ;
    for(char* c = _readBuf; c != _readBufEnd; c++) *c = 0;
    }
   }_historyIdx = _history.size() ;
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
