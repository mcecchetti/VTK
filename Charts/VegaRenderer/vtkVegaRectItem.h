/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaRectItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaRectItem -
//
// .SECTION Description
// vtkVegaRectItem

#ifndef __vtkVegaRectItem_h
#define __vtkVegaRectItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaMarkItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaRectItem : public vtkVegaMarkItem
{
public:
  static const int Type = vtkVegaMarkItem::RECT;

public:
  static vtkVegaRectItem *New();
  vtkTypeMacro(vtkVegaRectItem, vtkVegaMarkItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaRectItem();
  ~vtkVegaRectItem();

private:
  vtkVegaRectItem(const vtkVegaRectItem& );  // Not implemented.
  void operator=(const vtkVegaRectItem& );  // Not implemented.
};

#endif
