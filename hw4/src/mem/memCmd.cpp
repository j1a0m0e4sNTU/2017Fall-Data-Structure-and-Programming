/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> tokens;
   int n , s;
   lexOptions( option, tokens);
   if(tokens.size() == 0) return CmdExec::errorOption(CMD_OPT_MISSING,"");
   if( myStr2Int(tokens[0],n)&&n>0){
     if(tokens.size() == 1){
     try{
        mtest.newObjs((size_t)n );
     }catch(bad_alloc&){
        return CMD_EXEC_ERROR;
     }
   }else{
     if( myStrNCmp("-Array",tokens[1],2) != 0){return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);}
     if(tokens.size()<3) { return CmdExec::errorOption(CMD_OPT_MISSING,tokens[1]);}
     if(! myStr2Int(tokens[2],s)||s<=0) {return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);}
     if( tokens.size()>3){return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);}
     try{
        mtest.newArrs((size_t)n,(size_t)s);
     }catch(bad_alloc&){
        return CMD_EXEC_ERROR;
     }
   }return CMD_EXEC_DONE;
   }else if(myStrNCmp("-Array",tokens[0],2) == 0){
      if(! myStr2Int(tokens[1],s))return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
      if(! myStr2Int(tokens[2],n))return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
      try{
        mtest.newArrs((size_t)n,(size_t)s);
     }catch(bad_alloc&){
        return CMD_EXEC_ERROR;
     }return CMD_EXEC_DONE;
   }
   return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO   rnGen()
   vector <string> tokens;
   int a ;
   lexOptions( option , tokens);
   if(tokens.size() == 0) return CmdExec::errorOption(CMD_OPT_MISSING,"");
   if(myStrNCmp("-Array",tokens[0],2)==0){
     if(tokens.size()==1) return CmdExec::errorOption(CMD_OPT_MISSING,tokens[0]);
     if(myStrNCmp("-Index",tokens[1],2)!=0&&myStrNCmp("-Random",tokens[1],2)!=0){
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
     }if(!myStr2Int(tokens[2],a)||a<0) { return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]); }
     if(tokens.size()>3){ return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[3]);}
     string temp = tokens[0];
     for(size_t i = 0 ; i<2;i++){
       tokens[i] = tokens[i+1];
     }tokens[2] = temp;
   }
   if(myStrNCmp("-Index",tokens[0],2)!=0&&myStrNCmp("-Random",tokens[0],2)!=0){
     return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }if(!myStr2Int(tokens[1],a)||a<0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
   if(tokens.size() > 2){
     if(myStrNCmp("-Array",tokens[2],2)!=0)return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
     if(tokens.size()>3)return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[3]);
   }
   
   if(myStrNCmp("-Index",tokens[0],2) ==0 ){ //-Index
     if(tokens.size() == 2){
       if( a >= (int) mtest.getObjListSize()){
       cerr<<"Size of object list (" << mtest.getObjListSize() 
           <<") is <= "<< a <<"!!"<< endl;
       return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
       }
       mtest.deleteObj((size_t)a);
       return CMD_EXEC_DONE;
     }else{ //-a
       if(a >= (int) mtest.getArrListSize()){
        cerr<<"Size of array list (" << mtest.getArrListSize() 
            <<") is <= "<< a <<"!!"<< endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
       }
       mtest.deleteArr((size_t)a);
       return CMD_EXEC_DONE;
     }
   }else{   // -Random 
    if( tokens.size()==2&&mtest.getObjListSize() == 0 ) {
      cerr<<"Size of object list is 0!!"<<endl;
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
    }if(tokens.size()==3&&mtest.getArrListSize() == 0 ) {
      cerr<<"Size of array list is 0!!"<<endl;
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
    }
     int range;
     size_t idx;
     if(tokens.size() == 2){
      range = mtest.getObjListSize();
      for(int i = 0 ; i<a ; i++){
        idx = rnGen(range);
        mtest.deleteObj(idx);
      }
     }else {  // -a
      range = mtest.getArrListSize();
      for(int i = 0 ; i<a ; i++){
        idx = rnGen(range);
        mtest.deleteArr(idx);
      }
     }
   } 
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


