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

#include "vtkVegaSceneItem.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkContext2D.h"
#include "vtkTransform2D.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaSceneItem);


//------------------------------------------------------------------------------
void vtkVegaSceneItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaSceneItem::vtkVegaSceneItem()
{
  this->TopPadding = 0;
  this->BottomPadding = 0;
  this->LeftPadding = 0;
  this->RightPadding = 0;
}


//------------------------------------------------------------------------------
vtkVegaSceneItem::~vtkVegaSceneItem()
{
}


//------------------------------------------------------------------------------
void vtkVegaSceneItem::SetPadding(float top, float left, float bottom, float right)
{
  this->TopPadding = top;
  this->BottomPadding = bottom;
  this->LeftPadding = left;
  this->RightPadding = right;
}


//------------------------------------------------------------------------------
void vtkVegaSceneItem::GetPadding(float *padding)
{
  padding[0] = this->TopPadding;
  padding[1] = this->LeftPadding;
  padding[2] = this->BottomPadding;
  padding[3] = this->RightPadding;
}


//------------------------------------------------------------------------------
bool vtkVegaSceneItem::Paint(vtkContext2D* painter)
{
  painter->PushMatrix();

  // First off, we need to swap the Y axis direction since in `SVG` and `Canvas`
  // 2D context the Y axis points downwards whilst in vtkContext2D it points
  // upwards.
  vtkSmartPointer<vtkTransform2D> transform = vtkSmartPointer<vtkTransform2D>::New();
  transform->Scale(1.0, -1.0);
  transform->Translate(0, -this->Height);
  painter->AppendTransform(transform);

  // We need to paint the background before adjusting the coords for padding.
  this->Superclass::Superclass::Paint(painter);

  // Perform a translation for padding.
  transform->Identity();
  transform->Translate(this->LeftPadding, this->TopPadding);
  painter->AppendTransform(transform);
  // Paint the scene content.
  this->Superclass::PaintChildren(painter);

  painter->PopMatrix();
  return true;
}
