
#ifndef __sflight_fdm_Table2D_H__
#define __sflight_fdm_Table2D_H__

#include <string>

namespace sflight {
namespace fdm {

//------------------------------------------------------------------------------
// Class: Table2D
//------------------------------------------------------------------------------
class Table2D {
 public:
   Table2D() = default;
   Table2D(const int numRows, const int numCols, double rowVals[],
           double colVals[]);
   virtual ~Table2D();

   int getNumCols();
   int getNumRows();
   void multiply(double val);
   void setRowData(int row, double rowdata[]);
   void setData(std::string tabledata);

   double** getData();
   double* getColVals();
   double* getRowVals();

   void set(int row, int col, double val);
   double get(int row, int col);
   double interp(double rowVal, double colVal);
   void print();

 private:
   double** data{};

   int numRows{};
   int numCols{};

   double* rowVals{};
   double* colVals{};
};
}
}

#endif