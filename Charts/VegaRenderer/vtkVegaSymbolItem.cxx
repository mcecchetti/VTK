/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaSymbolItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaSymbolItem.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"




vtkStandardNewMacro(vtkVegaSymbolItem);


//------------------------------------------------------------------------------
void vtkVegaSymbolItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaSymbolItem::vtkVegaSymbolItem()
{
}


//------------------------------------------------------------------------------
vtkVegaSymbolItem::~vtkVegaSymbolItem()
{}


//------------------------------------------------------------------------------
bool vtkVegaSymbolItem::Paint(vtkContext2D* painter)
{

  return true;
}
