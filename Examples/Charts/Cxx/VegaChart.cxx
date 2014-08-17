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
#include <fstream>
#include <sstream>
#include <string>

typedef std::string String;


//------------------------------------------------------------------------------
bool ReadFile(const String& filePath, String& fileContent)
{
  std::ifstream file(filePath.c_str());
  if(!file)
    {
    return false;
    }
  std::stringstream ss;
  ss << file.rdbuf();
  fileContent = ss.str();
  return true;
}


int main(int argc, char *argv[])
{
  if (argc < 2)
    {
    std::cout << "No scene graph argument" << std::endl;
    return EXIT_FAILURE;
    }

  String fileContent;
  if (!ReadFile(argv[1], fileContent))
      {
      std::cout << "Error: Passed file is not accessible." << std::endl;
      return EXIT_FAILURE;
      }

  // Set input scene graph.
  vtkSmartPointer<vtkVegaRenderer> renderer =
      vtkSmartPointer<vtkVegaRenderer>::New();
  renderer->SetInputSceneString(fileContent);

  //std::string scene = renderer->GetInputSceneString();
  //std::cout << scene << std::endl;

  if (argc > 3) // Check for base data URL path
    {
    std::string bSwitch(argv[2]);
    if (bSwitch.compare("-b") == 0)
      {
      // Set base url for data retrieving, e.g. images.
      renderer->SetBaseDataURL(argv[3]);
      }
    }

  //renderer->DebugOn();
  renderer->Update();
  vtkVegaSceneItem* sceneItem = renderer->GetSceneItem();
  //sceneItem->PrintSelf(std::cout, vtkIndent(2));

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
