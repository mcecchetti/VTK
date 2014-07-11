/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaSymbolItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaSymbolItem -
//
// .SECTION Description
// vtkVegaSymbolItem

#ifndef __vtkVegaSymbolItem_h
#define __vtkVegaSymbolItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaMarkItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaSymbolItem : public vtkVegaMarkItem
{
public:
  static const int Type = vtkVegaMarkItem::SYMBOL;

public:
  static vtkVegaSymbolItem *New();
  vtkTypeMacro(vtkVegaSymbolItem, vtkVegaMarkItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaSymbolItem();
  ~vtkVegaSymbolItem();


private:
  vtkVegaSymbolItem(const vtkVegaSymbolItem& );  // Not implemented.
  void operator=(const vtkVegaSymbolItem& );  // Not implemented.
};

#endif
