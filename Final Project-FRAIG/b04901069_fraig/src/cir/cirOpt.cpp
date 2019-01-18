/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  CirGate::REF++;
  for(int i=0,size=ckt.size();i<size;i++){
    if(ckt[i]==0)continue;
    if(ckt[i]->_type==PO_GATE)markDFS(ckt[i]);
  }
  for(int i =0,size=ckt.size();i<size;i++){
    if(ckt[i]==0)continue;
    if(ckt[i]->is_marked()||!ckt[i]->isAig())continue; 
    cout<<"Sweeping: "<<ckt[i]->getTypeStr()<<"("<<i<<") removed..."<<endl;
    AIGgate* g = static_cast<AIGgate*>(ckt[i]);
    delFout(g->_fin[0],ckt[i]);
    delFout(g->_fin[1],ckt[i]);
    for(int j=0,out = g->_fout.size();j<out;j++){ delFin(g->_fout[j],ckt[i]); }
    delete ckt[i];
    ckt[i] = 0;
  }
}
void 
CirMgr::markDFS(CirGate* gptr){
  if(gptr->is_marked())return;
  if(gptr->_type==PI_GATE||gptr->_type==CONST_GATE){gptr->mark();return;}
  AIGgate* g = static_cast<AIGgate*>(gptr);
  if(g->_fin[0]!=0)markDFS(g->_fin[0]);
  if(g->_fin[1]!=0)markDFS(g->_fin[1]);
  gptr -> mark();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize(){
  CirGate::REF++;
  for(int i = 0,size=ckt.size();i<size;i++){
    if(ckt[i]==0)continue;
    if(ckt[i]->_type == PO_GATE)opt(ckt[i]);
  }
}
void 
CirMgr::opt(CirGate* gptr){
  if(gptr == 0)return;
  if(gptr->is_marked()||gptr->_type==PI_GATE||gptr->_type==CONST_GATE)return;
  if(gptr -> _type==PO_GATE){
    POgate* g = static_cast<POgate*>(gptr);
    opt(g->_fin[0]); gptr->mark();
  }else { //  AIG
    AIGgate* g = static_cast<AIGgate*>(gptr);
    opt(g->_fin[0]); opt(g->_fin[1]);
    for(int i=0;i<2;i++){                                          //one of fanin is 0
      if(g->_fin[i]->_type==CONST_GATE&&g->_inv[i]==false){
        printOpt(0,gptr);
        for(int i=0;i<2;i++){ delFout(g->_fin[i],gptr);}
        for(int j =0,out=g->_fout.size();j<out;j++){
          addFout(ckt[0],g->_fout[j]);
          replaceFin(g->_fout[j],gptr,ckt[0]);
        }delgate(gptr); return;
      }
    }if( g->_fin[0]==g->_fin[1]){                                  // 2 same fanin
      if((g->_inv[0]==false&&g->_inv[1]==false) || (g->_inv[0]==true&&g->_inv[1]==true)){
        printOpt(g->_fin[0],gptr);
        for(int i=0;i<2;i++){ delFout(g->_fin[i],gptr);}
        if(g->_inv[0]==true){
          for (int i=0,out = g->_fout.size();i<out;i++) changeInv(g->_fout[i],gptr);
        }
        for(int i=0,out = g->_fout.size();i<out;i++){
          addFout(g->_fin[0],g->_fout[i]);
          replaceFin(g->_fout[i],gptr,g->_fin[0]);
        }
        delgate(gptr); return;
      }else {                                //same fanin, but one is inverse, the other is not
        printOpt(0,gptr);
        for(int i=0;i<2;i++){ delFout(g->_fin[i],gptr);}
        for(int i=0,size=g->_fout.size();i<size;i++){
          addFout(ckt[0],g->_fout[i]);
          replaceFin(g->_fout[i],gptr,ckt[0]);
        }delgate(gptr); return;
      }
    }
    for(int i =0;i<2;i++){                                      //one of fanins is 1
      if(g->_fin[i]->_type==CONST_GATE && g->_inv[i]==true){
        int x = 1-i;
        printOpt(g->_fin[x],gptr);
        for(int i=0;i<2;i++){ delFout(g->_fin[i],gptr);}
        if(g->_inv[x]==true){
          for(int j=0,out=g->_fout.size();j<out;j++)changeInv(g->_fout[j],gptr);
        }
        for(int j=0,out=g->_fout.size();j<out;j++){
          addFout(g->_fin[x],g->_fout[j]);
          replaceFin(g->_fout[j],gptr,g->_fin[x]);
        }
        delgate(gptr); return;
      }
    }  
    gptr -> mark();
  }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void 
CirMgr::delgate(CirGate* g){
  int i = g->_id;
  delete ckt[i];
  ckt[i] = 0;
}
void 
CirMgr::replaceFin(CirGate* gptr,CirGate* r,CirGate* n){
  if(gptr==0||r==0||n==0)return;
  assert(gptr->_type==PO_GATE||gptr->isAig());
  if(gptr ->_type == PO_GATE){
    POgate* po = static_cast<POgate*>(gptr);
    po -> _fin[0] = n;
  }else { //AIG
    AIGgate* g = static_cast<AIGgate*>(gptr);
    for(int i=0;i<2;i++){
      if(g->_fin[i]==r) g->_fin[i]=n;
    }
  }
}
void 
CirMgr::changeInv(CirGate* g1,CirGate* g2){
  if(g1==0||g2==0)return;
  assert(g1->_type ==PO_GATE||g1->isAig() );
  if(g1->_type==PO_GATE){
    POgate* po = static_cast<POgate*>(g1);
    if(po->_fin[0]==g2)po->_inv[0]= !(po->_inv[0]);
  }else {
    AIGgate* g = static_cast<AIGgate*>(g1);
    for(int i=0;i<2;i++){
      if(g->_fin[i]==g2){ g->_inv[i] = !(g->_inv[i]); } 
    }
  }
}
void 
CirMgr::printOpt(CirGate*g1=0,CirGate*g2=0){
  assert(g2->isAig());
  cout<<"Simplifying: ";
  if(g1==0)cout<<"0";
  else cout<<g1->_id;
  cout<<" merging ";
  AIGgate* gptr = static_cast<AIGgate*>(g2);
  for(int i=0;i<2;i++){
    if(gptr->_fin[i]==g1&&gptr->_inv[i]==true){cout<<"!";break;}
  }cout<<g2->_id<<"..."<<endl;
}