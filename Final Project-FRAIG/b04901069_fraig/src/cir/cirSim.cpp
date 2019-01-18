/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "myHashMap.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
RandomNumGen rgn;
#define R 2147483647

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void 
CirMgr::sim(CirGate* gptr){
  if(gptr ->_type == PO_GATE){
    POgate* po = (POgate*)gptr ;
    sim(po->_fin[0]);
    if(po->_inv[0])po->_value = ~(po->_fin[0]->_value);
    else           po->_value =   po->_fin[0]->_value;
  }else if( gptr -> isAig()){
    if(gptr -> is_marked())return;
    if(gptr -> undef()){gptr ->mark();return;}
    AIGgate* g = (AIGgate*)gptr;
    sim(g->_fin[0]);sim(g->_fin[1]);
    size_t in1 = ( g->_inv[0])? ~(g->_fin[0]->_value) : (g->_fin[0]->_value),
           in2 = ( g->_inv[1])? ~(g->_fin[1]->_value) : (g->_fin[1]->_value);
    g -> _value = in1 & in2;
    g -> mark();
  }
}
void 
CirMgr::simOnce(int limit = 64){
  if(fectable.sim == false){ initFEC(); fectable.sim = true; }
  CirGate::REF++;
  for(unsigned i = 0,po = 0;i<cktSize&&po<O;i++){
    if(ckt[i]==0)continue;
    if(ckt[i]->_type == PO_GATE){  po++;  sim(ckt[i]); }
  }updateFEC();
  if(_simLog == 0)return;
  size_t* in = new size_t[I]; size_t* out = new size_t[O];
  for(unsigned i=0;i<I;i++)in[i] = pi[i]->_value;
  for(unsigned i=0;i<O;i++)out[i] = po[i]->_value;
  for(int i=0;i<64&&i<limit;i++){
     for(unsigned j=0;j<I;j++){
       if(in[j]%2==0)(*_simLog)<<"0";
       else          (*_simLog)<<"1";
       in[j] = in[j]/2;
     }(*_simLog)<<" ";
     for(unsigned j=0;j<O;j++){
       if(out[j]%2==0)(*_simLog)<<"0";
       else           (*_simLog)<<"1";
       out[j] = out[j]/2;
     }(*_simLog)<<endl;
  }delete [] in; delete [] out;
}
void
CirMgr::randomSim()
{
  int T = 150;  //how many times to simulate -->function of amounts of gates
  for(int t =0;t<T;t++){
    for(unsigned i=0;i<I;i++){
      size_t n1 = rgn(R), n2 = rgn(R);
      pi[i]->_value = pi[i]->_value|(n1<<32)|(n2);
    } simOnce();
  }cout<<T*64<<" patterns simulated."<<endl;
  set_fec();
}
void
CirMgr::fileSim(ifstream& patternFile)
{
  string line; char* c; int p = 0; size_t mask = 1;
  while(getline(patternFile,line)){
    if(p == 0){ if(line.size() < I){cout<<"Error: Pattern("<<line<<") length("<<line.size()
                  <<") does not match the number of inputs("<<I<<") in a circuit!!"<<endl; return;}}
    for(int i= 0;i<(int)I;i++){
      c = &line[i];
      if((*c) == '1')(pi[i]->_value)|=(mask<<(p%64));
      else if((*c) != '0')cout<<"Error: there are patterns not 0 or 1"<<endl;
    }p++;
    if( p%64 == 0 ){
      simOnce();
      for(int i = 0;i<(int)I;i++)pi[i]->_value = 0; 
    }
  }if(p%64 != 0){simOnce(p%64);}
  cout<<p<<" patterns simulated."<<endl; set_fec();
}
void 
CirMgr::simPattern(size_t* pattern){
  for(unsigned i=0; i<I; i++){
    pi[i]->_value = pattern[i];
  }simOnce(); set_fec();
}
void 
CirMgr::setSimLog(ofstream *logFile) 
{
  _simLog = logFile;
}
/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void 
CirMgr::init(FECgroup& group,CirGate* g){
  if(g==0)return;if(!g->isAig()||g->undef()||g->is_marked())return;
  g->mark();
  AIGgate* aig = (AIGgate*) g;
  init(group,aig->_fin[0]);init(group,aig->_fin[1]);
  group.add(false,aig->_id);
}
void 
CirMgr::initFEC(){
  CirGate::REF++;
  fectable.sim = true; fectable.table.clear();
  FECgroup group; group.add(false,0);
  for(int i = 0;i<(int)cktSize;i++){
    if(ckt[i]==0)continue;
    if(ckt[i]->_type==PO_GATE){
      POgate* po = (POgate*)ckt[i];
      init(group,po->_fin[0]);
    }
  }fectable.add(group);
}
void 
CirMgr::updateFEC(){
  if(fectable.sim == false)return;
  int Tsize = fectable.size();
  for(int i = 0;i<Tsize;i++){
    int Gsize = fectable[i].size(); 
    HashMap<HashKey,FECgroup> map(3);
    for(int j=0;j<Gsize;j++){
      int gateId = abs(fectable[i][j]); 
      if(ckt[gateId]==0){
        fectable[i].erase(j); 
        j--;Gsize--;
        continue;
      }if(Gsize == 1)break;

      HashKey hashkey; FECgroup temp;
      size_t v = ckt[gateId]->_value;
      v = ~v;                                    //check inversed match
      hashkey.setKey(v);
      if(map.query(hashkey,temp)){ 
        temp.add(true,gateId); map.update(hashkey,temp);
        continue;}
      v = ~v;                                   //check non-inversed match
      hashkey.setKey(v);
      if(map.query(hashkey,temp)){ 
        temp.add(false,gateId); map.update(hashkey,temp);
        continue;}
      temp.add(false,gateId);                   //isn't matched
      map.insert(hashkey,temp);
    }
    HashMap<HashKey,FECgroup>::iterator it; 
    for( it = map.begin() ;it != map.end();it++){
      if((*it).second.size()>1)fectable.add((*it).second);
    } 
  }
  fectable.remove(0,Tsize);
}

void
CirMgr::set_fec(){
  for(unsigned i = 0; i< cktSize;i++)if(ckt[i]!=0)ckt[i]->_fec = 0;
  for(int i=0,T=fectable.size();i<T;i++){
    for(int j=0,G = fectable[i].size();j<G;j++){
      if(fectable[i][j] <0 )ckt[abs(fectable[i][j])]->_fec = -(i+1);
      else                  ckt[abs(fectable[i][j])]->_fec = i+1;
    }
  }
}