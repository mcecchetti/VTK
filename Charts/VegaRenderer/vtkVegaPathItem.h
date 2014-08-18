/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaPathItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaPathItem - The class which handles Vega mark item of path type.
//
// .SECTION Description
// vtkVegaPathItem provides the properties and functionality typical
// of a Vega mark item of path type.

#ifndef __vtkVegaPathItem_h
#define __vtkVegaPathItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaMarkItem.h"
#include "vtkSmartPointer.h"

class vtkSVGPath;

class VTKCHARTSVEGARENDERER_EXPORT vtkVegaPathItem : public vtkVegaMarkItem
{
public:
  static const int Type = vtkVegaMarkItem::PATH;

public:
  static vtkVegaPathItem *New();
  vtkTypeMacro(vtkVegaPathItem, vtkVegaMarkItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Methods for set/get an SVG path defined through an vtkSVGPath object.
  void SetPath(vtkSVGPath *path);
  vtkSVGPath* GetPath();

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaPathItem();
  ~vtkVegaPathItem();

protected:
  vtkSmartPointer<vtkSVGPath> Path;

private:
  vtkVegaPathItem(const vtkVegaPathItem& );  // Not implemented.
  void operator=(const vtkVegaPathItem& );  // Not implemented.
};

#endif