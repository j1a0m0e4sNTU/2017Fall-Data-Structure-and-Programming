/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include <vector>
#include "dbTable.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/

ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for(size_t i=0;i<r.size();i++){
     if(r[i]==INT_MAX)os<<".";
     else os<<r[i];
     if( i < r.size()-1 )os<<" ";
   }
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   for(size_t i=0;i<t.nRows();i++){
     for(size_t j=0;j<t.nCols();j++){
       if(t[i][j]==INT_MAX)os<<setw(6)<<right<<".";
       else os<<setw(6)<<right<<t[i][j];
     }os<<endl;
   }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume the input file is with correct csv file format
   // - NO NEED to handle error file format
   t.reset();
   string n_s="";
   vector<int>    elements;
   ifs.seekg(0,ios::end);
   int size=ifs.tellg();
   ifs.seekg(0,ios::beg);
   char* c=new char[size];
   ifs.read(c,size);
   int col=1,temp;
   for(int i=0;c[i]!=13;i++){if(c[i]==','){col++;}}
   for(int i=0;i<size;i++){
     if(c[i]!=','&&c[i]!=13&&c[i]!=10){
       n_s+=c[i];
     }else if(c[i]==','||c[i]==13){
       if(n_s==""){elements.push_back(INT_MAX);}
       else{
        myStr2Int(n_s,temp);
        n_s="";
        elements.push_back(temp);
       }
     }if(i==size-1){
       if(c[i]==','){elements.push_back(INT_MAX);}
       else {
        myStr2Int(n_s,temp);
        elements.push_back(temp);
       }
     }
     if((int)elements.size()==col){
       DBRow* newR=new DBRow(elements);
       t.addRow(*newR);
       delete newR;
       while(elements.size()!=0){elements.pop_back();}
     }
   }
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
   for(size_t i=c;i<_data.size()-1;i++){_data[i]=_data[i+1];}
   _data.pop_back();
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for(size_t i=0;i<_sortOrder.size();i++){
     if(r1[_sortOrder[i]]==r2[_sortOrder[i]])continue;
     else return (r1[_sortOrder[i]]<r2[_sortOrder[i]]);
   }return true;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   size_t n=nRows();
   for(size_t i=0;i<n;i++)_table.pop_back();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   for(size_t i=0;i<_table.size();i++){
     if(i>=d.size())_table[i].addData(INT_MAX);
     else _table[i].addData(d[i]);
   }
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   vector<DBRow> temp;
   size_t size=_table.size(),delc=c;
   if(delc <size){
     for(size_t i = delc+1;i< size;i++){temp.push_back(_table[i]);}
     for(size_t i = delc  ;i< size;i++){_table.pop_back();} 
     size=temp.size(); 
     for(size_t i = 0   ;i< size;i++)   {_table.push_back(temp[i]);}
   }
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   if(c<0||c>=nCols()){return NAN;}
   vector<int> temp;
   for(size_t i=0;i<_table.size();i++){
     if(_table[i][c]!=INT_MAX){temp.push_back(_table[i][c]);}
   }
   if(temp.size()==0){return NAN;}
   else{
     int max=temp[0];
     for(size_t i=0;i<temp.size();i++){if(temp[i]>max){max=temp[i];}}
     return (float) max;
   }
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   if(c<0||c>=nCols()){return NAN;}
   vector<int> temp;
   for(size_t i=0;i<_table.size();i++){
     if(_table[i][c]!=INT_MAX){temp.push_back(_table[i][c]);}
   }
   if(temp.size()==0){return NAN;}
   else{
     int min=temp[0];
     for(size_t i=0;i<temp.size();i++){if(temp[i]<min){min=temp[i];}}
     return (float) min;
   }
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   if(c<0||c>=nCols()){return NAN;}
   vector<int> temp;
   for(size_t i=0;i<_table.size();i++){
     if(_table[i][c]!=INT_MAX){temp.push_back(_table[i][c]);}
   }
   if(temp.size()==0){return NAN;}
   else{
     int sum=0;
     for(size_t i=0;i<temp.size();i++){sum+=temp[i];}
     return (float) sum;
   }
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   if(c<0||c>=nCols()){return 0;}
   vector<int> temp;
   for(size_t i=0;i<_table.size();i++){
     if(_table[i][c]!=INT_MAX){temp.push_back(_table[i][c]);}
   }
   size_t size=temp.size();
   if(size==0){return 0;}
   else if(size==1){return 1;}
   else{
     for(size_t i=0;i<size-1;i++){
       for(size_t j=i+1;j<size;j++){
         if(temp[i]==temp[j]){temp[i]=INT_MAX;}
       }
     }
    }
     int count=0;
     for(size_t i=0;i<size;i++){
       if(temp[i]!=INT_MAX){count++;}
     }
     return count;
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   if(c<0||c>=nCols()){return NAN;}
   vector<int> temp;
   for(size_t i=0;i<_table.size();i++){
     if(_table[i][c]!=INT_MAX){temp.push_back(_table[i][c]);}
   }
   if(temp.size()==0){return NAN;}
   else{
     return (float)(getSum(c)/(temp.size()));
   }
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
   ::sort(&_table[0],&_table[nRows()],s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   if(c>=0&&c<nCols()){
     for(size_t i=0;i<_table.size();i++){
       if(_table[i][c]==INT_MAX){cout<<"."<<flush;}
       else { cout<<_table[i][c]<<flush;}
       if(i!=_table.size()-1){cout<<" "<<flush;}
     }
   }

}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

