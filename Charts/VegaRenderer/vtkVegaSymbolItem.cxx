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
#include "vtkSmartPointer.h"
#include "vtkSVGPath.h"
#include "vtkMath.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"

#include <cmath>


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaSymbolItem);


//------------------------------------------------------------------------------
void vtkVegaSymbolItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaSymbolItem::vtkVegaSymbolItem()
{
  this->Shape = vtkVegaSymbolItem::CIRCLE;
  this->Size = 100;
}


//------------------------------------------------------------------------------
vtkVegaSymbolItem::~vtkVegaSymbolItem()
{}


//------------------------------------------------------------------------------
bool vtkVegaSymbolItem::Paint(vtkContext2D* painter)
{
  switch (this->Shape)
    {
    default:
    case vtkVegaSymbolItem::CIRCLE:
      {
      this->DrawCircle();
      break;
      }
    case vtkVegaSymbolItem::SQUARE:
      {
      this->DrawSquare();
      break;
      }
    case vtkVegaSymbolItem::CROSS:
      {
      this->DrawCross();
      break;
      }
    case vtkVegaSymbolItem::DIAMOND:
      {
      this->DrawDiamond();
      break;
      }
    case vtkVegaSymbolItem::TRIANGLE_UP:
      {
      this->DrawTriangleUp();
      break;
      }
    case vtkVegaSymbolItem::TRIANGLE_DOWN:
      {
      this->DrawTriangleDown();
      break;
      }
    }

  this->Superclass::Paint(painter);
  return true;
}

//------------------------------------------------------------------------------
void vtkVegaSymbolItem::DrawCircle()
{
  vtkSVGPath* path = this->Path.GetPointer();

  float r = std::sqrt(this->Size / vtkMath::Pi());

  path->AppendCommand('M', 0, r);
  path->AppendCommand('A', r, r, 0, 1, 1, 0, -r);
  path->AppendCommand('A', r, r, 0, 1, 1, 0, r);
  path->AppendCommand('Z');
}

//------------------------------------------------------------------------------
void vtkVegaSymbolItem::DrawSquare()
{
  vtkSVGPath* path = this->Path.GetPointer();

  float r = std::sqrt(this->Size) / 2.0;

  path->AppendCommand('M', -r, -r);
  path->AppendCommand('L', r, -r);
  path->AppendCommand('L', r, r);
  path->AppendCommand('L', -r, r);
  path->AppendCommand('Z');
}

//------------------------------------------------------------------------------
void vtkVegaSymbolItem::DrawCross()
{
  vtkSVGPath* path = this->Path.GetPointer();

  float r = std::sqrt(this->Size / 5.0) / 2.0;

  path->AppendCommand('M', -3 * r, -r);
  path->AppendCommand('H', -r);
  path->AppendCommand('V', -3 * r);
  path->AppendCommand('H', r);
  path->AppendCommand('V', -r);
  path->AppendCommand('H', 3 * r);
  path->AppendCommand('V', r);
  path->AppendCommand('H', r);
  path->AppendCommand('V', 3 * r);
  path->AppendCommand('H', -r);
  path->AppendCommand('V', r);
  path->AppendCommand('H', -3 * r);
  path->AppendCommand('Z');
}

//------------------------------------------------------------------------------
void vtkVegaSymbolItem::DrawDiamond()
{
  static const double Tan30 = std::tan(vtkMath::Pi() / 6.0);

  vtkSVGPath* path = this->Path.GetPointer();

  float ry = std::sqrt(this->Size / (2 * Tan30));
  float rx = ry * Tan30;

  path->AppendCommand('M', 0, -ry);
  path->AppendCommand('L', rx, 0);
  path->AppendCommand('L', 0, ry);
  path->AppendCommand('L', -rx, 0);
  path->AppendCommand('Z');
}

//------------------------------------------------------------------------------
void vtkVegaSymbolItem::DrawTriangleUp()
{
  static const double Sqrt3 = std::sqrt(3.0);

  vtkSVGPath* path = this->Path.GetPointer();

  float rx = std::sqrt(this->Size / Sqrt3);
  float ry = rx * Sqrt3 / 2.0;

  path->AppendCommand('M', 0, -ry);
  path->AppendCommand('L', rx, ry);
  path->AppendCommand('L', -rx, ry);
  path->AppendCommand('Z');
}

//------------------------------------------------------------------------------
void vtkVegaSymbolItem::DrawTriangleDown()
{
  static const double Sqrt3 = std::sqrt(3.0);

  vtkSVGPath* path = this->Path.GetPointer();

  float rx = std::sqrt(this->Size / Sqrt3);
  float ry = rx * Sqrt3 / 2.0;

  path->AppendCommand('M', 0, ry);
  path->AppendCommand('L', rx, -ry);
  path->AppendCommand('L', -rx, -ry);
  path->AppendCommand('Z');
}
