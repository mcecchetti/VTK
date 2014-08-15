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
// .NAME vtkVegaImageLoader - Helper class for loading an image from a given URL.
//
// .SECTION Description
// vtkVegaImageLoader is a specialization of vtkVegaDataLoader and is used for
// loading an image from the resource specified by a given URI.

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

  // Description:
  // Retrieve the image resource specified by the set URI and load it into memory.
  virtual void Update();

  // Description:
  // Return a pointer to the loaded image, or NULL if no image has been loaded.
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
