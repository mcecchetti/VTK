/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaMarkItemCollection.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaMarkItemCollection -
//
// .SECTION Description
// vtkVegaMarkItemCollection

#ifndef __vtkVegaMarkItemCollection_h
#define __vtkVegaMarkItemCollection_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkContextItem.h"

#include <map>

class vtkVegaMarkItem;

class VTKCHARTSVEGARENDERER_EXPORT vtkVegaMarkItemCollection : public vtkContextItem
{
public:
  static vtkVegaMarkItemCollection *New();
  vtkTypeMacro(vtkVegaMarkItemCollection, vtkContextItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  typedef unsigned int KeyType;
  typedef unsigned int RefKeyType;
  typedef unsigned int ConstRefKeyType;

public:
  bool SetMarkType(int type);
  vtkGetMacro(MarkType,int);
  bool IsMarkTypeSet();

  vtkVegaMarkItem* GetMarkItem(ConstRefKeyType key);
  bool IsMarkItemPresent(ConstRefKeyType key);
  bool RemoveMarkItem(ConstRefKeyType key);
  void RemoveAllMarkItems();
  void RemoveNotUpdatedMarkItems();
  unsigned int Size() const;

  void FlagMarkItemAsUpdated(ConstRefKeyType key);
  void FlagMarkItemAsNotUpdated(ConstRefKeyType key);
  void FlagAllAsUpdated();
  void FlagAllAsNotUpdated();

  virtual bool Paint(vtkContext2D *painter);

protected:
  typedef vtkVegaMarkItem* ItemPointer;
  typedef bool FlagType;
  typedef std::pair<ItemPointer, FlagType> ValueType;
  typedef std::map<KeyType, ValueType> CollectionType;
  typedef CollectionType::iterator Iterator;
  typedef CollectionType::const_iterator ConstIterator;

  enum
  {
    NOT_UPDATED = false,
    UPDATED = true
  };

protected:
  vtkVegaMarkItemCollection();
  ~vtkVegaMarkItemCollection();


  vtkVegaMarkItem* NewMarkItem();
  Iterator  Find(ConstRefKeyType key);
  RefKeyType GetKey(Iterator pos);
  ItemPointer GetItemPointer(Iterator pos);
  FlagType GetFlagValue(Iterator pos);
  void SetFlagValue(Iterator pos, FlagType value);
  void FlagMarkItemAs(ConstRefKeyType key, FlagType value);
  void FlagAllAs(FlagType value);

protected:
  int MarkType;
  CollectionType Items;

private:
  vtkVegaMarkItemCollection(const vtkVegaMarkItemCollection&);  // Not implemented.
  void operator=(const vtkVegaMarkItemCollection&);  // Not implemented.
};

#endif
