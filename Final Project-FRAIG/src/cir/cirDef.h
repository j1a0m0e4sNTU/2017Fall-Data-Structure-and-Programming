/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include <cmath>
#include <algorithm>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum

class CirGate;
class CirMgr;
class SatSolver;

enum GateType
{
   UNDEF_GATE = 0,
   PI_GATE    = 1,
   PO_GATE    = 2,
   AIG_GATE   = 3,
   CONST_GATE = 4,
};

//************************** FEC classes ***********************

class FECgroup{
friend class FECtable;
public:
FECgroup(){}
FECgroup(const FECgroup& g){group = g.group;}
int size()const{ return group.size();}
bool empty() const{ return group.empty();}
void clear() { group.clear();}
int& operator [](int i) { return group[i]; }
const int& operator [](int i)const{ return group[i];} 
void operator = (const FECgroup& g){group = g.group;}
bool operator == (const FECgroup& g)const{
  int size = group.size();
  if(size != g.size())return false;
  for(int i=0;i<size;i++){
    if(group[i] != g[i])return false;
  }return true;
}
vector<int> getGroup()const{ return group;} 
void add(bool inv = false, int i =0){ 
  if(inv)group.push_back(i*(-1));
  else   group.push_back(i); 
  }
bool check(int x){
  for(int i=0,size = group.size();i<size;i++ ){
    if(abs(group[i])==x)return true;
  }return false;
}
void remove(int x){
  for(int i =0, size = group.size();i<size;i++){
    if(abs(group[i]) == x) { group.erase(group.begin()+i); break;}
  }
}
void erase(int i) {group.erase(group.begin()+i);}
void sort(){
  int temp ;
  for(int i =1,size=group.size();i<size;i++){
    for(int j =i-1;j>=0;j--){
      if(abs(group[j])<=abs(group[j+1]))break;
      temp = group[j];group[j]=group[j+1];group[j+1]=temp;
    }
  }
  if(group[0]<0){
    for(int i =0,size=group.size();i<size;i++)group[i] = -(group[i]);  
  }
}
void print()const{
  for(int i=0,size=group.size();i<size;i++){
    if(group[i]<0)cout<<" !"<< -(group[i]) ;
    else          cout<<" "<< group[i] ;
  }cout<<endl;
}
void report(unsigned i)const{
  int id = i;
  FECgroup temp = (*this);
  temp.sort();  bool inv = false;
  for(int i = 0,G=temp.size();i<G;i++){
    if(temp[i]==(-id)&&id!=0)inv = true;
  }if(inv){for(int i = 0,G=temp.size();i<G;i++)temp[i]*=(-1);}
  for(int i = 0,G=temp.size();i<G;i++){
    if(temp[i] == id)continue;
    if(temp[i]<0)cout<<"!"; cout<<abs(temp[i])<<" ";
  }cout<<endl;
}
private:
vector<int> group;
};

class FECtable{
friend class CirMgr;
public:
FECtable():sim(false){}
int size()const{return table.size();}
FECgroup& operator [] (int i){ return (table[i]); }
const FECgroup& operator [] (int i)const{ return (table[i]);}
void operator = (const FECtable& t){
  table.clear();
  for(int i =0, T=t.size();i<T; i++){ table.push_back(t[i]); }
}
bool empty()const{return (table.empty()); }
void clear(){ table.clear(); table.resize(0); sim=false;}
void add(FECgroup& group){
  table.push_back(group);
}
void remove(int s = 0,int e = 0){ 
  table.erase(table.begin()+s,table.begin()+e);
}
void sort(){
  FECgroup temp;
  for(int i = 0,size = table.size();i<size;i++)table[i].sort();
  for(int i = 1,size = table.size();i<size;i++){
    for(int j=i-1;j>=0;j--){
      if(table[j][0]<=table[j+1][0])break;
      temp = table[j];table[j]=table[j+1];table[j+1]=temp;
    }
  }
}
void print(bool b = false)const{
  if(b == false){
    FECtable temp = (*this);
    temp.print(true); return;  
  }
  const_cast<FECtable*>(this)->sort();
  for(int i=0,size=table.size();i<size;i++){
    cout<<"["<<i<<"]";
    table[i].print();
  }
}

private:
vector<FECgroup> table;
bool sim;
};

#endif // CIR_DEF_H
