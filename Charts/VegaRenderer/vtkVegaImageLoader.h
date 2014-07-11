/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaImageLoader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaImageLoader -
//
// .SECTION Description
// vtkVegaImageLoader

#ifndef __vtkVegaImageLoader_h
#define __vtkVegaImageLoader_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkVegaDataLoader.h"
#include "vtkSmartPointer.h"

class vtkImageData;

class VTKCHARTSVEGARENDERER_EXPORT vtkVegaImageLoader : public vtkVegaDataLoader
{
public:
  static vtkVegaImageLoader *New();
  vtkTypeMacro(vtkVegaImageLoader, vtkVegaDataLoader);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Update();


  vtkImageData* GetOutputImage();

protected:
  vtkVegaImageLoader();
  ~vtkVegaImageLoader();

  vtkSmartPointer<vtkImageData> OutputImage;

private:
  vtkVegaImageLoader(const vtkVegaImageLoader& );  // Not implemented.
  void operator=(const vtkVegaImageLoader& );  // Not implemented.
};

#endif
