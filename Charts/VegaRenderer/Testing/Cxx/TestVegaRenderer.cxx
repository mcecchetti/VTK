/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestVegaRenderer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"


#include "vtkVegaRenderer.h"
#include "vtkVegaSceneItem.h"
#include "vtkSmartPointer.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkContextView.h"
#include "vtkContextScene.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkSmartPointer.h"

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

//------------------------------------------------------------------------------
int TestVegaRenderer(int argc, char *argv[])
{
  if (argc < 2)
    {
    vtkGenericWarningMacro(<< "Too few arguments.\n");
    return 1;
    }

  String fileContent;
  if (!ReadFile(argv[1], fileContent))
      {
      vtkGenericWarningMacro(<< "Error: Passed file is not accessible.\n");
      return 1;
      }
  vtkGenericWarningMacro("example file: " << argv[1] << "\n");

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

  int width = static_cast<int>(sceneItem->GetWidth());
  int height = static_cast<int>(sceneItem->GetHeight());

  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(width, height);
  view->GetScene()->AddItem(sceneItem);


  // Render the scene and compare the image to a reference image
  view->GetRenderWindow()->SetMultiSamples(0);
  //view->GetInteractor()->Initialize();


  vtkRenderWindow *renWin = view->GetRenderWindow();
  renWin->Render();
  int retVal = vtkRegressionTestImageThreshold( renWin, 15);
  if ( retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    view->GetInteractor()->Start();
    }
  return !retVal;
}
