/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaAreaItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaAreaItem.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaAreaItem);


//------------------------------------------------------------------------------
void vtkVegaAreaItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaAreaItem::vtkVegaAreaItem()
{
}


//------------------------------------------------------------------------------
vtkVegaAreaItem::~vtkVegaAreaItem()
{}


//------------------------------------------------------------------------------
bool vtkVegaAreaItem::Paint(vtkContext2D* painter)
{
  // Vega mark items of area type are rendered as filled SVG path.
  return this->Superclass::Paint(painter);
}
