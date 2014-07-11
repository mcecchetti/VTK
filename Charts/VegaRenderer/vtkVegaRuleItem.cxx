/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaRuleItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaRuleItem.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaRuleItem);


//------------------------------------------------------------------------------
void vtkVegaRuleItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaRuleItem::vtkVegaRuleItem()
{
}


//------------------------------------------------------------------------------
vtkVegaRuleItem::~vtkVegaRuleItem()
{
}


//------------------------------------------------------------------------------
bool vtkVegaRuleItem::Paint(vtkContext2D* painter)
{
  if (this->GetOpacity() == 0 || (!this->IsPenVisible() && !this->IsBrushVisible()))
    {
    return true;
    }

  this->ApplyPen(painter);
  this->ApplyBrush(painter);

//  float point[] = {this->X, this->Y};
//
//  static int symbol = 0;
//
//  symbol = (symbol + 1) % 6;
//  symbol = (symbol == 0) ? 1 : symbol;
//  painter->GetPen()->SetWidth(25);
//  painter->GetPen()->SetColor(255, 0, 0);
//  painter->GetBrush()->SetColor(0, 0, 255);
//  painter->DrawMarkers(symbol, false, point, 1);

  painter->DrawLine(this->X, this->Y, this->X2, this->Y2);

  return true;
}


//------------------------------------------------------------------------------
void vtkVegaRuleItem::ApplyBrush(vtkContext2D *painter)
{
  vtkBrush* brush = painter->GetBrush();
  brush->SetColorF(0, 0, 0, 0);
}
