/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaImageLoader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaImageLoader.h"
#include "vtkObjectFactory.h"
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaImageLoader);


//------------------------------------------------------------------------------
void vtkVegaImageLoader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaImageLoader::vtkVegaImageLoader()
{
}


//------------------------------------------------------------------------------
vtkVegaImageLoader::~vtkVegaImageLoader()
{
}


//------------------------------------------------------------------------------
void vtkVegaImageLoader::Update()
{
  this->Superclass::Update();
  if (this->FilePath == NULL || *(this->FilePath) == '\0')
    {
    return;
    }

  vtkSmartPointer<vtkImageReader2Factory> readerFactory =
     vtkSmartPointer<vtkImageReader2Factory>::New();
   vtkImageReader2 *imageReader = readerFactory->CreateImageReader2(this->FilePath);
   if (imageReader)
     {
     imageReader->SetFileName(this->FilePath);
     imageReader->Update();
     vtkImageData* outimg = imageReader->GetOutput();
     if (outimg)
       {
       this->OutputImage = outimg;
       this->Modified();
       }
     else
       {
       vtkErrorMacro(<< "Cannot read image: '"
                     << this->InputURI << "'");
       }
     imageReader->Delete();
     }
   else
     {
     vtkErrorMacro(<< "No suitable image reader found for: '"
                   << this->InputURI << "'");
     }
}


//------------------------------------------------------------------------------
vtkImageData* vtkVegaImageLoader::GetOutputImage()
{
  return this->OutputImage.GetPointer();
}
