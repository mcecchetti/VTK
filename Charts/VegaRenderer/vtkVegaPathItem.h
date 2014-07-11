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
// .NAME vtkVegaPathItem -
//
// .SECTION Description
// vtkVegaPathItem

#ifndef __vtkVegaPathItem_h
#define __vtkVegaPathItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaMarkItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaPathItem : public vtkVegaMarkItem
{
public:
  static const int Type = vtkVegaMarkItem::PATH;

public:
  static vtkVegaPathItem *New();
  vtkTypeMacro(vtkVegaPathItem, vtkVegaMarkItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaPathItem();
  ~vtkVegaPathItem();


private:
  vtkVegaPathItem(const vtkVegaPathItem& );  // Not implemented.
  void operator=(const vtkVegaPathItem& );  // Not implemented.
};

#endif
