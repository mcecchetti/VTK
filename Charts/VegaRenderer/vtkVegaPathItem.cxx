/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaPathItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaPathItem.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaPathItem);


//------------------------------------------------------------------------------
void vtkVegaPathItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaPathItem::vtkVegaPathItem()
{
}


//------------------------------------------------------------------------------
vtkVegaPathItem::~vtkVegaPathItem()
{}


//------------------------------------------------------------------------------
bool vtkVegaPathItem::Paint(vtkContext2D* painter)
{

  return true;
}
