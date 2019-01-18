/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include "p2Table.h"

using namespace std;

int main()
{
   Table table;

   // Read in the csv file. Do NOT change this part of code.
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << csvFile << "\"!" << endl;
      exit(-1); // csvFile does not exist.
   }
   //test
   
   // TODO read and execute commands
   while (true) {
      string com;
      cin>>com;    
      if(com=="EXIT")break;
      else if(com=="PRINT")table.print();
      else if(com=="ADD"){
            table.add_newRow();
            string s;
            int i=0,r=table.get_nRows()-1;
            for(int i=0;i<1;i++)getline(cin,s); 
            string::iterator it=s.begin();
            it++;
            while(it!=s.end()){
                switch(*it){
                   case '-':
                      table[r][i]=2147483647;
                      break;
                   case '0':
                      table[r][i]=table[r][i]*10;
                      table[r].oc(i)=true;
                      break;
                   case '1':
                      if(table[r][i]==2147483647)table[r][i]=-1;
                      else if(table[r][i]<0)table[r][i]=table[r][i]*10-1;
                      else table[r][i]=table[r][i]*10+1;
                      table[r].oc(i)=true;
                      break;
                   case '2':
                      if(table[r][i]==2147483647)table[r][i]=-2;
                      else if(table[r][i]<0)table[r][i]=table[r][i]*10-2;
                      else table[r][i]=table[r][i]*10+2;
                      table[r].oc(i)=true;
                      break;
                   case '3':
                      if(table[r][i]==2147483647)table[r][i]=-3;
                      else if(table[r][i]<0)table[r][i]=table[r][i]*10-3;
                      else table[r][i]=table[r][i]*10+3;
                      table[r].oc(i)=true;
                      break;
                   case '4':
                      if(table[r][i]==2147483647)table[r][i]=-4;
                      else if(table[r][i]<0)table[r][i]=table[r][i]*10-4;
                      else table[r][i]=table[r][i]*10+4;
                      table[r].oc(i)=true;
                      break;
                   case '5':
                      if(table[r][i]==2147483647)table[r][i]=-5;
                      else if(table[r][i]<0)table[r][i]=table[r][i]*10-5;
                      else table[r][i]=table[r][i]*10+5;
                      table[r].oc(i)=true;
                      break;
                   case '6':
                      if(table[r][i]==2147483647)table[r][i]=-6;
                      else if(table[r][i]<0)table[r][i]=table[r][i]*10-6;
                      else table[r][i]=table[r][i]*10+6;
                      table[r].oc(i)=true;
                      break;
                   case '7':
                     if(table[r][i]==2147483647)table[r][i]=-7;
                      else if(table[r][i]<0)table[r][i]=table[r][i]*10-7;
                      else table[r][i]=table[r][i]*10+7;
                      table[r].oc(i)=true;
                      break;
                   case '8':
                      if(table[r][i]==2147483647)table[r][i]=-8;
                      else if(table[r][i]<0)table[r][i]=table[r][i]*10-8;
                      else table[r][i]=table[r][i]*10+8;
                      table[r].oc(i)=true;
                      break;
                   case '9':
                      if(table[r][i]==2147483647)table[r][i]=-9;
                      else if(table[r][i]<0)table[r][i]=table[r][i]*10-9;
                      else table[r][i]=table[r][i]*10+9;
                      table[r].oc(i)=true;
                      break;
                   case ' ':
                      i++;
                      break;
                   default:
                      break;
                }it++;
            }
      } 
      else{
         size_t i;
         cin>>i;
         if(i<0||i>=table.get_nCols()){
            cout<<"ERROR!"<<endl;
            continue;
            }
         if(com=="SUM")table.sum(i);
         else if(com=="AVE")table.ave(i);
         else if(com=="MAX")table.max(i);
         else if(com=="MIN")table.min(i);
         else if(com=="DIST")table.dist(i);
      }
   }
   return 0;
}
