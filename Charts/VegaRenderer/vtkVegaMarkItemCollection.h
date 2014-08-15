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
// .NAME vtkVegaMarkItemCollection - The class which handles a collection of
// Vega mark items of the same type.
//
// .SECTION Description
// vtkVegaMarkItemCollection behaves like a map: at each key is associated
// a Vega mark item. Moreover it handles internally the creation of any new
// Vega mark item which needs to be added to the collection.
// Before an instance of this class can be used a call to SetMarkType must be
// performed for defining which mark item type has to be handled by the new
// object instance. In fact each object of type vtkVegaMarkItemCollection must
// contain mark items of one single type. Once the mark item type is
// initialized it is not allowed to change it anymore. In fact any later call
// to SetMarkType has no effect.
// Each mark item object present in a collection instance has associated
// a boolean field which is used to flag if the mark item has to be regarded
// as updated or not. This feature is useful for supporting dynamic charts.

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

  // Description:
  // Methods for set/get the type of mark items handled by this collection.
  // In fact each object of type vtkVegaMarkItemCollection must
  // contain mark items of one single type. Once the mark item type is
  // initialized it is not allowed to change it anymore. In fact any later call
  // to SetMarkType has no effect.
  // Return true if the passed mark item type is valid.
  bool SetMarkType(int type);
  vtkGetMacro(MarkType,int);
  bool IsMarkTypeSet();

  // Description:
  // If a mark item object is associated with the key argument, a pointer to
  // the collection is returned. If the key argument is not associated with
  // any mark item object, a new mark item object is created and returned.
  // Note that if the mark item type has not yet been set this method returns
  // NULL.
  vtkVegaMarkItem* GetMarkItem(ConstRefKeyType key);

  // Description:
  // Return true if it exists a mark item object associated
  // with the passed key argument.
  bool IsMarkItemPresent(ConstRefKeyType key);

  // Description:
  // Remove the mark item associated with the key argument, if any.
  // If the remove operation is successful it returns true, else it returns
  // false.
  bool RemoveMarkItem(ConstRefKeyType key);

  // Description:
  // Remove all mark items of this collection.
  void RemoveAllMarkItems();

  // Description:
  // Remove all mark items of this collection that are flagged as not updated.
  void RemoveNotUpdatedMarkItems();

  // Description:
  // Return the number of mark items that belong to this collection.
  unsigned int Size() const;

  // Description:
  // Flag the mark items associated to the key argument, if any, as updated.
  void FlagMarkItemAsUpdated(ConstRefKeyType key);

  // Description:
  // Flag the mark items associated to the key argument, if any, as not updated.
  void FlagMarkItemAsNotUpdated(ConstRefKeyType key);

  // Description:
  // Flag all mark items of this collection as updated.
  void FlagAllAsUpdated();

  // Description:
  // Flag all mark items of this collection as not updated.
  void FlagAllAsNotUpdated();

  // Description:
  // Invokes the Paint method on all mark items included in this collection.
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
