/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <cmath>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

static bool in(unsigned &x,vector<unsigned> &v){
  int size = v.size();
  for(int i =0; i<size; i++){
    if(x==v[i])return true;
  }return false;
}
/**************************************/
/*   class CirGate member functions   */
/**************************************/
void 
CirGate::print() const{
  if     (undef())            cout<<"UNDEF "<<_id;
  else if(_type == CONST_GATE)cout<<"CONST 0";
  else if(_type == PI_GATE)   cout<<"PI "<<_id;
  else if(_type == PO_GATE)   cout<<"PO "<<_id;
  else if(_type == AIG_GATE)  cout<<"AIG "<<_id;
}
void
CirGate::printValue()const{
  int v[64] = {0};
  size_t temp = _value;
  for(int i = 63;i>=0;i--){
    v[i] = temp % 2;
    temp = temp / 2; 
  }
  for(int  i =0;i<64;i++){
    if(i!=0&&i%8==0)cout<<"_";
    cout<<v[i];
  }cout<<endl;
}
void 
CirGate::reportGate() const{
  stringstream ss ;
  if( _type == PI_GATE){
    const PIgate* gptr = static_cast<const PIgate*>(this);
    if(gptr -> _name != "")ss<<getTypeStr()<<"("<<gptr->_id<<")\""<<gptr->_name<<"\", line "<<gptr->_line;
    else ss<<getTypeStr()<<"("<<_id<<"), line "<<_line;
  }else if( _type == PO_GATE ){
    const POgate* gptr = static_cast<const POgate*>(this);
    if(gptr -> _name != "")ss<<getTypeStr()<<"("<<gptr->_id<<")\""<<gptr->_name<<"\", line "<<gptr->_line;
    else ss<<getTypeStr()<<"("<<_id<<"), line "<<_line;
  }else {
    if(undef())ss<<getTypeStr()<<"("<<_id<<"), line 0";
    else ss<<getTypeStr()<<"("<<_id<<"), line "<<_line;
  }
  string info = ss.str();
  for(int i=0;i<80;i++)cout<<"="; cout<<endl;
  cout<<"= "<<info<<endl;
  cout<<"= FECs: ";  cirMgr ->getGroup(groupNum()).report(_id);
  cout<<"= Value: "; printValue();
  for(int i=0;i<80;i++)cout<<"="; cout<<endl;
}
void
CirGate::reportFanin(int level) const{
   assert (level >= 0);
   int layer = 1;
   vector<unsigned> ids;
   print(); cout<<endl;
   fanin(this,layer,level,ids);
}
bool 
CirGate::nofanin(const CirGate* g = 0) const{
  if(g == 0)g=this;
  if(g -> _type == PI_GATE || g-> _type == CONST_GATE)return true;
  else if(g->isAig()){
    const AIGgate* gptr = static_cast<const AIGgate*>(g);
    if(gptr -> _fin[0]==0 && gptr ->_fin[1]==0)return true;
  }else if(g->_type==PO_GATE){
    const POgate* gptr = static_cast<const POgate*>(g);
    if(gptr -> _fin[0]==0 )return true;
  }return false;
}

void 
CirGate::fanin( const CirGate* g,int layer ,const int& level,vector<unsigned>& ids)const{
  if(layer>level|| g ->_type == PI_GATE|| g->_type == CONST_GATE)return;
  if(g -> isAig()){
    const AIGgate* gptr = static_cast< const AIGgate*>(g);
    if(gptr->_fin[0]==0&&gptr->_fin[1]==0)return;
    for(int i = 0; i<2; i++){
      if(gptr -> _fin[i]==0)break;
      int l = layer;           
      while(l>0){ cout<<"  "; l--; }
      if(gptr ->_inv[i])cout<<"!";
      gptr -> _fin[i] -> print(); 
      if(in(gptr->_fin[i]->_id,ids)&&(layer<level)&&(!nofanin(gptr->_fin[i])))cout<<" (*)"<<endl;
      else{ 
        cout<<endl; 
        if(layer < level)ids.push_back(gptr->_fin[i]->_id);
        fanin(gptr->_fin[i],layer+1,level,ids); 
      }
    }
  }else { //  PO_GATE
    const POgate* gptr = static_cast< const POgate*>(g);
    if(gptr->_fin[0]==0)return;
    int l = layer;
    while(l>0){ cout<<"  "; l--; }
    if(gptr ->_inv[0])cout<<"!";
    gptr -> _fin[0] -> print(); 
    if(in(gptr->_fin[0]->_id,ids)&&(layer<level)&&(!nofanin(gptr->_fin[0])))cout<<" (*)"<<endl;
    else{ 
      cout<<endl; 
      if(layer < level)ids.push_back(gptr->_fin[0]->_id);
      fanin(gptr->_fin[0],layer+1,level,ids); 
    }
  }
}

void
CirGate::reportFanout(int level) const{
   assert (level >= 0);
   int layer = 1;
   vector<unsigned> ids;
   print(); cout<<endl;
   fanout(this, layer, level,ids);
}
bool 
CirGate::nofanout(const CirGate* g = 0)const{
  if (g == 0) g =this;
  if(g -> _type == PO_GATE)return true;
  else if( g-> isAig()){
    const AIGgate* gptr = static_cast<const AIGgate*>(g);
    return (gptr->_fout.empty());
  }else if( g-> _type == PI_GATE){
    const PIgate* gptr = static_cast<const PIgate*>(g);
    return (gptr->_fout.empty());
  } return false;
}

void 
CirGate::fanout(const CirGate* g ,int layer,const int& level,vector<unsigned>& ids)const{
  if(layer>level || g->_type == PO_GATE )return;
  if( g -> isAig()){
    const AIGgate* gptr = static_cast< const AIGgate*>(g);
    if(gptr->_fout.empty())return;
    for(int i = 0,size = gptr ->_fout.size(); i<size; i++){
      int l = layer;
      while(l>0){ cout<<"  "; l--; }
      if(out_inv(gptr,gptr->_fout[i]))cout<<"!";
      gptr -> _fout[i] -> print(); 
      if(in(gptr->_fout[i]->_id,ids)&&(layer<level)&& !nofanout(gptr->_fout[i]))cout<<" (*)"<<endl;
      else{ 
        cout<<endl; 
        if(layer < level)ids.push_back(gptr->_fout[i]->_id);
        fanout(gptr->_fout[i],layer+1,level,ids); 
      }
    }
  }else{ //PI_GATE  CONST 0
    const PIgate* gptr = static_cast< const PIgate*>(g);
    if(gptr->_fout.empty())return;
    for(int i = 0,size = gptr ->_fout.size(); i<size; i++){
      int l = layer;
      while(l>0){ cout<<"  "; l--; }
      if(out_inv(gptr,gptr->_fout[i]))cout<<"!";
      gptr -> _fout[i] -> print(); 
      if(in(gptr->_fout[i]->_id,ids)&&(layer<level)&& !nofanout(gptr->_fout[i]))cout<<" (*)"<<endl;
      else{ 
        cout<<endl; 
        if(layer < level)ids.push_back(gptr->_fout[i]->_id);
        fanout(gptr->_fout[i],layer+1,level,ids); 
      }
    }
  }
}
bool 
CirGate::out_inv(const CirGate* in, const CirGate* out)const{
  if( out -> _type == AIG_GATE){
    const AIGgate* gptr = static_cast<const AIGgate*>(out);
    for(int i =0;i<2;i++)if(gptr->_fin[i] == in)return gptr -> _inv[i];
  }else if( out -> _type == PO_GATE){
    const POgate* gptr = static_cast<const POgate*>(out);
    if(gptr->_fin[0] == in)return gptr -> _inv[0];
  } return false;
}

bool 
CirGate::undef() const{
  if( isAig()){
    const AIGgate* gptr = static_cast< const AIGgate*>(this);
    if( gptr -> _fin[0]==0 || gptr -> _fin[1]==0 )return true;
  } return false;
}

bool 
CirGate::unuse() const{
  if( _type == PI_GATE){
    const PIgate* gptr = static_cast< const PIgate*>(this);
    if( gptr -> _fout.empty())return true;
  }else if( _type == AIG_GATE){
    const AIGgate* gptr = static_cast<const AIGgate*>(this);
    if( gptr -> _fout.empty())return true;
  }return false;
}

//========================= PI ===============================================
void 
PIgate::printGate() const{
  const PIgate* gptr = static_cast<const PIgate*>(this);
  if(gptr->_type==PI_GATE)cout<<"PI  ";else cout<<"CONST";
  cout<<gptr->_id;
  if(gptr->_name!="")cout<<" ("<<gptr->_name<<")";
  cout<<endl;
}
//========================= AIG ==============================================
void 
AIGgate::printGate() const{
  const AIGgate* gptr = static_cast<const AIGgate*>(this);
  cout<<"AIG "<<gptr->_id;
  for(int i =0;i<2;i++){
    if(gptr->_fin[i]==0)break;
    cout<<" ";
    if(gptr->_fin[i]->undef())cout<<"*";
    if(gptr->_inv[i]==true)cout<<"!";
    cout<<gptr->_fin[i]->_id;
  }cout<<endl;
}
//========================= PO ===============================================
void 
POgate::printGate() const{
  const POgate* gptr = static_cast<const POgate*>(this);
  cout<<"PO  "<<_id;
  if(gptr->_fin[0]==0)return;
  cout<<" ";
  if(gptr->_fin[0]->undef())cout<<"*";
  if(gptr->_inv[0]==true)cout<<"!";
  cout<<gptr->_fin[0]->_id;
  if(gptr->_name!="")cout<<" ("<<gptr->_name<<")";
  cout<<endl;
}