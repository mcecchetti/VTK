/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaRectItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaRectItem.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaRectItem);


//------------------------------------------------------------------------------
void vtkVegaRectItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaRectItem::vtkVegaRectItem()
{
}


//------------------------------------------------------------------------------
vtkVegaRectItem::~vtkVegaRectItem()
{
}


//------------------------------------------------------------------------------
bool vtkVegaRectItem::Paint(vtkContext2D* painter)
{
  if (this->GetOpacity() == 0 || (!this->IsPenVisible() && !this->IsBrushVisible()))
    {
    return true;
    }

  this->ApplyPen(painter);
  this->ApplyBrush(painter);

  painter->DrawRect(this->X, this->Y, this->Width, this->Height);

  return true;
}
