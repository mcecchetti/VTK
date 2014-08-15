/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaGroupItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaGroupItem.h"
#include "vtkVegaMarkItemCollection.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkContext2D.h"
#include "vtkTransform2D.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaGroupItem);


//------------------------------------------------------------------------------
void vtkVegaGroupItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  unsigned int n = this->GetNumberOfCollections();
  for (unsigned int i = 0; i < n; ++i)
    {
    vtkVegaMarkItemCollection* collection = this->ItemCollections[i];
    if (collection != NULL)
      {
      os << indent << "Collection of mark items [" << i << "]: \n";
      collection->PrintSelf(os, indent.GetNextIndent());
      }
    }
}


//------------------------------------------------------------------------------
vtkVegaGroupItem::vtkVegaGroupItem()
{
}


//------------------------------------------------------------------------------
vtkVegaGroupItem::~vtkVegaGroupItem()
{
  unsigned int n = this->GetNumberOfCollections();
  for (unsigned int i = 0; i < n; ++i)
    {
    vtkVegaMarkItemCollection* collection = this->ItemCollections[i];
    if (collection != NULL)
      {
      collection->RemoveAllMarkItems();
      collection->Delete();
      }
    }
}


//------------------------------------------------------------------------------
vtkVegaMarkItemCollection* vtkVegaGroupItem::GetMarkItemCollection(IndexType index)
{
  if (this->IsIndexUsed(index))
    {
    return this->ItemCollections[index];
    }
  else if (index == this->GetNumberOfCollections())
    {
    vtkVegaMarkItemCollection* collection = vtkVegaMarkItemCollection::New();
    if (collection == NULL)
      {
      vtkErrorMacro(<< "failed to create a new vtkVegaMarkItemCollection object.");
      return NULL;
      }
    this->ItemCollections.push_back(collection);
    this->Modified();
    return collection;
    }
  else
    {
    vtkWarningMacro(<< "passed index argument (" << index
                    << ") is greater than internal array size ("
                    << this->GetNumberOfCollections() << ").");
    return NULL;
    }
}


//------------------------------------------------------------------------------
bool vtkVegaGroupItem::IsIndexUsed(IndexType index)
{
  return (index < this->GetNumberOfCollections());
}


//------------------------------------------------------------------------------
bool vtkVegaGroupItem::RemoveMarkItemCollection(IndexType index)
{
  if (this->IsIndexUsed(index))
    {
    vtkVegaMarkItemCollection* collection = this->ItemCollections[index];
    if (collection != NULL && collection->Size() == 0)
      {
      collection->Delete();
      vtkVegaGroupItem::Iterator pos = this->ItemCollections.begin();
      pos += index;
      this->ItemCollections.erase(pos);
      this->Modified();
      return true;
      }
    }
  return false;
}


//------------------------------------------------------------------------------
unsigned int vtkVegaGroupItem::GetNumberOfCollections() const
{
  return this->ItemCollections.size();
}


//------------------------------------------------------------------------------
bool vtkVegaGroupItem::Paint(vtkContext2D* painter)
{
  // Render background
  this->Superclass::Paint(painter);

  // Render group contents
  painter->PushMatrix();
  vtkSmartPointer<vtkTransform2D> transform = vtkSmartPointer<vtkTransform2D>::New();
  transform->Translate(this->X, this->Y);
  painter->AppendTransform(transform);

  this->PaintChildren(painter);

  painter->PopMatrix();

  return true;
}


//------------------------------------------------------------------------------
bool vtkVegaGroupItem::PaintChildren(vtkContext2D* painter)
{
  bool result = true;
  unsigned int n = this->GetNumberOfCollections();
  for (unsigned int i = 0; i < n; ++i)
    {
    vtkVegaMarkItemCollection* collection = this->ItemCollections[i];
    if (collection != NULL)
      {
      result = result && collection->Paint(painter);
      }
    }
  return result;
}
