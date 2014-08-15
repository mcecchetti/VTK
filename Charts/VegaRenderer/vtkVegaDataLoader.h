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
// .NAME vtkVegaDataLoader - The base class for Vega resource loaders.
//
// .SECTION Description
// vtkVegaDataLoader provides the basic functionalities shared
// by all Vega resource loaders.

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

  // Description:
  // Methods for set/get the URI from which to retrieve the resource.
  vtkSetStringMacro(InputURI);
  vtkGetStringMacro(InputURI);

  // Description:
  // Return true if the resource to be retrieved is a local file.
  bool IsFile(const vtkStdString& url);

  // Description:
  // In derived concrete classes is responsible for retrieving and loading
  // the resource specified by the set URI.
  virtual void Update();

protected:
  static const char* FileProtocol; // "file://"

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
