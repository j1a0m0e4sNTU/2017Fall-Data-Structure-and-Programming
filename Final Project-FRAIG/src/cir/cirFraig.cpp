/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
  CirGate::REF++;
  HashMap<HashKey,CirGate*> hash(M);
  for(unsigned i=0;i<M+O+1;i++){
    if(ckt[i]==0)continue;
    if(ckt[i]->_type == PO_GATE){
      POgate* po = (POgate*)(ckt[i]);
      s(hash,po->_fin[0]);
    }
  }
}
void
CirMgr::s(HashMap<HashKey,CirGate*>& hash,CirGate* gptr){
  if( !gptr->isAig()||gptr->is_marked()||gptr->undef())return;
  HashKey key;
  AIGgate* g =(AIGgate*)gptr;
  CirGate* temp = 0;
  s(hash,g->_fin[0]);
  s(hash,g->_fin[1]);
  key.contain(g->_inv[0],g->_fin[0]->getID(),g->_inv[1],g->_fin[1]->getID());
  if(hash.query(key,temp)){
    cout<<"Strashing: "<<temp->_id<<" merging "<<gptr->_id<<"..."<<endl;
    merge(temp,gptr,false);
  }else {
    hash.insert(key,gptr);
    gptr -> mark();
  }
}

//********************************/Fraig**********************************
//************************************************************************

void
CirMgr::fraig(){
  SatSolver solver;
  solver.initialize();
  buildModel(solver);
  size_t  pattern [I] ={0};
  bool result; 
  bool Break=false; int count = 0; 
  
  while(!fectable.empty()){
    CirGate* table[fectable.size()] = {0};
    Break = false;   buildDFS(); 
    for(int i = 0,size = dfs.size();i<size&&(!Break);i++){
      int n = dfs[i] -> groupNum() ; if(n == -1)continue;
      if(table[n] == 0)table[n] = dfs[i];
      else{
        bool inv = ( table[n] -> groupInv())^( dfs[i] -> groupInv());
        Var newV = solver.newVar();
        solver.addXorCNF(newV, table[n]->getVar(),inv, dfs[i]->getVar(),false);
        solver.assumeRelease(); 
        solver.assumeProperty(ckt[0]->getVar(),false);
        solver.assumeProperty(newV, true);
        result = solver.assumpSolve();
        conclude(table[n],dfs[i],result,inv,
                 solver,pattern,count,Break);
      }
    } if(Break == false ){fectable.clear();}
  }
}
void 
CirMgr::buildModel(SatSolver& s){
  for(unsigned i=0;i<cktSize;i++){
    if(ckt[i] == 0)continue;
    if(ckt[i]->_type == PO_GATE)continue;
    Var v = s.newVar();
    ckt[i]->setVar(v);
    if(i == 0)s.assumeProperty(v,false);
  }
  for(unsigned i=0;i<cktSize;i++){
    if(ckt[i]==0)continue;
    if(ckt[i]->isAig()){
      AIGgate* g = (AIGgate*) ckt[i] ;
      s.addAigCNF(g->getVar(),
                  g->_fin[0]->getVar(),g->_inv[0],
                  g->_fin[1]->getVar(),g->_inv[1]); 
    }
  }
}

void 
CirMgr::conclude(CirGate* gptr1, CirGate* gptr2 , bool result ,bool inv ,
                 SatSolver& solver  ,size_t* pattern ,int& count , bool& Break ){
  if(result == true){                      //SAT  --> not equal
    for(unsigned i =0;i<I; i++){
      size_t temp = solver.getValue(pi[i]->getVar());
      pattern[i] = pattern[i]|(temp<<(count%64)); 
    }count++;
    if(count%64 == 0){
      simPattern(pattern);
      for(unsigned i = 0;i<I;i++)pattern[i]=0;
      Break = true; 
      cout<<"Updating by SAT...";cout<<"#FEC groups: "<<fectable.size()<<endl;
      solver.initialize(); buildModel(solver);
    }
  }else{                                   //UNSAT  -->  equal
    if(gptr2->_id==0){
      cout<<"Fraig: "<<gptr2->_id<<" merging "; if(inv){cout<<"!";}cout<<gptr1->_id<<"..."<<endl;
      merge(gptr2,gptr1,inv);
    }else {
      cout<<"Fraig: "<<gptr1->_id<<" merging "; if(inv){cout<<"!";}cout<<gptr2->_id<<"..."<<endl;
      merge(gptr1,gptr2,inv);
    }
  }
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void 
CirMgr::merge(CirGate* gptr1,CirGate* gptr2, bool inv = false){  //gptr1 stay
  if(gptr1 == 0 || gptr2 == 0)return;
  if(gptr2 -> _id == 0 || !gptr2->isAig())return; if(!gptr1->isAig()&&gptr1->_id!= 0)return;

  AIGgate* g2 = static_cast<AIGgate*>(gptr2);
  delFout(g2->_fin[0],gptr2);delFout(g2->_fin[1],gptr2);
  for(int i=0,out = g2->_fout.size();i<out;i++){
    addFout(gptr1,g2->_fout[i]);
    if(inv)changeInv(g2 ->_fout[i],gptr2);
    replaceFin(g2->_fout[i],gptr2,gptr1);
  } 
  delete ckt[gptr2->_id];
  ckt[gptr2->_id] = 0;
}

void
CirMgr::buildDFS(){
  CirGate::REF++;
  dfs.clear();
  dfs.push_back(ckt[0]);
  for(unsigned i =0;i<O;i++){ build(po[i]->_fin[0]); }
}

void
CirMgr::build(CirGate* gptr){
  if(gptr == 0)return;
  if(!gptr->isAig()||gptr->is_marked())return;
  AIGgate* g = (AIGgate*)gptr;
  build(g->_fin[0]); build(g->_fin[1]);
  gptr -> mark();
  dfs.push_back(gptr);
}
