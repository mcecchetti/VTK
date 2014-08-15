/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaSceneItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaSceneItem - The class which represents the root mark item
// of a Vega scene graph.
//
// .SECTION Description
// vtkVegaSceneItem inherits from vtkVegaGroupItem, but it has some special
// features since it represents the root mark item of a Vega scene graph.
// It defines padding properties (top, left, bottom, right) for the scene.

#ifndef __vtkVegaSceneItem_h
#define __vtkVegaSceneItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaGroupItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaSceneItem : public vtkVegaGroupItem
{
public:
  static const int Type = vtkVegaMarkItem::GROUP;

public:
  static vtkVegaSceneItem *New();
  vtkTypeMacro(vtkVegaSceneItem, vtkVegaGroupItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Methods for set/get the scene padding properties.
  vtkSetMacro(TopPadding, float);
  vtkGetMacro(TopPadding, float);
  vtkSetMacro(BottomPadding, float);
  vtkGetMacro(BottomPadding, float);
  vtkSetMacro(LeftPadding, float);
  vtkGetMacro(LeftPadding, float);
  vtkSetMacro(RightPadding, float);
  vtkGetMacro(RightPadding, float);
  void SetPadding(float top, float left, float bottom, float right);
  void GetPadding(float *padding);

  // Invokes the Paint method on all collections of mark items which made up
  // the scene. Since in SVG and in Canvas 2D context the Y axis points
  // downwards whilst in vtkContext2D points upwards, this method performs
  // a transformation in order to swap the Y axis direction.
  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaSceneItem();
  ~vtkVegaSceneItem();

protected:
  float TopPadding;
  float BottomPadding;
  float LeftPadding;
  float RightPadding;

private:
  vtkVegaSceneItem(const vtkVegaSceneItem& );  // Not implemented.
  void operator=(const vtkVegaSceneItem& );  // Not implemented.
};



#endif
