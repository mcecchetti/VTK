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
// .NAME vtkVegaGroupItem - The class which handles Vega mark item of group type.
//
// .SECTION Description
// vtkVegaGroupItem provides the properties and functionality typical
// of a Vega mark item of group type. It inherits from vtkVegaRectItem.
// That allows to define a rectangle as common background of all mark items
// included into the group. Each mark item, except a group must belongs to
// a group mark item, which may contain one or more mark items.
// The coordinates used for placing any mark item are defined by the x and y
// properties of the item: these coordinates are always relative to the ones of
// the group the mark item belongs to.
// A mark item does not belong directly to a group. Instead a group contains
// one or more collection of mark items where each collection contains mark
// items of one single type. Mark item collection are handled through
// the vtkVegaMarkItemCollection class. vtkVegaGroupItem provides methods
// for adding and removing collections of mark items.

#ifndef __vtkVegaGroupItem_h
#define __vtkVegaGroupItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaRectItem.h"

#include <vector>


class vtkVegaMarkItemCollection;

class VTKCHARTSVEGARENDERER_EXPORT vtkVegaGroupItem : public vtkVegaRectItem
{
public:
  typedef unsigned int IndexType;

public:
  static const int Type = vtkVegaMarkItem::GROUP;

public:
  static vtkVegaGroupItem *New();
  vtkTypeMacro(vtkVegaGroupItem, vtkVegaRectItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // If a collection of mark items is associated with the index argument,
  // a pointer to the collection is returned. If the index argument is
  // not associated with any collection object, a NULL pointer is returned
  // except when the passed index value is equal to the current number of
  // collections present in this group item object. In such a case a new
  // collection object is created, internally, by the method, pushed back
  // into the internal vector and returned.
  vtkVegaMarkItemCollection* GetMarkItemCollection(IndexType index);

  // Description:
  // Return true if the there is a collection object associated with the index
  //argument.
  bool IsIndexUsed(IndexType index);

  // Description:
  // Remove the collection object associated with the index, if any.
  // In order to remove a collection, it must be empty.
  // Return true if such a collection exists and the removing operation is
  // successful.
  bool RemoveMarkItemCollection(IndexType index);

  // Return the number of collection owned by this group.
  unsigned int GetNumberOfCollections() const;

  virtual bool Paint(vtkContext2D *painter);
  virtual bool PaintChildren(vtkContext2D *painter);

protected:
  vtkVegaGroupItem();
  ~vtkVegaGroupItem();

protected:
  typedef vtkVegaMarkItemCollection* Pointer;
  typedef std::vector<Pointer> ArrayType;
  typedef ArrayType::iterator Iterator;

protected:
  ArrayType ItemCollections;

private:
  vtkVegaGroupItem(const vtkVegaGroupItem& );  // Not implemented.
  void operator=(const vtkVegaGroupItem& );  // Not implemented.
};

#endif
