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
#include "cirGate.h"
#include "myHashMap.h"
using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class FECtable;

class CirMgr
{
public:
   CirMgr(){M=I=L=O=A=0;}
   ~CirMgr() {
     vector<CirGate*>::iterator it = ckt.begin();
     for(; it!=ckt.end() ; it++)if(*it!=0){delete *it; *it = 0;}
   } 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
     if(gid>=M+O+1)return 0;
     else return ckt[gid];
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   void parse(string& , vector<string>& );
   void trans(vector<string>&, vector<unsigned>&);
   void setFin(CirGate*&,int ,CirGate*&);
   void setInv(CirGate*&,int,bool );
   void addFout(CirGate*&,CirGate*&);
   void delFout(CirGate*&,CirGate*&);
   void delFin(CirGate*&,CirGate*&);

   // Member functions about circuit optimization
   void sweep();
   void markDFS(CirGate*);
   void optimize();
   void opt(CirGate*);
   

   // Member functions about simulation
   void sim(CirGate*);
   void simOnce(int);
   void randomSim();
   void fileSim(ifstream&);
   void simPattern(size_t*);
   void setSimLog(ofstream *) ;

   // Member functions about fraig
   void strash();
   void s(HashMap<HashKey,CirGate*>& ,CirGate* );
   
   void fraig();
   void buildModel(SatSolver& );
   void conclude(CirGate*, CirGate* , bool, bool ,
                 SatSolver&,size_t*,int&, bool&);


   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printN(int&,CirGate*)const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   
   // member functions about FEC 
   void init(FECgroup&,CirGate*);
   void initFEC();
   void updateFEC();
   void printFEC() const;
   void set_fec();
   const FECgroup& getGroup(int g)const{ return fectable[g];}

private:
   void delgate(CirGate*);
   void replaceFin(CirGate*,CirGate*,CirGate*);
   void changeInv(CirGate*,CirGate*);
   void printOpt(CirGate*,CirGate*);
   void merge(CirGate*,CirGate*,bool );
   void buildDFS();
   void build(CirGate*);

   ofstream           *_simLog;
   unsigned M,I,L,O,A;
   unsigned cktSize;
   vector<CirGate*> ckt;
   vector<PIgate*> pi;
   vector<POgate*> po;
   vector<CirGate*> dfs;
   
   
   FECtable fectable;
};



#endif // CIR_MGR_H
