/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaAreaItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaAreaItem -  The class which handles Vega mark item of area type.
//
// .SECTION Description
// vtkVegaAreaItem is used for rendering mark of area type.

#ifndef __vtkVegaAreaItem_h
#define __vtkVegaAreaItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaLineItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaAreaItem : public vtkVegaLineItem
{
public:
  static const int Type = vtkVegaMarkItem::AREA;

public:
  static vtkVegaAreaItem *New();
  vtkTypeMacro(vtkVegaAreaItem, vtkVegaLineItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaAreaItem();
  ~vtkVegaAreaItem();


private:
  vtkVegaAreaItem(const vtkVegaAreaItem& );  // Not implemented.
  void operator=(const vtkVegaAreaItem& );  // Not implemented.
};

#endif
