/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaTextItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaTextItem -
//
// .SECTION Description
// vtkVegaTextItem

#ifndef __vtkVegaTextItem_h
#define __vtkVegaTextItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaMarkItem.h"

class vtkTextProperty;

class VTKCHARTSVEGARENDERER_EXPORT vtkVegaTextItem : public vtkVegaMarkItem
{
public:
  static const int Type = vtkVegaMarkItem::TEXT;

public:
  static vtkVegaTextItem *New();
  vtkTypeMacro(vtkVegaTextItem, vtkVegaMarkItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  void SetTextProperty(vtkTextProperty *textProp);
  vtkGetObjectMacro(TextProperty, vtkTextProperty);

  vtkSetStringMacro(Text);
  vtkGetStringMacro(Text);

  vtkSetMacro(Radius, float);
  vtkGetMacro(Radius, float);

  vtkSetMacro(Dx, float);
  vtkGetMacro(Dx, float);

  vtkSetMacro(Dy, float);
  vtkGetMacro(Dy, float);

  vtkSetMacro(Theta, double);
  vtkGetMacro(Theta, double);

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaTextItem();
  ~vtkVegaTextItem();

  vtkTextProperty* TextProperty;
  char* Text;
  float Dx;
  float Dy;
  float Radius;
  double Theta;

private:
  vtkVegaTextItem(const vtkVegaTextItem& );  // Not implemented.
  void operator=(const vtkVegaTextItem& );  // Not implemented.
};

#endif
