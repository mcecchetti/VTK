/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaDataLoader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaDataLoader -
//
// .SECTION Description
// vtkVegaDataLoader

#ifndef __vtkVegaDataLoader_h
#define __vtkVegaDataLoader_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkObject.h"

class vtkStdString;

class VTKCHARTSVEGARENDERER_EXPORT vtkVegaDataLoader : public vtkObject
{
public:
  vtkTypeMacro(vtkVegaDataLoader, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(InputURI);
  vtkGetStringMacro(InputURI);

  bool IsFile(const vtkStdString& url);

  virtual void Update();

protected:
  static const char* FileProtocol;

protected:
  vtkVegaDataLoader();
  ~vtkVegaDataLoader();

  vtkSetStringMacro(FilePath);
  vtkGetStringMacro(FilePath);

  char* InputURI;
  char* FilePath;

private:
  vtkVegaDataLoader(const vtkVegaDataLoader& );  // Not implemented.
  void operator=(const vtkVegaDataLoader& );  // Not implemented.
};

#endif
