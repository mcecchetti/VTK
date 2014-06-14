
#ifndef __vtkVegaSpecReader_h
#define __vtkVegaSpecReader_h

#include "vtkChartsQtVegaModule.h" // For export macro
#include "vtkAlgorithm.h"
#include "vtkStdString.h"

#define VTK_ASCII 1

class vtkCharArray;

class VTKCHARTSQTVEGA_EXPORT vtkVegaSpecReader  : public vtkAlgorithm
{
public:
  static vtkVegaSpecReader *New();
  vtkTypeMacro(vtkVegaSpecReader, vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify file name of vtk data file to read.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Specify the InputString for use when reading from a character array.
  // Optionally include the length for binary strings. Note that a copy
  // of the string is made and stored. If this causes exceedingly large
  // memory consumption, consider using InputArray instead.
  void SetInputString(const char *in);
  vtkGetStringMacro(InputString);
  void SetInputString(const char *in, int len);
  vtkGetMacro(InputStringLength, int);
  void SetInputString(const vtkStdString& input)
    { this->SetInputString(input.c_str(), static_cast<int>(input.length())); }

  // Description:
  // Specify the vtkCharArray to be used  when reading from a string.
  // If set, this array has precedence over InputString.
  // Use this instead of InputString to avoid the extra memory copy.
  // It should be noted that if the underlying char* is owned by the
  // user ( vtkCharArray::SetArray(array, 1); ) and is deleted before
  // the reader, bad things will happen during a pipeline update.
  virtual void SetInputArray(vtkCharArray*);
  vtkGetObjectMacro(InputArray, vtkCharArray);

  // Description:
  // Enable reading from an InputString or InputArray instead of the default,
  // a file.
  vtkSetMacro(ReadFromInputString,int);
  vtkGetMacro(ReadFromInputString,int);
  vtkBooleanMacro(ReadFromInputString,int);

protected:
  vtkVegaSpecReader();
  ~vtkVegaSpecReader();

  char *FileName;
  int ReadFromInputString;
  char *InputString;
  int InputStringLength;
  vtkCharArray* InputArray;


  virtual int ProcessRequest(vtkInformation *, vtkInformationVector **,
                             vtkInformationVector *)
  { return 1; }


private:
  vtkVegaSpecReader(const vtkVegaSpecReader&);  // Not implemented.
  void operator=(const vtkVegaSpecReader&);  // Not implemented.
};


#endif
