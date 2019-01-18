/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){M=I=L=O=A=0;}
   ~CirMgr() {
     int size = ckt.size();
     for(int i = 0; i<size ; i++)delete ckt[i];
     ckt.clear();
   } //clear

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
     int size = ckt.size();
     for(int i = 0; i < size; i++){
     if(ckt[i]->id == gid)return ckt[i];
     }return 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);  
   void parse(string& , vector<string>& );
   void trans(vector<string>&, vector<unsigned>&);


   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printN(int&,CirGate*)const; // for recursive call
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   void outputAIG(ostream&,CirGate*)const;

private:
   unsigned M,I,L,O,A;
   vector<CirGate*> ckt;
};

#endif // CIR_MGR_H
