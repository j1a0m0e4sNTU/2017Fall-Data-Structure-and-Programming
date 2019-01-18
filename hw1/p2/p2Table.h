/****************************************************************************
  FileName     [ p2Table.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
#include <iomanip>

using namespace std;

class Row
{
public:
   // TODO: define constructor and member functions on your own
   Row(int );
   bool& oc(size_t);
   // Basic access functions
   const int operator[] (size_t i) const { return _data[i]; }
   int& operator[] (size_t i) { return _data[i]; }

private:
   int*  _data; // DO NOT change this definition. Use it to store data.
   bool*  _occupy;
};

class Table
{
public:
   // TODO: define constructor and member functions on your own
   
   Table(){ _nCols=0; }
   bool read(const string&);
   void add_newRow();

   bool emptyCol(int);
   void print();//setw(4), right
   void sum(int);
   void ave(int);//fixed, setprecision
   void max(int);
   void min(int);
   void dist(int);
   size_t get_nCols(){return _nCols;};
   int get_nRows(){return _rows.size();};
   // Basic access functions
   size_t nCols() const { return _nCols; }
   const Row& operator[] (size_t i) const { return _rows[i]; }
   Row& operator[] (size_t i) { return _rows[i]; }

private:
   size_t       _nCols; // You should record the number of columns.
   vector<Row>  _rows;  // DO NOT change this definition. Use it to store rows.
};

#endif // P2_TABLE_H
