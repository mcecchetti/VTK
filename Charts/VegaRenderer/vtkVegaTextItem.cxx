/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaTextItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaTextItem.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkContext2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"
#include "vtkTextProperty.h"
#include "vtkTransform2D.h"
#include "vtkMath.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaTextItem);


//------------------------------------------------------------------------------
void vtkVegaTextItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Text: ";
  if (this->Text)
    {
    os << '"' << this->Text << '"';
    }
  else
    {
    os << "NULL";
    }
  os << "\n";

  os << indent << "Font family: "
     << this->TextProperty->GetFontFamilyAsString() << "\n";
  os << indent << "Font size: " << this->TextProperty->GetFontSize() << "\n";
  os << indent << "Font bold: " << this->TextProperty->GetBold() << "\n";
  os << indent << "Font italic: " << this->TextProperty->GetItalic() << "\n";

  os << indent << "Align: "
     << this->TextProperty->GetJustificationAsString() << "\n";
  os << indent << "Baseline: "
     << this->TextProperty->GetVerticalJustificationAsString() << "\n";

  os << indent << "Text angle: " << this->TextProperty->GetOrientation() << "\n";

  os << indent << "Dx: " << this->Dx << "\n";
  os << indent << "Dy: " << this->Dy << "\n";
  os << indent << "Radius: " << this->Radius << "\n";
  os << indent << "Theta: " << this->Theta << "\n";
}


//------------------------------------------------------------------------------
vtkVegaTextItem::vtkVegaTextItem()
{
  this->Text = NULL;
  this->TextProperty = vtkTextProperty::New();
  this->Dx = 0;
  this->Dy = 0;
  this->Radius = 0;
  this->Theta = 0;
}


//------------------------------------------------------------------------------
vtkVegaTextItem::~vtkVegaTextItem()
{
  if (this->TextProperty)
    {
    this->TextProperty->Delete();
    }
  if (this->Text)
    {
    delete [] this->Text;
    }
}


//------------------------------------------------------------------------------
void vtkVegaTextItem::SetTextProperty(vtkTextProperty *textProp)
{
  if (textProp == NULL)
    {
    return;
    }
  if (this->TextProperty != textProp)
    {
    this->TextProperty->ShallowCopy(textProp);
    this->Modified();
    }
}


//------------------------------------------------------------------------------
bool vtkVegaTextItem::Paint(vtkContext2D* painter)
{
  if (this->TextProperty->GetOpacity() == 0 || this->Text == NULL || *(this->Text) == '\0' )
    {
    return true;
    }

  static const double HALFPI = vtkMath::Pi() / 2.0;

  float x = this->X;
  float y = this->Y;

  // Add the polar offset.
  double t = this->Theta - HALFPI;  // Theta == 0 indicates 'north'.
  float offsetX = static_cast<float>(this->Radius * std::cos(t));
  float offsetY = static_cast<float>(this->Radius * std::sin(t));
  x += offsetX;
  y += offsetY;

  // Add the xy-offset.
  double angle = this->TextProperty->GetOrientation();
  if (angle != 0)
    {
    // We need to perform a translation T{dx, dy} followed by a rotation R{angle}
    // of the text label. Unfortunately it is not possible to get this result
    // by using the `orientation` text property, because the text rotation is
    // always performed earlier than the translation.
    // By setting the `orientation` to 0 and applying a rotate transformation,
    // we would get the wanted result, but text label appearance is ugly.
    // Since R{angle}(T{dx, dy}(x, y)) = T{R{angle}(dx, dy)}(x, y),
    // the solution is to rotate the offset vector (dx, dy) and use the
    // result as the actual offset vector in place of (dx, dy).
    double deltaVector[3] = {this->Dx, this->Dy, 0};
    double rotatedDeltaVector[3] = {0, 0, 0};
    vtkSmartPointer<vtkTransform2D> transform = vtkSmartPointer<vtkTransform2D>::New();
    transform->Rotate(angle);
    transform->MultiplyPoint(deltaVector, rotatedDeltaVector);
    x += rotatedDeltaVector[0];
    y += rotatedDeltaVector[1];
    }
  else
    {
    x += this->Dx;
    y += this->Dy;
    }

  painter->ApplyTextProp(this->TextProperty);

  painter->DrawString(x, y, this->Text);

  return true;
}
