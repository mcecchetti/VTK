/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaImageItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaImageItem.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkVegaImageLoader.h"
#include "vtkImageData.h"
#include "vtkRect.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaImageItem);


//------------------------------------------------------------------------------
void vtkVegaImageItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  std::string align;
  switch (this->Align)
    {
    case vtkVegaImageItem::CENTER:
      align.append("center");
      break;
    case vtkVegaImageItem::RIGHT:
      align.append("right");
      break;
    case vtkVegaImageItem::LEFT:
      align.append("left");
      break;
    default:
      align.append("unknown");
    }
  os << indent << "Align: " << align << "\n";

  std::string baseline;
  switch (this->Baseline)
    {
    case vtkVegaImageItem::MIDDLE:
      baseline.append("middle");
      break;
    case vtkVegaImageItem::TOP:
      baseline.append("top");
      break;
    case vtkVegaImageItem::BOTTOM:
      baseline.append("bottom");
      break;
    default:
      baseline.append("unknown");
    }
  os << indent << "Baseline: " << baseline << "\n";

  if (this->Url)
    {
    os << indent << "Url: '" << this->Url << "'\n";
    }
  else
    {
    os << indent << "Url:\n";
    }

}


//------------------------------------------------------------------------------
vtkVegaImageItem::vtkVegaImageItem()
{
  this->Align = vtkVegaImageItem::LEFT;
  this->Baseline = vtkVegaImageItem::TOP;
  this->Url = NULL;
}


//------------------------------------------------------------------------------
vtkVegaImageItem::~vtkVegaImageItem()
{
  if (this->Url)
    {
    delete [] this->Url;
    }
}


//------------------------------------------------------------------------------
bool vtkVegaImageItem::Paint(vtkContext2D* painter)
{
  if (this->GetOpacity() == 0 )
    {
    return true;
    }

  vtkSmartPointer<vtkVegaImageLoader> loader = vtkSmartPointer<vtkVegaImageLoader>::New();
  loader->SetInputURI(this->GetUrl());
  loader->Update();
  vtkImageData* image = loader->GetOutputImage();
  if (image)
    {
    float x;
    switch (this->Align)
      {
      case vtkVegaImageItem::CENTER:
        x = this->X - (this->Width / 2.0);
        break;
      case vtkVegaImageItem::RIGHT:
        x = this->X - this->Width;
        break;
      case vtkVegaImageItem::LEFT:
        /* no break */
      default:
        x = this->X;
      }

    float y;
    switch (this->Baseline)
      {
      case vtkVegaImageItem::MIDDLE:
        y = this->Y + (this->Height / 2.0);
        break;
      case vtkVegaImageItem::TOP:
        y = this->Y + this->Height;
        break;
      case vtkVegaImageItem::BOTTOM:
        /* no break */
      default:
        y = this->Y;
      }

    // We need the negative height or image is drawn upside down.
    vtkRectf rect(x, y, this->Width, -this->Height);
    painter->DrawImage(rect, image);
    }
  return true;
}
