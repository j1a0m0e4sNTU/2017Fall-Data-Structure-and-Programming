/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

int CirGate::REF = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}



/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream file;
   file.open(fileName);
   if( ! file.is_open()){cout<<"Cannot open design \""<<fileName<<"\"!!"<<endl; return false;}
   string s;  
   unsigned l = 1,temp;
   int x;
   vector<string>       vs;
   vector<unsigned>     vi;
   getline(file,s); parse(s,vs); trans(vs,vi);
   M = vi[0]; I = vi[1]; L = vi[2]; O = vi[3]; A = vi[4];
   CirGate* gptr = new CirGate(0,0,CONST_GATE);
   ckt.push_back(gptr);
   for( unsigned i=0 ; i<I; i++){
     getline(file,s); l++;
     myStr2Int(s, x);
     temp = (unsigned)x;
     CirGate* pi = new CirGate(temp/2,l,PI_GATE);
     ckt.push_back(pi);
   }
   for( unsigned i=0 , num = M +1 ; i<O; i++,num++){
     getline(file,s); l++;
     myStr2Int(s, x);
     temp = (unsigned)x;
     CirGate* po = new CirGate(num, l, PO_GATE );
     CirGate* gptr = new CirGate(temp/2, l ); // l should be modified later
     po -> _fin[0] = gptr ;
     if(temp%2 ==1) po->_inv[0] = true;
     gptr -> _fout.push_back(po);
     ckt.push_back(po);
     ckt.push_back(gptr);
   } //PI,PO are done
   while(getline(file,s)){
     l++;
     parse(s,vs);
     if(myStr2Int(vs[0],x)){ // set gates 
       trans(vs, vi);
       CirGate* g[3] = {0} ;
       int size = vi.size();
       if(getGate(vi[0]/2)!=0)getGate(vi[0]/2) -> line = l;
       for(int i = 0; i < size; i++){
         if(getGate(vi[i]/2)!=0){
           g[i] = getGate(vi[i]/2);
          }
         else {
           g[i] = new CirGate(vi[i]/2,l);
           ckt.push_back(g[i]);
          }
       }
       if(size > 0){
         g[0] -> _fin[0] = g[1];
         if(vi[1]%2==1) g[0] -> _inv[0] = true;
         g[1] -> _fout.push_back(g[0]);
       }if(size >1 ){
         g[0] -> _fin[1] = g[2];
         if(vi[2]%2==1) g[0] -> _inv[1] = true;
         g[2] -> _fout.push_back(g[0]);
       }
     }else{  //not gate declaration
        string::iterator it = s.begin();
        int x;
        int size = ckt.size();
        if(*it == 'c')  break;
        if(*it == 'i'){
          s = s.substr(1);
          parse(s, vs); myStr2Int(vs[0],x);
          for(int i = 0, count = 0; i<size ; i++){
            if( ckt[i]->type == PI_GATE ){
              if(count == x) ckt[i]->setName(vs[1]);
              else count ++;
            }
          }
        }else if(*it == 'o'){
          s = s.substr(1);
          parse(s, vs); myStr2Int(vs[0],x);
          for(int i = 0, count = 0; i<size ; i++){
            if( ckt[i]->type == PO_GATE ){
              if(count == x) ckt[i]->setName(vs[1]);
              else count ++;
            }
        }
      }
     }
   }
   int size = ckt.size();
   for(int i=0; i<size; i++){ if(ckt[i]->undef())ckt[i]->line = 0; }
   return true;
}

void 
CirMgr::parse( string& s , vector<string> & v){
  v.clear();
  size_t  i = 0; string temp;
  while( i != string::npos){
    i = myStrGetTok(s,temp,i);
    v.push_back(temp);
  }
}
void
CirMgr::trans(vector<string>& vs, vector<unsigned>& vi){
  vi.clear();
  size_t size = vs.size(); int temp;
  for(size_t i = 0 ; i< size ; i++){
    if(myStr2Int(vs[i],temp))vi.push_back((unsigned)temp);
  }
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{ 
  unsigned aig = 0;
  int size = ckt.size();
  for(int i = 0; i<size; i++){
    if(ckt[i]->type == AIG_GATE && !(ckt[i]->undef()))aig++;
  }cout<<endl;
  cout<<"Circuit Statistics"   <<"\n"
      <<"=================="   <<"\n"
      <<"  PI   "<<setw(9)<<right<<I  <<"\n"
      <<"  PO   "<<setw(9)<<right<<O  <<"\n"
      <<"  AIG  "<<setw(9)<<right<<aig<<"\n"
      <<"------------------"   <<"\n"
      <<"  Total"<<setw(9)<<right<<I+O+aig
      <<endl;
    
}

void
CirMgr::printNetlist() const
{
  cout<<endl;
  CirGate::REF++; 
  int size = ckt.size(), count = 0;
  for(int i = 0; i<size; i++){
    if(ckt[i]->type == PO_GATE){
      printN(count, ckt[i] );
    }
  }
}
void 
CirMgr::printN(int& count,CirGate* gptr) const
{
   if(gptr -> undef())gptr -> mark(); 
   else if( gptr -> _fin[0]==0 && gptr ->_fin[1]==0){
     cout<<"["<<count<<"] "; gptr -> printGate(); 
     gptr->mark(); 
     count++; 
   }else{
     if( gptr -> _fin[0]!=0){
       if(! (gptr -> _fin[0]->is_marked())){
         printN(count, gptr->_fin[0]);
       }
     }
     if( gptr -> _fin[1]!=0){
       if(! (gptr -> _fin[1]->is_marked())){
         printN(count, gptr->_fin[1]);
       }
     }
     cout<<"["<<count<<"] "; gptr -> printGate(); 
     gptr->mark(); 
     count++; 
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   int size = ckt.size();
   for(int i = 0; i<size; i++){
     if(ckt[i]->type == PI_GATE)cout<<" "<<ckt[i]->id;
   }cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   int size = ckt.size();
   for(int i = 0; i<size; i++){
     if(ckt[i]->type == PO_GATE)cout<<" "<<ckt[i]->id;
   }cout << endl;
}

void
CirMgr::printFloatGates() const
{
  vector<unsigned> undefined;
  vector<unsigned> unused;
  int size = ckt.size();
  for(int i = 0 ; i < size; i++){
    if((ckt[i]->type == AIG_GATE||ckt[i]->type == PI_GATE) && ckt[i]->unuse())unused.push_back(ckt[i] -> id );
    if( ckt[i]->type == AIG_GATE||ckt[i]->type == PO_GATE){
      if(ckt[i]->_fin[0]!=0){
        if(ckt[i]->_fin[0]->undef())undefined.push_back( ckt[i] -> id );continue;
      }if(ckt[i]->_fin[1]!=0){
        if(ckt[i]->_fin[1]->undef())undefined.push_back( ckt[i] -> id );continue;
      }
    }
  }
  if(! undefined.empty()){
    cout<<"Gates with floating fanin(s):";
    for(size_t i = 0; i<undefined.size(); i++)cout<<" "<<undefined[i];
    cout<<endl;
  }
  if(! unused.empty()){
    cout<<"Gates defined but not used:";
    for(size_t i = 0; i<unused.size(); i++)cout<<" "<<unused[i];
    cout<<endl;
  }
}

void
CirMgr::writeAag(ostream& os) const{
  CirGate::REF++;
  unsigned a = 0, size = ckt.size();
  for(unsigned i = 0; i<size; i++){
    if(ckt[i]->type==AIG_GATE && !(ckt[i]->unuse())&& !(ckt[i]->undef()))a++;
  }
  os<<"aag "<<M<<" "<<I<<" "<<L<<" "<<O<<" "<<a<<endl;
  for(unsigned i = 0; i<size; i++){ 
    if(!(ckt[i]->type==PI_GATE))continue;
    os<<(ckt[i]->id)*2<<endl; 
   }
  for(unsigned i = 0; i<size; i++){
    if(!(ckt[i]->type==PO_GATE))continue;
    if(ckt[i]->_inv[0])os<<(ckt[i]->_fin[0]->id)*2+1<<endl;
    else os<<(ckt[i]->_fin[0]->id)*2<<endl;
  }
  for(unsigned i = 0; i<size; i++){
    if(!(ckt[i]->type == PO_GATE))continue;
    outputAIG(os,ckt[i]->_fin[0]);
  }
  int count = -1;
  for(unsigned i = 0; i < size; i++){
    if(!(ckt[i]->type == PI_GATE) )continue;
    count++;
    if(ckt[i]->name =="" )continue;
    os<<"i"<<count<<" "<<ckt[i]->name<<endl;
  }
  count = -1;
  for(unsigned i =0; i<size;i++){
    if(!(ckt[i]->type == PO_GATE))continue;
    count++;
    if(ckt[i]->name =="" )continue;
    os<<"o"<<count<<" "<<ckt[i]->name<<endl;
  }
  os<<"c\n"<<"AAG output by j1a0m0e4s~~"<<endl;
}
void 
CirMgr::outputAIG(ostream& os,CirGate* gptr)const{
  if(gptr -> undef()||!(gptr -> type == AIG_GATE )) { gptr -> mark(); return ; }
  if(gptr -> _fin[0]!=0 && !( gptr -> _fin[0] -> is_marked())){
    outputAIG(os,gptr-> _fin[0]);
  }
  if(gptr -> _fin[1]!=0 && !( gptr -> _fin[1] -> is_marked())){
    outputAIG(os,gptr-> _fin[1]);
  }
  os<<(gptr->id)*2;
  for(int j =0;j<2; j++){
    if(gptr->_inv[j]) os<<" "<<(gptr->_fin[j]->id)*2+1;
    else os<<" "<<(gptr->_fin[j]->id)*2;
  }os<<endl;
  gptr -> mark();
}