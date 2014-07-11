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
// .NAME vtkVegaGroupItem -
//
// .SECTION Description
// vtkVegaGroupItem

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

  vtkVegaMarkItemCollection* GetMarkItemCollection(IndexType index);
  bool IsIndexUsed(IndexType index);
  bool RemoveMarkItemCollection(IndexType index);
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
