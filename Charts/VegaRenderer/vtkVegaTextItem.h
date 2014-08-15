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
// .NAME vtkVegaTextItem - The class which handles Vega mark item of text type.
//
// .SECTION Description
// vtkVegaTextItemprovides the properties and functionality typical
// of a Vega mark item of text type.

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

  // Description:
  // Methods for set/get text properties, such as font family, font size,
  // font weight, font style, horizontal and vertical text alignment and the
  // the rotation angle of the text label.
  void SetTextProperty(vtkTextProperty *textProp);
  vtkGetObjectMacro(TextProperty, vtkTextProperty);

  // Description:
  // Methods for set/get the text to display.
  vtkSetStringMacro(Text);
  vtkGetStringMacro(Text);

  // Description:
  // Methods for set/get the polar coordinate radial offset, in pixels, of the
  // text label from the origin determined by the x and y properties.
  vtkSetMacro(Radius, float);
  vtkGetMacro(Radius, float);

  // Description:
  // Methods for set/get the horizontal margin, in pixels, between the text
  // label and its anchor point. The value is ignored if the align property is
  // center.
  vtkSetMacro(Dx, float);
  vtkGetMacro(Dx, float);

  // Description:
  // Methods for set/get the vertical margin, in pixels, between the text label
  // and its anchor point. The value is ignored if the baseline property is
  // middle.
  vtkSetMacro(Dy, float);
  vtkGetMacro(Dy, float);

  // Description:
  // Methods for set/get the polar coordinate angle of the text
  // label from the origin determined by the x and y properties.
  // Values for theta are measured in radians, with 0 indicating "north".
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
