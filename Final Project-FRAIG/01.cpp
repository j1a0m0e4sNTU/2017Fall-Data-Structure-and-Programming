bool
CirMgr::readCircuit(const string& fileName) //performance has room to be improved
{
  ifstream file;
   file.open(fileName);
   if( ! file.is_open()){cout<<"Cannot open design \""<<fileName<<"\"!!"<<endl; return false;}
   string s;                        
   unsigned l = 1, temp;
   int x;
   CirGate* gtemp; CirGate* g[3] = {0,0,0} ;
   vector<string>       vs;
   vector<unsigned>     vi;
   getline(file,s); parse(s,vs); trans(vs,vi);
   M = vi[0]; I = vi[1]; L = vi[2]; O = vi[3]; A = vi[4];
   CirGate* gptr = new PIgate(0,0); gptr -> _type = CONST_GATE;
   ckt.push_back(gptr);
   for( unsigned i=0 ; i<I; i++){
     getline(file,s); l++;
     myStr2Int(s, x);
     temp = (unsigned)x;
     CirGate* pi = new PIgate(temp/2,l);
     ckt.push_back(pi);
   }
   for( unsigned i=0 , num = M +1 ; i<O; i++,num++){
     getline(file,s); l++;
     myStr2Int(s, x);
     temp = (unsigned)x;
     CirGate* po = new POgate(num, l );
     CirGate* gptr = new AIGgate(temp/2, l ); // l should be modified later
     setFin(po,0,gptr); 
     if(temp%2 ==1) setInv(po,0,true); 
     addFout(gptr,po); 
     ckt.push_back(po);
     ckt.push_back(gptr);
   } //PI,PO are done
   while(getline(file,s)){
     l++;
     parse(s,vs);
     if(myStr2Int(vs[0],x)){ // set gates 
       trans(vs, vi);
       for(int i = 0; i < 3; i++){
         gtemp = getGate(vi[i]/2);
         if(gtemp !=0){ g[i] = gtemp; if(i==0){gtemp->_line =l;}}
         else { g[i] = new AIGgate(vi[i]/2,l); ckt.push_back(g[i]); }
       }
       setFin(g[0],0,g[1]);
       if(vi[1]%2==1) setInv(g[0],0,true);
       addFout(g[1],g[0]);
       setFin(g[0],1,g[2]);
       if(vi[2]%2==1) setInv(g[0],1,true);
       addFout(g[2],g[0]);
       g[0]=g[1]=g[2]=0;
     }else{  //not gate declaration
        string::iterator it = s.begin();
        int x;
        int size = ckt.size();
        if(*it == 'c')  break;
        if(*it == 'i'){
          s = s.substr(1);
          parse(s, vs); myStr2Int(vs[0],x);
          for(int i = 0, count = 0; i<size ; i++){
            if( ckt[i]->_type == PI_GATE ){
              if(count == x) ckt[i]->setName(vs[1]);
              else count ++;
            }
          }
        }else if(*it == 'o'){
          s = s.substr(1);
          parse(s, vs); myStr2Int(vs[0],x);
          for(int i = 0, count = 0; i<size ; i++){
            if( ckt[i]->_type == PO_GATE ){
              if(count == x) ckt[i]->setName(vs[1]);
              else count ++;
            }
        }
      }
     }
   }
   int size = ckt.size();
   for(int i=0; i<size; i++){ if(ckt[i]->undef()){ckt[i]->_line = 0;ckt[i]->_type=UNDEF_GATE;} }
   return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool
CirMgr::readCircuit(const string& fileName) //performance has room to be improved
{
  ifstream file;
   file.open(fileName);
   if( ! file.is_open()){cout<<"Cannot open design \""<<fileName<<"\"!!"<<endl; return false;}
   string s;                        
   unsigned l = 1, temp;
   int x;
   CirGate* gtemp; CirGate* g[3] = {0,0,0} ;
   vector<string>       vs;
   vector<unsigned>     vi;
   vector<unsigned>     vo;
   getline(file,s); parse(s,vs); trans(vs,vi);
   M = vi[0]; I = vi[1]; L = vi[2]; O = vi[3]; A = vi[4];
   CirGate* gptr = new PIgate(0,0); gptr -> _type = CONST_GATE;
   ckt.push_back(gptr);
   for( unsigned i=0 ; i<I; i++){
     getline(file,s); l++;
     myStr2Int(s, x);
     temp = (unsigned)x;
     CirGate* pi = new PIgate(temp/2,l);
     ckt.push_back(pi);
   }
   for( unsigned i=0 , num = M +1 ; i<O; i++,num++){
     getline(file,s); l++;
     myStr2Int(s, x); temp = (unsigned)x; vo.push_back(temp);
     CirGate* po = new POgate(num, l );  
     ckt.push_back(po);
   } //PI,PO are done
   while(getline(file,s)){
     l++;
     parse(s,vs);
     if(myStr2Int(vs[0],x)){ // set gates 
       trans(vs, vi);
       for(int i = 0; i < 3; i++){
         gtemp = getGate(vi[i]/2);
         if(gtemp !=0){ g[i] = gtemp; }
         else { g[i] = new AIGgate(vi[i]/2,l); ckt.push_back(g[i]); }
       }
       setFin(g[0],0,g[1]);
       if(vi[1]%2==1) setInv(g[0],0,true);
       addFout(g[1],g[0]);
       setFin(g[0],1,g[2]);
       if(vi[2]%2==1) setInv(g[0],1,true);
       addFout(g[2],g[0]);
       g[0]=g[1]=g[2]=0;
     }else{  //not gate declaration
        string::iterator it = s.begin();
        int x;
        int size = ckt.size();
        if(*it == 'c')  break;
        if(*it == 'i'){
          s = s.substr(1);
          parse(s, vs); myStr2Int(vs[0],x);
          for(int i = 0, count = 0; i<size ; i++){
            if( ckt[i]->_type == PI_GATE ){
              if(count == x) ckt[i]->setName(vs[1]);
              else count ++;
            }
          }
        }else if(*it == 'o'){
          s = s.substr(1);
          parse(s, vs); myStr2Int(vs[0],x);
          for(int i = 0, count = 0; i<size ; i++){
            if( ckt[i]->_type == PO_GATE ){
              if(count == x) ckt[i]->setName(vs[1]);
              else count ++;
            }
        }
      }
     }
   }
   int size = ckt.size();
   for(unsigned i = I+1,j=0; i<I+1+O;i++,j++){
     gtemp = getGate(vo[j]/2);
     setFin(ckt[i],0,gtemp);
     if(vo[j]%2 ==1) setInv(ckt[i],0,true);
     addFout(gtemp,ckt[i]);
   }
   for(int i=0; i<size; i++){ if(ckt[i]->undef()){ckt[i]->_line = 0;ckt[i]->_type=UNDEF_GATE;} }
   return true;
}
