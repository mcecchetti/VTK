/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaImageItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaImageItem - The class which handles Vega mark item of image type.
//
// .SECTION Description
// vtkVegaImageItem provides the properties and functionality typical
// of a Vega mark item of image type.

#ifndef __vtkVegaImageItem_h
#define __vtkVegaImageItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaMarkItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaImageItem : public vtkVegaMarkItem
{
public:
  static const int Type = vtkVegaMarkItem::IMAGE;

public:
  // Description:
  // Valid horizontal alignment types.
  enum AlignEnum {
    LEFT = 0,
    CENTER,
    RIGHT
  };

  // Description:
  // Valid vertical alignment types.
  enum BaselineEnum {
    TOP = 0,
    MIDDLE,
    BOTTOM
  };

public:
  static vtkVegaImageItem *New();
  vtkTypeMacro(vtkVegaImageItem, vtkVegaMarkItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Methods for set/get the horizontal alignment of the image.
  // One of left, right, center.
  vtkSetMacro(Align, int);
  vtkGetMacro(Align, int);

  // Description:
  // Methods for set/get the vertical alignment of the image.
  // One of top, middle, bottom.
  vtkSetMacro(Baseline, int);
  vtkGetMacro(Baseline, int);

  // Description:
  // Methods for set/get the URL from which to retrieve the image.
  vtkSetStringMacro(Url);
  vtkGetStringMacro(Url);

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaImageItem();
  ~vtkVegaImageItem();

  char* Url;
  int Align;
  int Baseline;

private:
  vtkVegaImageItem(const vtkVegaImageItem& );  // Not implemented.
  void operator=(const vtkVegaImageItem& );  // Not implemented.
};

#endif
