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


#include "vtkVegaRenderer.h"
#include "vtkVegaSceneItem.h"
#include "vtkSmartPointer.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkContextView.h"
#include "vtkContextScene.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <string>

#include <QFile>
#include <QTextStream>
#include <QString>

typedef std::string String;

//------------------------------------------------------------------------------
bool ReadFile(const String& filePath, String& fileContent)
{
  QFile scriptFile(QString::fromStdString(filePath));
  if (!scriptFile.open(QIODevice::ReadOnly))
    {
    return false;
    }
  QTextStream stream(&scriptFile);
  QString contents = stream.readAll();
  scriptFile.close();
  fileContent = contents.toStdString();
  return true;
}



int main(int argc, char *argv[])
{
  String fileContent;
  ReadFile(argv[1], fileContent);

  vtkSmartPointer<vtkVegaRenderer> renderer = vtkSmartPointer<vtkVegaRenderer>::New();
  renderer->SetInputSceneString(fileContent);

  std::string scene = renderer->GetInputSceneString();

  //std::cout << scene << std::endl;

  // Set base url for data retrieving, e.g. images.
  renderer->SetBaseDataURL("file:///opt/shared/work/source_code/vega/vega/examples");

  //renderer->DebugOn();
  renderer->Update();
  vtkVegaSceneItem* sceneItem = renderer->GetSceneItem();
  sceneItem->PrintSelf(std::cout, vtkIndent(2));

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
