/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
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



/**************************************/
/*   Static varaibles and functions   */
/**************************************/


/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName) //performance has room to be improved
{                                           // use hash to implement getGate?
  ifstream file;
   file.open(fileName);
   if( ! file.is_open()){cout<<"Cannot open design \""<<fileName<<"\"!!"<<endl; return false;}
   string s;                        
   unsigned l = 1, temp;
   int x;
   CirGate* gtemp; CirGate* g[3] = {0,0,0} ;
   vector<string>       vs;
   vector<unsigned>     vi;

   getline(file,s); parse(s,vs); trans(vs,vi);
   M = vi[0]; I = vi[1]; L = vi[2]; O = vi[3]; A = vi[4];
   cktSize = M+O+1;
   ckt.resize(cktSize); 
   CirGate* gptr = new PIgate(0,0); gptr -> _type = CONST_GATE;
   ckt[0] = gptr;
   for( unsigned j=0 ; j<I; j++){
     getline(file,s); l++;
     myStr2Int(s, x);
     temp = (unsigned)x;
     CirGate* pig = new PIgate(temp/2,l);
     ckt[temp/2] = pig; PIgate* i =(PIgate*)pig;
     pi.push_back(i);
   }
   for( unsigned i=0 , num = M +1 ; i<O; i++,num++){
     getline(file,s); l++;
     myStr2Int(s, x); temp = (unsigned)x; 
     CirGate* pog = new POgate(num, l ); 
     CirGate* in = getGate(temp/2);
     if(in==0)in = new AIGgate(temp/2,l);
     setFin(pog,0,in);
     if(temp %2== 1)setInv(pog,0,true);
     addFout(in,pog);
     ckt[num] = pog ; ckt[temp/2] = in;
     POgate* o = (POgate*)pog;
     po.push_back(o);
   } //PI,PO are done
   while(getline(file,s)){
     l++;
     parse(s,vs);
     if(myStr2Int(vs[0],x)){ // set gates 
       trans(vs, vi);
       for(int i = 0; i < 3; i++){
         gtemp = getGate(vi[i]/2);
         if(gtemp !=0){ g[i] = gtemp; g[i]->_line = l;}
         else { g[i] = new AIGgate(vi[i]/2,l); ckt[vi[i]/2] = g[i]; }
       }
       setFin(g[0],0,g[1]);
       if(vi[1]%2==1) setInv(g[0],0,true);
       addFout(g[1],g[0]);
       setFin(g[0],1,g[2]);
       if(vi[2]%2==1) setInv(g[0],1,true);
       addFout(g[2],g[0]);
       g[0]=g[1]=g[2]=0;
     }else{  //assign gate's name   -
        string::iterator c = s.begin();
        int x;
        if(*c == 'i'){
          s = s.substr(1);
          parse(s, vs); myStr2Int(vs[0],x); 
          pi[x] -> setName(vs[1]);
        }else if(*c == 'o'){ 
          s = s.substr(1);
          parse(s, vs); myStr2Int(vs[0],x); 
          po[x] -> setName(vs[1]);
        }else break;
     }
   }
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
void 
CirMgr::setFin(CirGate*& g1,int i,CirGate*& g2){
  if(g1==0||g2==0)return; if(g1 == g2)return;
  assert(g1->_type == PO_GATE || g1->isAig());assert(i==0||i==1);
  if( g1 -> isAig()){
    AIGgate* gptr = static_cast<AIGgate*>(g1);
    gptr -> _fin[i] = g2;
  }else {
    POgate* gptr = static_cast<POgate*>(g1);
    gptr -> _fin[i] = g2;
  }
}
void 
CirMgr::setInv(CirGate*& g1,int i, bool b){
  if(g1==0)return;
  assert(g1->_type == PO_GATE || g1->isAig());assert(i==0||i==1);
  if( g1 -> isAig()){
    AIGgate* gptr = static_cast<AIGgate*>(g1);
    gptr -> _inv[i] = b;
  }else {
    POgate* gptr = static_cast<POgate*>(g1);
    gptr -> _inv[i] = b;
  }
}
void 
CirMgr::addFout(CirGate*& g1, CirGate*& g2){
  if(g1==0||g2==0)return; if(g1 == g2)return;
  assert(g1->_type == PI_GATE || g1->_type==CONST_GATE|| g1->isAig());
  if(g1 -> isAig()){
    AIGgate* gptr = static_cast<AIGgate*>(g1);
    gptr -> _fout.push_back(g2);
  }else {
    PIgate* gptr = static_cast<PIgate*>(g1);
    gptr -> _fout.push_back(g2);
  }
}
void 
CirMgr::delFout(CirGate*& g1,CirGate*& g2){
  if(g1==0||g2==0)return; if( g1 == g2)return;
  assert(g1->_type == PI_GATE || g1->_type==CONST_GATE|| g1->isAig());
  if( g1 -> isAig()){
    AIGgate* gptr = static_cast<AIGgate*>(g1);
    for(int i=0,size=gptr->_fout.size();i<size;i++){
      if(gptr->_fout[i]==g2){gptr->_fout.erase(gptr->_fout.begin()+i);break;}
    }
  }else {
    PIgate* gptr = static_cast<PIgate*>(g1);
    for(int i=0,size=gptr->_fout.size();i<size;i++){
      if(gptr->_fout[i]==g2){gptr->_fout.erase(gptr->_fout.begin()+i);break;}
    }
  }
}
void 
CirMgr::delFin(CirGate*& g1,CirGate*&g2){
  if(g1==0||g2==0)return; if(g1 == g2 )return;
  assert(g1->_type==PO_GATE||g1->isAig());
  if(g1->isAig()){
    AIGgate* gptr = static_cast<AIGgate*>(g1);
    for(int i=0;i<2;i++){if(gptr->_fin[i]==g2)gptr->_fin[i]=0;}
  }else{
    POgate* gptr = static_cast<POgate*>(g1);
    if(gptr->_fin[0]==g2)gptr->_fin[0]=0;
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
    if(ckt[i]==0)continue;
    if(ckt[i]->_type == AIG_GATE && !(ckt[i]->undef()))aig++;
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
    if(ckt[i]==0)continue;
    if(ckt[i]->_type == PO_GATE){
      printN(count, ckt[i] );
    }
  }
}
void 
CirMgr::printN(int& count,CirGate* gptr) const
{
   if(gptr == 0 || gptr -> is_marked())return;
   if(gptr -> undef()){gptr -> mark();return;} 
   else{
     gptr->mark();
     if(gptr -> isAig()){
       const AIGgate* g = static_cast< const AIGgate*> (gptr);
       printN(count,g->_fin[0]);printN(count,g->_fin[1]);
     }else if( gptr -> _type == PO_GATE){
       const POgate* g = static_cast< const POgate*> (gptr);
       printN(count,g->_fin[0]);
     }
     cout<<"["<<count<<"] "; gptr -> printGate(); 
     count++; 
   }
}
void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(int i = 0,size = ckt.size(); i<size; i++){
     if(ckt[i]==0)continue;
     if(ckt[i]->_type == PI_GATE)cout<<" "<<i;
   }cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i = 0,size = ckt.size(); i<size; i++){
     if(ckt[i]==0)continue;
     if(ckt[i]->_type == PO_GATE)cout<<" "<<i;
   }cout << endl;
}

void
CirMgr::printFloatGates() const
{
  vector<unsigned> undefined;
  vector<unsigned> unused;
  for(int i = 0, size = ckt.size(); i < size; i++){
    if(ckt[i]==0)continue;
    if((ckt[i]->isAig()||ckt[i]->_type == PI_GATE) && ckt[i]->unuse())unused.push_back(i);
    if(ckt[i] -> isAig()){
      const AIGgate* g = static_cast<const AIGgate*>(ckt[i]);
      if(g->_fin[0]!=0)if(g->_fin[0]->undef()){ undefined.push_back( i ); continue; }
      if(g->_fin[1]!=0)if(g->_fin[1]->undef()){ undefined.push_back( i ); continue; }
    }else if(ckt[i]-> _type == PO_GATE){
      const POgate* g = static_cast<const POgate*>(ckt[i]);
      if(g->_fin[0]!=0)if(g->_fin[0]->undef()){ undefined.push_back( i ); continue; }
      if(g->_fin[1]!=0)if(g->_fin[1]->undef()){ undefined.push_back( i ); continue; }
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
CirMgr::printFECPairs() const
{
  fectable.print(); 
}

void
CirMgr::writeAag(ostream& os) const
{
  CirGate::REF++;
  unsigned a = 0, size = ckt.size();
  for(unsigned i = 0; i<size; i++){
    if(ckt[i]->isAig() && !(ckt[i]->unuse())&& !(ckt[i]->undef()))a++;
  }
  os<<"aag "<<M<<" "<<I<<" "<<L<<" "<<O<<" "<<a<<endl;
  for(unsigned i = 0; i<size; i++){ 
    if(ckt[i]==0)continue;
    if(!(ckt[i]->_type==PI_GATE))continue;
    os<<(ckt[i]->_id)*2<<endl; 
   }
  for(unsigned i = 0; i<size; i++){
    if(ckt[i]==0)continue;
    if(!(ckt[i]->_type==PO_GATE))continue;
    const POgate* g = static_cast<const POgate*>(ckt[i]);
    if(g ->_inv[0])os<<(g ->_fin[0]->_id)*2+1<<endl;
    else os<<(g ->_fin[0]->_id)*2<<endl;
  }
  for(unsigned i = 0; i<size; i++){
    if(ckt[i]==0)continue;
    if(!(ckt[i]->_type == PO_GATE))continue;
    const POgate* g = static_cast<const POgate*>(ckt[i]);
    writeGate(os,g ->_fin[0]);
  }
  int count = -1;
  for(unsigned i = 0; i < size; i++){
    if(ckt[i]==0)continue;
    if(!(ckt[i]->_type == PI_GATE) )continue;
    count++;
    const PIgate* g = static_cast<const PIgate*>(ckt[i]);
    if(g ->_name =="" )continue;
    os<<"i"<<count<<" "<<g ->_name<<endl;
  }
  count = -1;
  for(unsigned i =0; i<size;i++){
    if(ckt[i]==0)continue;
    if(!(ckt[i]->_type == PO_GATE))continue;
    count++;
    const POgate* g = static_cast<const POgate*>(ckt[i]);
    if(g ->_name =="" )continue;
    os<<"o"<<count<<" "<<g ->_name<<endl;
  }
  os<<"c\n"<<"AAG output by j1a0m0e4s~~"<<endl;
}

void
CirMgr::writeGate(ostream& os, CirGate *gptr) const
{
  if(gptr == 0)return ; if(gptr -> is_marked())return;
  if(gptr -> undef()||!(gptr -> _type == AIG_GATE )) { gptr -> mark(); return ; }
  const AIGgate* g = static_cast<const AIGgate*>(gptr);
  writeGate(os,g -> _fin[0]);
  writeGate(os,g -> _fin[1]);
  os<<(g ->_id)*2;
  for(int j =0;j<2; j++){
    if(g ->_inv[j]) os<<" "<<(g ->_fin[j]->_id)*2+1;
    else os<<" "<<(g ->_fin[j]->_id)*2;
  }os<<endl;
  gptr -> mark();
}
