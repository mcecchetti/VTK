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
// .NAME vtkVegaImageItem -
//
// .SECTION Description
// vtkVegaImageItem

#ifndef __vtkVegaImageItem_h
#define __vtkVegaImageItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaMarkItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaImageItem : public vtkVegaMarkItem
{
public:
  static const int Type = vtkVegaMarkItem::IMAGE;

public:
  enum AlignEnum {
    LEFT = 0,
    CENTER,
    RIGHT
  };

  enum BaselineEnum {
    TOP = 0,
    MIDDLE,
    BOTTOM
  };

public:
  static vtkVegaImageItem *New();
  vtkTypeMacro(vtkVegaImageItem, vtkVegaMarkItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(Align, int);
  vtkGetMacro(Align, int);

  vtkSetMacro(Baseline, int);
  vtkGetMacro(Baseline, int);

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
