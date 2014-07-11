/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaArcItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaArcItem.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"
#include "vtkMath.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaArcItem);


//------------------------------------------------------------------------------
void vtkVegaArcItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "OuterRadius: " << this->OuterRadius << "\n";
  os << indent << "InnerRadius: " << this->InnerRadius << "\n";
  os << indent << "StartAngle: " << this->StartAngle << "\n";
  os << indent << "StopAngle: " << this->EndAngle << "\n";
}


//------------------------------------------------------------------------------
vtkVegaArcItem::vtkVegaArcItem()
{
  this->OuterRadius = 0;
  this->InnerRadius = 0;
  this->StartAngle = 0;
  this->EndAngle = 0;
}


//------------------------------------------------------------------------------
vtkVegaArcItem::~vtkVegaArcItem()
{}


//------------------------------------------------------------------------------
bool vtkVegaArcItem::Paint(vtkContext2D* painter)
{
  if (this->GetOpacity() == 0 )
    {
    return true;
    }

  static const double HALFPI = vtkMath::Pi() / 2.0;

  float startAngle = vtkMath::DegreesFromRadians(this->StartAngle - HALFPI);
  float stopAngle = vtkMath::DegreesFromRadians(this->EndAngle - HALFPI);

  // Draw fill.
  if (this->IsBrushVisible())
    {
    this->ApplyBrush(painter);

    painter->DrawWedge(this->X, this->Y,
                       this->OuterRadius, this->InnerRadius,
                       startAngle, stopAngle);
    }

  // Draw outline.
  if (this->IsPenVisible())
    {
    this->ApplyPen(painter);
    painter->GetBrush()->SetOpacityF(0.0);

    // Outer Arc.
    painter->DrawArc(this->X, this->Y, this->OuterRadius,
                     startAngle, stopAngle);

    // Inner Arc.
    painter->DrawArc(this->X, this->Y, this->InnerRadius,
                     startAngle, stopAngle);

    // Left ray segment.
    double cosStartAngle = std::cos(this->StartAngle - HALFPI);
    double sinStartAngle = std::sin(this->StartAngle - HALFPI);
    float x1 = static_cast<float>(cosStartAngle * this->InnerRadius);
    float y1 = static_cast<float>(sinStartAngle * this->InnerRadius);
    float x2 = static_cast<float>(cosStartAngle * this->OuterRadius);
    float y2 = static_cast<float>(sinStartAngle * this->OuterRadius);
    painter->DrawLine(x1 + this->X, y1 + this->Y, x2 + this->X, y2 + this->Y);

    // Right ray segment.
    double cosEndAngle = std::cos(this->EndAngle - HALFPI);
    double sinEndAngle = std::sin(this->EndAngle - HALFPI);
    x1 = static_cast<float>(cosEndAngle * this->InnerRadius);
    y1 = static_cast<float>(sinEndAngle * this->InnerRadius);
    x2 = static_cast<float>(cosEndAngle * this->OuterRadius);
    y2 = static_cast<float>(sinEndAngle * this->OuterRadius);
    painter->DrawLine(x1 + this->X, y1 + this->Y, x2 + this->X, y2 + this->Y);
    }

  return true;
}
