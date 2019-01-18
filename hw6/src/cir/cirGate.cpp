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
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
static bool in(unsigned &x,vector<unsigned> &v){
  int size = v.size();
  for(int i =0; i<size; i++){
    if(x==v[i])return true;
  }return false;
}

void 
CirGate::printGate() const{
  if(type == CONST_GATE)cout<<"CONST";
  else if(type == PI_GATE)cout<<"PI  ";
  else if(type == PO_GATE)cout<<"PO  ";
  else if(type == AIG_GATE)cout<<"AIG ";
  cout<<id;
  for(int i =0;i<2;i++){
    if(_fin[i]==0)break;
    cout<<" ";
    if(_fin[i]->undef())cout<<"*";
    if(_inv[i]==true)cout<<"!";
    cout<<_fin[i]->id;
  }
  if(name!="")cout<<" ("<<name<<")";
  cout<<endl;
}
void
CirGate::reportGate() const
{
  stringstream ss ;
  if(name=="")ss<<getTypeStr()<<"("<<id<<"), line "<<line;
  else ss<<getTypeStr()<<"("<<id<<")\""<<name<<"\", line "<<line;
  string info = ss.str();
  cout<<"=================================================="<<"\n= ";
  cout<<setw(47)<<left<<info; cout<<"=\n";
  cout<<"=================================================="<<endl;
}

void 
CirGate::print() const{
  if     (undef())           cout<<"UNDEF "<<id;
  else if(type == CONST_GATE)cout<<"CONST 0";
  else if(type == PI_GATE)   cout<<"PI "<<id;
  else if(type == PO_GATE)   cout<<"PO "<<id;
  else if(type == AIG_GATE)  cout<<"AIG "<<id;
}

void
CirGate::reportFanin(int level) const{
   assert (level >= 0);
   int layer = 1;
   vector<unsigned> ids;
   print(); cout<<endl;
   fanin(this,layer,level,ids);
}
void 
CirGate::fanin( const CirGate* gptr,int layer ,const int& level,vector<unsigned>& ids)const{
  if(layer>level || (gptr->_fin[0]==0&&gptr->_fin[1]==0) )return;
  for(int i = 0; i<2; i++){
    if(gptr -> _fin[i]==0)break;
    int l = layer;
    while(l>0){ cout<<"  "; l--; }
    if(gptr ->_inv[i])cout<<"!";
    gptr -> _fin[i] -> print(); 
    if(in(gptr->_fin[i]->id,ids)&&(layer<level)&&(gptr->_fin[i]->_fin[0]!=0))cout<<" (*)"<<endl;
    else{ 
      cout<<endl; 
      if(layer < level)ids.push_back(gptr->_fin[i]->id);
      fanin(gptr->_fin[i],layer+1,level,ids); 
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
void 
CirGate::fanout(const CirGate* gptr,int layer,const int& level,vector<unsigned>& ids)const{
  if(layer>level || (gptr->_fout.empty()) )return;
  int size = gptr ->_fout.size();
  for(int i = 0; i<size; i++){
    int l = layer;
    while(l>0){ cout<<"  "; l--; }
    if(out_inv(gptr,gptr->_fout[i]))cout<<"!";
    gptr -> _fout[i] -> print(); 
    if(in(gptr->_fout[i]->id,ids)&&(layer<level)&& !(gptr->_fout[i]->_fout.empty()))cout<<" (*)"<<endl;
    else{ 
      cout<<endl; 
      if(layer < level)ids.push_back(gptr->_fout[i]->id);
      fanout(gptr->_fout[i],layer+1,level,ids); 
    }
  }
}
bool 
CirGate::out_inv(const CirGate* in, const CirGate* out)const{
  for(int i = 0;i<2;i++){
    if(out->_fin[i]==in )return (out->_inv[i]);
  }return false;
}

bool 
CirGate::undef() const{
  if( type == AIG_GATE){
    if( _fin[0]==0 || _fin[1]==0 )return true;
    else return false;
  } else return false;
}

bool 
CirGate::unuse() const{
  if( _fout.empty())return true;
  else return false;
}