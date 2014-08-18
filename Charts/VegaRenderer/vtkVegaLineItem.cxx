/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaLineItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaLineItem.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaLineItem);


//------------------------------------------------------------------------------
void vtkVegaLineItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaLineItem::vtkVegaLineItem()
{
}


//------------------------------------------------------------------------------
vtkVegaLineItem::~vtkVegaLineItem()
{}


//------------------------------------------------------------------------------
bool vtkVegaLineItem::Paint(vtkContext2D* painter)
{
  // Vega mark items of area type are rendered as filled SVG path.
  // The top-left point of a line item must be always (0,0).
  // A leading `moveTo` command is responsible to place it at the right place.
  this->X = 0.0;
  this->Y = 0.0;
  return this->Superclass::Paint(painter);
}