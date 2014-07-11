/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaMarkItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaMarkItem.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"


//------------------------------------------------------------------------------
void vtkVegaMarkItem::PrintSelf(ostream& os, vtkIndent indent)
{
  //this->Superclass::PrintSelf(os, indent);
  os << indent << "X: " << this->X << "\n";
  os << indent << "Y: " << this->Y << "\n";
  os << indent << "X2: " << this->X2 << "\n";
  os << indent << "Y2: " << this->Y2 << "\n";
  os << indent << "Width: " << this->Width << "\n";
  os << indent << "Height: " << this->Height << "\n";
  os << indent << "Opacity: " << this->Opacity << "\n";

  vtkIndent nextIndent = indent.GetNextIndent();
  os << indent << "Brush: \n";
  unsigned char color[4];
  this->Brush->GetColor(color);
  os << nextIndent << "color: (" << (int)color[0] << ", " << (int)color[1] << ", "
                                 << (int)color[2] << ", " << (int)color[3] << ")\n";
  os << indent << "Pen: \n";
  this->Pen->GetColor(color);
  color[3] = this->Pen->GetOpacity();
  os << nextIndent << "color: (" << (int)color[0] << ", " << (int)color[1] << ", "
                                 << (int)color[2] << ", " << (int)color[3] << ")\n";
  os << nextIndent << "width: " << this->Pen->GetWidth() << "\n";
}


//------------------------------------------------------------------------------
const char* vtkVegaMarkItem::GetMarkName(int type)
{
  static const char* names[] = {
      "None",
      "Arc",
      "Area",
      "Group",
      "Image",
      "Line",
      "Path",
      "Rect",
      "Rule",
      "Symbol",
      "Text",
      ""
  };

  if ( type < vtkVegaMarkItem::UNKNOWN)
    {
    return names[type];
    }
  else
    {
    return names[vtkVegaMarkItem::UNKNOWN];
    }
}


//------------------------------------------------------------------------------
vtkVegaMarkItem::vtkVegaMarkItem()
{
  this->X = 0.0;
  this->Y = 0.0;
  this->X2 = 0.0;
  this->Y2 = 0.0;
  this->Width = 0.0;
  this->Height = 0.0;
  this->Opacity = 1.0;
  this->Pen = vtkPen::New();
  this->Brush = vtkBrush::New();
}


//------------------------------------------------------------------------------
vtkVegaMarkItem::~vtkVegaMarkItem()
{
  if (this->Pen)
    {
    this->Pen->Delete();
    }
  if (this->Brush)
    {
    this->Brush->Delete();
    }
}


//------------------------------------------------------------------------------
void vtkVegaMarkItem::SetPen(vtkPen *pen)
{
  if (pen == NULL)
    {
    return;
    }
  if (this->Pen != pen)
    {
    this->Pen->DeepCopy(pen);
    this->Modified();
    }
}


//------------------------------------------------------------------------------
void vtkVegaMarkItem::SetBrush(vtkBrush *brush)
{
  if (brush == NULL)
    {
    return;
    }
  if (this->Brush != brush)
    {
    this->Brush->DeepCopy(brush);
    this->Modified();
    }
}


//------------------------------------------------------------------------------
bool vtkVegaMarkItem::IsPenVisible()
{
  return (this->GetPen()->GetOpacity() != 0 && this->GetPen()->GetWidth() != 0);
}


//------------------------------------------------------------------------------
bool vtkVegaMarkItem::IsBrushVisible()
{
  return (this->GetBrush()->GetOpacity() != 0);
}


//------------------------------------------------------------------------------
unsigned char vtkVegaMarkItem::GetGlobalOpacity(unsigned char toolOpacity)
{
  return static_cast<unsigned char>(toolOpacity * this->GetOpacity());
}


//------------------------------------------------------------------------------
void vtkVegaMarkItem::ApplyPen(vtkContext2D *painter)
{
  painter->ApplyPen(this->GetPen());
  vtkPen* pen = painter->GetPen();
  unsigned char gloabalOpacity = this->GetGlobalOpacity(pen->GetOpacity());
  pen->SetOpacity(gloabalOpacity);
}


//------------------------------------------------------------------------------
void vtkVegaMarkItem::ApplyBrush(vtkContext2D *painter)
{
  painter->ApplyBrush(this->GetBrush());
  vtkBrush* brush = painter->GetBrush();
  unsigned char gloabalOpacity = this->GetGlobalOpacity(brush->GetOpacity());
  brush->SetOpacity(gloabalOpacity);
}
