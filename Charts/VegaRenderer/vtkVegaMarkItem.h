/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaMarkItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaMarkItem - The base class for all Vega mark items.
//
// .SECTION Description
// vtkVegaMarkItem provides the basic properties and functionalities shared by
// all Vega mark items.

#ifndef __vtkVegaMarkItem_h
#define __vtkVegaMarkItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkContextItem.h"


class vtkContext2D;
class vtkPen;
class vtkBrush;

class VTKCHARTSVEGARENDERER_EXPORT vtkVegaMarkItem : public vtkContextItem
{
public:
  vtkTypeMacro(vtkVegaMarkItem, vtkContextItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // Vega mark item types.
  enum
  {
    NONE = 0,
    ARC,
    AREA,
    GROUP,
    IMAGE,
    LINE,
    PATH,
    RECT,
    RULE,
    SYMBOL,
    TEXT,
    UNKNOWN
  };

public:
  static const int Type = vtkVegaMarkItem::NONE;

  static bool IsMarkTypeValid(int t)
  { return ( NONE < t && t < UNKNOWN); }

  static const char* GetMarkName(int type);

  // Description:
  // Methods for set/get the first (typically left-most) x-coordinate.
  vtkSetMacro(X, float);
  vtkGetMacro(X, float);

  // Description:
  // Methods for set/get the first (typically top-most) y-coordinate.
  vtkSetMacro(Y, float);
  vtkGetMacro(Y, float);

  // Description:
  // Methods for set/get the second (typically right-most) x-coordinate.
  vtkSetMacro(X2, float);
  vtkGetMacro(X2, float);

  // Description:
  // Methods for set/get the second (typically bottom-most) y-coordinate.
  vtkSetMacro(Y2, float);
  vtkGetMacro(Y2, float);

  // Description:
  // Methods for set/get the width of the mark (if supported).
  vtkSetMacro(Width, float);
  vtkGetMacro(Width, float);

  // Description:
  // Methods for set/get the height of the mark (if supported).
  vtkSetMacro(Height, float);
  vtkGetMacro(Height, float);

  // Description:
  // Methods for set/get stroke color, opacity and width.
  void SetPen(vtkPen *pen);
  vtkGetObjectMacro(Pen, vtkPen);

  // Description:
  // Methods for set/get fill color and opacity.
  void SetBrush(vtkBrush *brush);
  vtkGetObjectMacro(Brush, vtkBrush);

  // Description:
  // Return true if stroke width and opacity are not zero.
  bool IsPenVisible();

  // Description:
  // Return true if fill opacity is not zero.
  bool IsBrushVisible();

protected:
  vtkVegaMarkItem();
  ~vtkVegaMarkItem();

  unsigned char GetGlobalOpacity(unsigned char toolOpacity);
  virtual void ApplyPen(vtkContext2D *painter);
  virtual void ApplyBrush(vtkContext2D *painter);

protected:
  float X;
  float Y;
  float X2;
  float Y2;
  float Width;
  float Height;
  vtkPen* Pen;
  vtkBrush* Brush;

private:
  vtkVegaMarkItem(const vtkVegaMarkItem&);  // Not implemented.
  void operator=(const vtkVegaMarkItem&);   // Not implemented.
};

#endif
