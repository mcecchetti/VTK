/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegascene.cxx

  Copyright (c) Marco Cecchetti
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaScene.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include <iostream>




vtkStandardNewMacro(vtkVegaScene);


//------------------------------------------------------------------------------
void vtkVegaScene::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//------------------------------------------------------------------------------
vtkVegaScene::vtkVegaScene()
{}


//------------------------------------------------------------------------------
vtkVegaScene::~vtkVegaScene()
{}




//------------------------------------------------------------------------------
int vtkVegascene(const String& specFilePath,
                 const String& outFilePath,
                 const String& baseURL)
{
  vtkSmartPointer<vtkVegaScene> vs = vtkSmartPointer<vtkVegaScene>::New();

  vs->SetBaseURL(baseURL);
#ifdef DEBUG
  std::cout << "Base URL: " << vs->GetBaseURL() << std::endl;
#endif
  vs->LoadSpecFromFile(specFilePath);
  vs->Render();
  return !(vs->Write(outFilePath));
}
