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
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
friend class CirMgr;
static int REF;
public:
   CirGate(unsigned n = 0, unsigned l = 0, GateType t = AIG_GATE):id(n),line(l),type(t) {
     _fin[0] = _fin[1] = 0;
     _inv[0] = _inv[1] = false;
     ref = 0 ; 
     name = "";
    }
   ~CirGate() {} //delete

   // Basic access methods
   string getTypeStr() const { 
     if(undef())return "UNDEF";
     if(type == PI_GATE) return "PI";
     else if(type == PO_GATE) return "PO";
     else if(type == AIG_GATE)return "AIG";
     else if(type == CONST_GATE)return "CONST";
     else return "";
     }
   unsigned getLineNo() const { return line; }

   // Printing functions
   void printGate() const ;
   void setName(string s) {name = s;}
   void reportGate() const;
   void reportFanin(int level) const;
   void print()const;
   void fanin( const CirGate* ,int,const int&,vector<unsigned>&)const;
   void reportFanout(int level) const;
   void fanout(const CirGate* ,int ,const int&,vector<unsigned>&)const;
   bool out_inv(const CirGate*,const CirGate*)const;
   
   bool is_marked()const {return (ref == REF);}
   void mark() { ref = REF ;}

   bool undef() const;
   bool unuse() const;
   
private:

protected:
   unsigned id ;
   unsigned line;
   GateType type;
   CirGate* _fin[2];
   bool     _inv[2];
   vector<CirGate*> _fout;

   int ref;
   string name;
};

#endif // CIR_GATE_H