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
#include "vtkVegaPathItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaSymbolItem : public vtkVegaPathItem
{
public:
  static const int Type = vtkVegaMarkItem::SYMBOL;

public:
  enum
  {
    CIRCLE = 0,
    SQUARE,
    CROSS,
    DIAMOND,
    TRIANGLE_UP,
    TRIANGLE_DOWN,
    UNKNOWN
  };

public:
  static vtkVegaSymbolItem *New();
  vtkTypeMacro(vtkVegaSymbolItem, vtkVegaPathItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(Shape, int);
  vtkGetMacro(Shape, int);

  vtkSetMacro(Size, float);
  vtkGetMacro(Size, float);

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaSymbolItem();
  ~vtkVegaSymbolItem();

  void DrawCircle();
  void DrawSquare();
  void DrawCross();
  void DrawDiamond();
  void DrawTriangleUp();
  void DrawTriangleDown();

  int Shape;
  float Size;


private:
  vtkVegaSymbolItem(const vtkVegaSymbolItem& );  // Not implemented.
  void operator=(const vtkVegaSymbolItem& );  // Not implemented.
};

#endif
