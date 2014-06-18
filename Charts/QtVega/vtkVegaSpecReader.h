/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaSpecReader.h

  Copyright (c) Marco Cecchetti
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaSpecReader - Read a Vega spec file and generate the related
// scene graph.
// .SECTION Description
// vtkVegaSpecReader is an object that reads from a data source, such as a file
// or a string, a Vega spec and is able to generate the related Vega internal
// scene graph in JSON format. It is possible to get the generated scene graph
// as a string.

#ifndef __vtkVegaSpecReader_h
#define __vtkVegaSpecReader_h

#include "vtkChartsQtVegaModule.h" // For export macro
#include "vtkObject.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"




class vtkCharArray;
class vtkVegaScene;

class VTKCHARTSQTVEGA_EXPORT vtkVegaSpecReader  : public vtkObject
{
public:
  static vtkVegaSpecReader *New();
  vtkTypeMacro(vtkVegaSpecReader, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify file name of vtk data file to read.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Specify the url utilized as base path for finding data resources,
  // referenced by the Vega spec file.
  vtkSetStringMacro(BaseURL);
  vtkGetStringMacro(BaseURL);


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

  // Description:
  // Open a Vega spec data stream. Returns zero if error.
  int OpenSpecFile();

  // Description:
  // Close the Vega spec data stream.
  void CloseSpecFile();

  // Description:
  // In case a Vega spec file or string is available and generate the related
  // scene graph.
  virtual void Update();

  // Description:
  // Return the scene graph, related to the last rendering, in JSON format
  // as a string. If Update has never been invoked return a null string.
  std::string GetSceneString() const;


protected:
  vtkVegaSpecReader();
  ~vtkVegaSpecReader();


  char *FileName;
  istream *IS;
  char *BaseURL;
  int ReadFromInputString;
  char *InputString;
  int InputStringLength;
  vtkCharArray *InputArray;
  vtkSmartPointer<vtkVegaScene> VegaScene;

private:
  vtkVegaSpecReader(const vtkVegaSpecReader&);  // Not implemented.
  void operator=(const vtkVegaSpecReader&);  // Not implemented.
};

#endif
