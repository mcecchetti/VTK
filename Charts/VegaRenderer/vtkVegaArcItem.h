/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaArcItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaArcItem -
//
// .SECTION Description
// vtkVegaArcItem

#ifndef __vtkVegaArcItem_h
#define __vtkVegaArcItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaMarkItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaArcItem : public vtkVegaMarkItem
{
public:
  static const int Type = vtkVegaMarkItem::ARC;

public:
  static vtkVegaArcItem *New();
  vtkTypeMacro(vtkVegaArcItem, vtkVegaMarkItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(OuterRadius, float);
  vtkGetMacro(OuterRadius, float);

  vtkSetMacro(InnerRadius, float);
  vtkGetMacro(InnerRadius, float);

  vtkSetMacro(StartAngle, double);
  vtkGetMacro(StartAngle, double);

  vtkSetMacro(EndAngle, double);
  vtkGetMacro(EndAngle, double);


  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaArcItem();
  ~vtkVegaArcItem();

  float OuterRadius;
  float InnerRadius;
  double StartAngle;
  double EndAngle;

private:
  vtkVegaArcItem(const vtkVegaArcItem& );  // Not implemented.
  void operator=(const vtkVegaArcItem& );  // Not implemented.
};

#endif
