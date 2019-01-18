/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
  friend class CirMgr;
  friend class PIgate;friend class AIGgate;friend class POgate;

  static int REF;
public:
   CirGate(unsigned n = 0, unsigned l = 0):_id(n),_line(l),_value(0){ ref=0; _fec = 0; }
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return _line; }
   virtual bool isAig() const {return false;}
   unsigned getID()const {return _id;}

   // Printing functions
   virtual void printGate() const {}
   void reportGate() const ;
   void reportFanin(int level) const ;
   bool nofanin(const CirGate*) const;
   void fanin( const CirGate* ,int,const int&,vector<unsigned>&)const;
   void reportFanout(int level) const ;
   bool nofanout(const CirGate*)const;
   void fanout(const CirGate* ,int ,const int&,vector<unsigned>&)const;
   bool out_inv(const CirGate*,const CirGate*)const;

   
   void print()const;
   void printValue()const;
   bool undef() const ;
   bool unuse() const ;
   
   virtual void setName(string s){}
   bool is_marked()const {return (ref == REF);}
   void mark() { ref = REF ;}

   Var   getVar()const {return _var;}
   void  setVar(const Var& v) { _var = v;}

   int groupNum()const {return (abs(_fec)-1); }
   bool groupInv()const { if(_fec <0)return true; else return false ; }
   
private:

protected:
  unsigned _id;
  unsigned _line;
  size_t   _value;
  GateType _type;
  int      ref;
  Var      _var;
  int      _fec;
};
//==================================================================================
class PIgate:public CirGate{
  friend class CirMgr; friend class CirGate;
public:
  PIgate(unsigned n = 0, unsigned l = 0):CirGate(n ,l){ _type = PI_GATE;_name="";} 
  virtual string getTypeStr() const { if(_type==PI_GATE)return "PI";else return "CONST"; }
  virtual void printGate() const ;
  virtual void setName(string s){ _name = s; }
private:
  vector<CirGate*> _fout;
  string  _name;
};
//=================================================================================
class AIGgate:public CirGate{
  friend class CirMgr; friend class CirGate;
public:
  AIGgate(unsigned n = 0, unsigned l = 0):CirGate(n ,l){
    _type = AIG_GATE;
    _fin[0]=_fin[1]=0;
    _inv[0]=_inv[1]=false;
    }
  virtual string getTypeStr() const {if(undef()) return "UNDEF";else return "AIG"; }
  virtual void printGate() const ;
  virtual bool isAig() const {return true;}
private:
  CirGate* _fin[2];
  bool     _inv[2];
  vector<CirGate*> _fout;
};
//=================================================================================
class POgate:public CirGate{
  friend class CirMgr; friend class CirGate;
public:
  POgate(unsigned n = 0, unsigned l = 0):CirGate(n,l){
    _type = PO_GATE; _name="";
    _fin[0]=_fin[1]=0;
    _inv[0]=_inv[1]=false;
  }
  virtual string getTypeStr() const { return "PO"; }
  virtual void printGate() const ;
  virtual void setName(string s){ _name = s; }

private:
  CirGate* _fin[2];
  bool     _inv[2];
  string  _name;
};

#endif // CIR_GATE_H
