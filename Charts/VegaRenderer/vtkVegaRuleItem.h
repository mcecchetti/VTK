/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaRuleItem.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaRuleItem - The class which handles Vega mark item of rule type.
//
// .SECTION Description
// vtkVegaRuleItem provides the properties and functionality typical
// of a Vega mark item of rule type.

#ifndef __vtkVegaRuleItem_h
#define __vtkVegaRuleItem_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaMarkItem.h"


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaRuleItem : public vtkVegaMarkItem
{
public:
  static const int Type = vtkVegaMarkItem::RULE;

public:
  static vtkVegaRuleItem *New();
  vtkTypeMacro(vtkVegaRuleItem, vtkVegaMarkItem);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool Paint(vtkContext2D *painter);

protected:
  vtkVegaRuleItem();
  ~vtkVegaRuleItem();

  virtual void ApplyBrush(vtkContext2D *painter);

private:
  vtkVegaRuleItem(const vtkVegaRuleItem& );  // Not implemented.
  void operator=(const vtkVegaRuleItem& );  // Not implemented.
};

#endif
