//
// Floating point representation of 1,2,3D texture coordinates
//
//  use internal floating point array to represent data
//
#ifndef __vlFloatTCoords_h
#define __vlFloatTCoords_h

#include "TCoords.hh"
#include "FArray.hh"

class vlFloatTCoords : public vlTCoords
{
public:
  vlFloatTCoords() {};
  vlTCoords *MakeObject(int sze, int d=2, int ext=1000);
  int Initialize(const int sz, const int dim=2, const int ext=1000) 
    {return this->TC.Initialize(dim*sz,dim*ext);};
  vlFloatTCoords(const vlFloatTCoords& ftc) 
    {this->TC = ftc.TC;this->Dimension = ftc.Dimension;};
  vlFloatTCoords(const int sz, const int d=2, const int ext=1000):Dimension(d), TC(d*sz,d*ext) {};
  ~vlFloatTCoords() {};
  vlFloatTCoords &operator=(const vlFloatTCoords& ftc);
  char *GetClassName() {return "vlFloatTCoords";};

  void operator+=(const vlFloatTCoords& ftc) {this->TC += ftc.TC;};
  int NumTCoords() {return (this->TC.GetMaxId()+1)/this->Dimension;};
  void Reset() {this->TC.Reset();};

  float *GetTCoord(int i) {return this->TC.GetPtr(this->Dimension*i);};
  void SetTCoord(int i, float *x) 
    {i*=this->Dimension; for(int j=0;j<this->Dimension;j++) this->TC[i+j]=x[j];};
  void InsertTCoord(int i, float *x) {
    for(int j=0; j<this->Dimension; j++) this->TC.InsertValue(3*i+j, x[j]);
  }
  int InsertNextTCoord(float *x) {
    int id = this->TC.InsertNextValue(x[0]);
    for(int j=1; j<this->Dimension; j++) this->TC.InsertNextValue(x[j]);
    return id/this->Dimension;
  }

  vlSetClampMacro(Dimension,int,1,3);
  vlGetMacro(Dimension,int);

private:
  vlFloatArray TC;
  int Dimension;
};

#endif
