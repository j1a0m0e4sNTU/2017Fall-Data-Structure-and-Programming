/****************************************************************************
  FileName     [ p2Table.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include "p2Table.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

// TODO
// Implement member functions of class Row and Table here
///////////////////   Row    ////////////////////////
Row::Row(int r)
{
   _data=new int(r);
   for(int i=0;i<r;i++)_data[i]=0;
   _occupy=new bool(r);
   for(int i=0;i<r;i++)_occupy[i]=false;
}
bool& Row::oc(size_t i){return _occupy[i];}

///////////////////  Table   ////////////////////////
bool Table::read(const string& csvFile)
{
   ifstream file(csvFile);
   if(file.is_open())
   {
      string line="";
      vector<string> vs;
      while(getline(file,line))vs.push_back(line);
      file.close();
      //set _cols
      string s=vs[0];
      size_t n_1=1;
      string::iterator it_1=s.begin();
      while(it_1!=s.end())
      {
         if(*it_1==',')n_1++;
         it_1++;
      }_nCols=n_1;
      //set _rows & data in each cell
      int _nRows=vs.size();
      size_t r=0,c=0;
      string::iterator it_2=vs[r].begin();
      for(int i=0;i<_nRows;i++)add_newRow();
      while(it_2!=vs[_nRows-1].end())
      {
         switch(*it_2)
         {
            case '-':
               _rows[r][c]=2147483647;//
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '1':
               if(_rows[r][c]==2147483647)_rows[r][c]=-1;
               else if(_rows[r][c]<0)_rows[r][c]=_rows[r][c]*10-1;
               else _rows[r][c]=_rows[r][c]*10+1;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '2':
               if(_rows[r][c]==2147483647)_rows[r][c]=-2;
               else if(_rows[r][c]<0)_rows[r][c]=_rows[r][c]*10-2;
               else _rows[r][c]=_rows[r][c]*10+2;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '3':
               if(_rows[r][c]==2147483647)_rows[r][c]=-3;
               else if(_rows[r][c]<0)_rows[r][c]=_rows[r][c]*10-3;
               else _rows[r][c]=_rows[r][c]*10+3;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '4':
               if(_rows[r][c]==2147483647)_rows[r][c]=-4;
               else if(_rows[r][c]<0)_rows[r][c]=_rows[r][c]*10-4;
               else _rows[r][c]=_rows[r][c]*10+4;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '5':
               if(_rows[r][c]==2147483647)_rows[r][c]=-5;
               else if(_rows[r][c]<0)_rows[r][c]=_rows[r][c]*10-5;
               else _rows[r][c]=_rows[r][c]*10+5;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '6':
               if(_rows[r][c]==2147483647)_rows[r][c]=-6;
               else if(_rows[r][c]<0)_rows[r][c]=_rows[r][c]*10-6;
               else _rows[r][c]=_rows[r][c]*10+6;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '7':
               if(_rows[r][c]==2147483647)_rows[r][c]=-7;
               else if(_rows[r][c]<0)_rows[r][c]=_rows[r][c]*10-7;
               else _rows[r][c]=_rows[r][c]*10+7;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '8':
               if(_rows[r][c]==2147483647)_rows[r][c]=-8;
               else if(_rows[r][c]<0)_rows[r][c]=_rows[r][c]*10-8;
               else _rows[r][c]=_rows[r][c]*10+8;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '9':
               if(_rows[r][c]==2147483647)_rows[r][c]=-9;
               else if(_rows[r][c]<0)_rows[r][c]=_rows[r][c]*10-9;
               else _rows[r][c]=_rows[r][c]*10+9;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case '0':
               _rows[r][c]=_rows[r][c]*10;
               _rows[r].oc(c)=true;
               it_2++;
               break;
            case ',':
               c++;
               it_2++;
               break;
            default:
               c=0;
               r++;
               it_2=vs[r].begin();
               break;           
         }
      }return true;
   }else return false;
}

void Table::add_newRow()
{
   Row row(_nCols);
   _rows.push_back(row);
}
void Table::print()//setw(4), right
{
   int _nRows=_rows.size();
   for(int i=0;i<_nRows;i++)
   {
      for(size_t j=0;j<_nCols;j++)
      {
         if(_rows[i].oc(j)==false)cout<<setw(4)<<right<<".";
         else cout<<setw(4)<<right<<_rows[i][j];
      }cout<<endl;
   }
}
//cal func
bool Table::emptyCol(int n)
{
   bool b=true;
   int r=_rows.size();
   for(int i=0;i<r;i++)if(_rows[i].oc(n))b=false;
   return b;
}
void Table::sum(int i)
{  
   if(emptyCol(i))cout<<"Error: This is a NULL column!!"<<endl;
   else{
   int r=_rows.size(),s=0;
   for(int j=0;j<r;j++)s+=_rows[j][i];
   cout<<"The summation of data in column #"<<i<<" is "<<s<<"."<<endl;
   }
}
void Table::ave(int i)//fixed, setprecision(1)
{
   if(emptyCol(i))cout<<"Error: This is a NULL column!!"<<endl;
   else{
   int a=0,r=_rows.size();
   for(size_t j=0;j<_rows.size();j++)if(_rows[j].oc(i)==true)a++;
   double s=0;
   for(int j=0;j<r;j++)s+=_rows[j][i];
   cout<<"The average of data in column #"<<i<<" is "<<fixed<<setprecision(1)<<s/a<<"."<<endl;
   }
}
void Table::max(int i)
{
   if(emptyCol(i))cout<<"Error: This is a NULL column!!"<<endl;
   else{
   int r=_rows.size(),max=_rows[0][i];
   for(int j=1;j<r;j++)if(_rows[j][i]>max&&_rows[j].oc(i)==true)max=_rows[j][i];
   cout<<"The maximum of data in column #"<<i<<" is "<<max<<"."<<endl;
   }
}
void Table::min(int i)
{
   if(emptyCol(i))cout<<"Error: This is a NULL column!!"<<endl;
   else{
   int r=_rows.size(),min=_rows[0][i];
   for(int j=1;j<r;j++)if(_rows[j][i]<min&&_rows[j].oc(i)==true)min=_rows[j][i];
   cout<<"The minimum of data in column #"<<i<<" is "<<min<<"."<<endl;
   }
   
}
void Table::dist(int i)
{
   if(emptyCol(i))cout<<"Error: This is a NULL column!!"<<endl;
   else{
   int r=_rows.size();
   vector<int> v;
   for(int j=0;j<r;j++)if(_rows[j].oc(i)==true)v.push_back(_rows[j][i]);
   int s=v.size();
   if(s==1)cout<<"The distinct count of data in column #"<<i<<" is 1"<<"."<<endl;
   for(int j=1;j<r;j++)for(int k=0;k<j;k++)if(_rows[k][i]==_rows[j][i])s--;
   cout<<"The distinct count of data in column #"<<i<<" is "<<s<<"."<<endl;
   }
}


