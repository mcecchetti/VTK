/*=========================================================================

 Program:   Visualization Toolkit
 Module:    $RCSfile$

 Copyright (c) Marco Cecchetti
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE. See the above copyright notice for more information.

 =========================================================================*/


#include "vtkSVGPath.h"
#include "vtkVegaSceneItem.h"
#include "vtkVegaMarkItemCollection.h"
#include "vtkVegaPathItem.h"
#include "vtkSmartPointer.h"
#include "vtkPen.h"
#include "vtkBrush.h"
#include "vtkColor.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkContextView.h"
#include "vtkContextScene.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <string>



int main(int argc, char *argv[])
{
  vtkSmartPointer<vtkSVGPath> path = vtkSmartPointer<vtkSVGPath>::New();
  path->AppendCommandsFromString(
      "M 20,10 l 100,100 h 50 v -50 a 200,100 15 1 1 -50,100 Z"
      "M 100,100 l 0,100 l100,-100 l 0,100 Z"
      "M 10,300 q 100,50 150,0 t 200,50"
      "c 150,50 -230,120 150,200 s -200,80 -330,-300 Z");

  std::cout << "path: '" << path->ToString() << "'\n";

  vtkSmartPointer<vtkVegaSceneItem> sceneItem =
      vtkSmartPointer<vtkVegaSceneItem>::New();
  sceneItem->SetWidth(800);
  sceneItem->SetHeight(600);
  sceneItem->GetBrush()->SetColor(255, 255, 255, 255);
  vtkVegaMarkItemCollection* markItems = sceneItem->GetMarkItemCollection(0);
  markItems->SetMarkType(vtkVegaMarkItem::PATH);
  vtkVegaMarkItem* item = markItems->GetMarkItem(0);
  vtkVegaPathItem* pathItem = vtkVegaPathItem::SafeDownCast(item);
  pathItem->GetPen()->SetWidth(1.0);
  pathItem->GetPen()->SetColor(0, 0, 0, 255);
  pathItem->GetBrush()->SetColor(100, 100, 255, 255);
  pathItem->SetPath(path);

  int width = static_cast<int>(sceneItem->GetWidth()) + 30;
  int height = static_cast<int>(sceneItem->GetHeight()) + 30;

  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(width, height);
  view->GetScene()->AddItem(sceneItem);

  // Render the scene and compare the image to a reference image
  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
