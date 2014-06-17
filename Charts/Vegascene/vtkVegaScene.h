/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegascene.h

  Copyright (c) Marco Cecchetti
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegascene - a VTK wrapper for VegaScene class
// .SECTION Description
// This class is used for reading Vega spec files and generating the related
// scene graph in JSON format. The generated scene graph can be get as a string
// or written directly to a file.


#ifndef __vtkVegaScene_h
#define __vtkVegaScene_h


#include "vtkChartsVegasceneModule.h"  // For export macro
#include "vtkObject.h"

#include "vegascene.h"




class VTKCHARTSVEGASCENE_EXPORT vtkVegaScene : public VegaScene, public vtkObject
{
public:
  static vtkVegaScene *New();
  vtkTypeMacro(vtkVegaScene, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkVegaScene();
  ~vtkVegaScene();

private:
  vtkVegaScene(const vtkVegaScene&);  // Not implemented.
  void operator=(const vtkVegaScene&);  // Not implemented.
};


// Description:
// Write the scene graph generated from the passed Vega spec file to a file or
// to the standard output if the passed output file path is empty.
// The output content will be in JSON format. An URL used as base path for
// finding data resources referenced in the spec file, can be passed as third
// argument.
VTKCHARTSVEGASCENE_EXPORT
int vtkVegascene(const String& specFilePath,
                 const String& outFilePath = String(""),
                 const String& baseURL = String(""));
#endif
