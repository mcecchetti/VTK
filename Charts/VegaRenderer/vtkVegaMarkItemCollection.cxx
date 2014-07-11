/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaMarkItemCollectionCollectionBase.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaMarkItemCollection.h"
#include "vtkObjectFactory.h"
#include "vtkVegaMarkItem.h"
#include "vtkVegaArcItem.h"
#include "vtkVegaAreaItem.h"
#include "vtkVegaGroupItem.h"
#include "vtkVegaImageItem.h"
#include "vtkVegaLineItem.h"
#include "vtkVegaPathItem.h"
#include "vtkVegaRectItem.h"
#include "vtkVegaRuleItem.h"
#include "vtkVegaSymbolItem.h"
#include "vtkVegaTextItem.h"


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaMarkItemCollection);


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  //this->Superclass::PrintSelf(os, indent);
  os << indent << "Mark item type: "
     << vtkVegaMarkItem::GetMarkName(this->MarkType)
     << " (enum: " << this->MarkType << ")" << "\n";

  vtkVegaMarkItemCollection::Iterator pos = this->Items.begin();
  vtkVegaMarkItemCollection::Iterator end = this->Items.end();

  for (; pos != end; ++pos)
    {
    vtkVegaMarkItemCollection::ItemPointer item = this->GetItemPointer(pos);
    if (item != NULL)
      {
      os << indent << "Mark item [" << this->GetKey(pos) << "]: \n";
      item->PrintSelf(os, indent.GetNextIndent());
      }
    }
}


//------------------------------------------------------------------------------
vtkVegaMarkItemCollection::vtkVegaMarkItemCollection()
{
  this->MarkType = vtkVegaMarkItem::NONE;
}


//------------------------------------------------------------------------------
vtkVegaMarkItemCollection::~vtkVegaMarkItemCollection()
{
  this->RemoveAllMarkItems();
}


//------------------------------------------------------------------------------
bool vtkVegaMarkItemCollection::SetMarkType(int type)
{
  if (!vtkVegaMarkItem::IsMarkTypeValid(type) || this->IsMarkTypeSet())
    {
    vtkWarningMacro(<< "Passed a not valid mark type argument: " << type << ".");
    return false;
    }
  else
    {
    this->MarkType = type;
    this->Modified();
    return true;
    }
}


//------------------------------------------------------------------------------
bool vtkVegaMarkItemCollection::IsMarkTypeSet()
{
  return vtkVegaMarkItem::IsMarkTypeValid(this->MarkType);
}


//------------------------------------------------------------------------------
vtkVegaMarkItem* vtkVegaMarkItemCollection::GetMarkItem(ConstRefKeyType key)
{
  vtkVegaMarkItemCollection::Iterator pos = this->Find(key);
  if (pos != this->Items.end())
    {
    vtkVegaMarkItemCollection::ItemPointer item = this->GetItemPointer(pos);
    return item;
    }
  else if (this->IsMarkTypeSet())
    {
    vtkVegaMarkItem* item = this->NewMarkItem();
    if (item == NULL)
      {
      vtkErrorMacro(<< "Failed to create an instance for mark item of type "
                    << vtkVegaMarkItem::GetMarkName(this->MarkType) << ".");
      return NULL;
      }
    ValueType value(item, vtkVegaMarkItemCollection::UPDATED);
    this->Items[key] = value;
    this->Modified();
    return item;
    }
  else
    {
  return NULL;
    }
}


//------------------------------------------------------------------------------
bool vtkVegaMarkItemCollection::RemoveMarkItem(ConstRefKeyType key)
{
  vtkVegaMarkItemCollection::Iterator pos = this->Find(key);
  if (pos != this->Items.end())
    {
    vtkVegaMarkItemCollection::ItemPointer item = this->GetItemPointer(pos);
    if (item != NULL)
      {
      item->Delete();
      }
    this->Items.erase(pos);
    this->Modified();
    return true;
    }
  else
    {
    return false;
    }
}


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::RemoveAllMarkItems()
{
  vtkVegaMarkItemCollection::Iterator pos = this->Items.begin();
  vtkVegaMarkItemCollection::Iterator end = this->Items.end();
  for (; pos != end; ++pos)
    {
    vtkVegaMarkItemCollection::ItemPointer item = this->GetItemPointer(pos);
    if (item != NULL)
      {
      item->Delete();
      }
    }
  this->Items.clear();
  this->Modified();
}


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::RemoveNotUpdatedMarkItems()
{
  bool modified = false;
  vtkVegaMarkItemCollection::Iterator pos = this->Items.begin();
  vtkVegaMarkItemCollection::Iterator end = this->Items.end();
  vtkVegaMarkItemCollection::ItemPointer item;
  for (; pos !=end; ++pos)
    {
    if (this->GetFlagValue(pos) == vtkVegaMarkItemCollection::NOT_UPDATED)
      {
      item = this->GetItemPointer(pos);
      if (item != NULL)
        {
        item->Delete();
        }
      this->Items.erase(pos);
      modified = true;
      }
    }
  if (modified)
    {
    this->Modified();
    }
}


//------------------------------------------------------------------------------
bool vtkVegaMarkItemCollection::IsMarkItemPresent(ConstRefKeyType key)
{
  vtkVegaMarkItemCollection::Iterator pos = this->Find(key);
  return (pos != this->Items.end());
}


//------------------------------------------------------------------------------
unsigned int vtkVegaMarkItemCollection::Size() const
{
  return this->Items.size();
}


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::FlagMarkItemAsUpdated(ConstRefKeyType key)
{
  this->FlagMarkItemAs(key, vtkVegaMarkItemCollection::UPDATED);
}


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::FlagMarkItemAsNotUpdated(ConstRefKeyType key)
{
  this->FlagMarkItemAs(key, vtkVegaMarkItemCollection::NOT_UPDATED);
}


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::FlagAllAsUpdated()
{
  this->FlagAllAs(vtkVegaMarkItemCollection::UPDATED);
}


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::FlagAllAsNotUpdated()
{
  this->FlagAllAs(vtkVegaMarkItemCollection::NOT_UPDATED);
}


//------------------------------------------------------------------------------
bool vtkVegaMarkItemCollection::Paint(vtkContext2D *painter)
{
  bool result = true;
  vtkVegaMarkItemCollection::Iterator pos = this->Items.begin();
  vtkVegaMarkItemCollection::Iterator end = this->Items.end();
  for (; pos != end; ++pos)
    {
    vtkVegaMarkItemCollection::ItemPointer item = this->GetItemPointer(pos);
    if (item != NULL)
      {
      result = result && item->Paint(painter);
      }
    }
  return result;
}


//------------------------------------------------------------------------------
vtkVegaMarkItem* vtkVegaMarkItemCollection::NewMarkItem()
{
  vtkVegaMarkItem* item = NULL;
  switch(this->GetMarkType())
    {
    case vtkVegaMarkItem::ARC:
      {
      item = vtkVegaArcItem::New();
      break;
      }
    case vtkVegaMarkItem::AREA:
      {
      item = vtkVegaAreaItem::New();
      break;
      }
    case vtkVegaMarkItem::GROUP:
      {
      item = vtkVegaGroupItem::New();
      break;
      }
    case vtkVegaMarkItem::IMAGE:
      {
      item = vtkVegaImageItem::New();
      break;
      }
    case vtkVegaMarkItem::LINE:
      {
      item = vtkVegaLineItem::New();
      break;
      }
    case vtkVegaMarkItem::PATH:
      {
      item = vtkVegaPathItem::New();
      break;
      }
    case vtkVegaMarkItem::RECT:
      {
      item = vtkVegaRectItem::New();
      break;
      }
    case vtkVegaMarkItem::RULE:
      {
      item = vtkVegaRuleItem::New();
      break;
      }
    case vtkVegaMarkItem::SYMBOL:
      {
      item = vtkVegaSymbolItem::New();
      break;
      }
    case vtkVegaMarkItem::TEXT:
      {
      item = vtkVegaTextItem::New();
      break;
      }
    default:
      {
      vtkErrorMacro(<< "Unknown mark type: " << this->GetMarkType() << ".");
      }
    }
  return item;
}


//------------------------------------------------------------------------------
vtkVegaMarkItemCollection::Iterator vtkVegaMarkItemCollection::Find(ConstRefKeyType key)
{
  return this->Items.find(key);
}


//------------------------------------------------------------------------------
vtkVegaMarkItemCollection::RefKeyType vtkVegaMarkItemCollection::GetKey(Iterator pos)
{
  return pos->first;
}


//------------------------------------------------------------------------------
vtkVegaMarkItemCollection::ItemPointer vtkVegaMarkItemCollection::GetItemPointer(Iterator pos)
{
  return pos->second.first;
}


//------------------------------------------------------------------------------
vtkVegaMarkItemCollection::FlagType vtkVegaMarkItemCollection::GetFlagValue(Iterator pos)
{
  return pos->second.second;
}


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::SetFlagValue(Iterator pos, FlagType value)
{
  pos->second.second = value;
}


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::FlagMarkItemAs(ConstRefKeyType key, FlagType value)
{
  vtkVegaMarkItemCollection::Iterator pos = this->Find(key);
  if (pos != this->Items.end())
    {
    this->SetFlagValue(pos, value);
    this->Modified();
    }
}


//------------------------------------------------------------------------------
void vtkVegaMarkItemCollection::FlagAllAs(FlagType value)
{
  vtkVegaMarkItemCollection::Iterator pos = this->Items.begin();
  vtkVegaMarkItemCollection::Iterator end = this->Items.end();
  for (; pos != end; ++pos)
    {
    this->SetFlagValue(pos, value);
    }
  this->Modified();
}
