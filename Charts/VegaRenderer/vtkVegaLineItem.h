/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaLineItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaLineItem -
//
// .SECTION Description
// vtkVegaLineItem

#ifndef __vtkVegaLineItem_h
#define __vtkVegaLineItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaPathItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaLineItem : public vtkVegaPathItem
{
public:
  static const int Type = vtkVegaMarkItem::LINE;

public:
  static vtkVegaLineItem *New();
  vtkTypeMacro(vtkVegaLineItem, vtkVegaPathItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaLineItem();
  ~vtkVegaLineItem();


private:
  vtkVegaLineItem(const vtkVegaLineItem& );  // Not implemented.
  void operator=(const vtkVegaLineItem& );  // Not implemented.
};

#endif
