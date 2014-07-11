/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaDataLoader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaDataLoader.h"
#include "vtkObjectFactory.h"
#include "vtkStdString.h"

#include <cstring>


//------------------------------------------------------------------------------
const char* vtkVegaDataLoader::FileProtocol = "file://";


//------------------------------------------------------------------------------
void vtkVegaDataLoader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  if (this->InputURI)
    {
    os << indent << "Input URI: '" << this->InputURI << "'\n";
    }
  else
    {
    os << indent << "Input URI:\n";
    }

  if (this->FilePath)
    {
    os << indent << "File path: '" << this->FilePath << "'\n";
    }
  else
    {
    os << indent << "File path:\n";
    }
}


//------------------------------------------------------------------------------
vtkVegaDataLoader::vtkVegaDataLoader()
{
  this->InputURI = NULL;
  this->FilePath = NULL;
}


//------------------------------------------------------------------------------
vtkVegaDataLoader::~vtkVegaDataLoader()
{
  if (this->FilePath)
    {
    delete [] this->FilePath;
    }

  if (this->InputURI)
    {
    delete [] this->InputURI;
    }
}


//------------------------------------------------------------------------------
bool vtkVegaDataLoader::IsFile(const vtkStdString& url)
{
  return (url.compare(vtkVegaDataLoader::FileProtocol) > 0);
}


//------------------------------------------------------------------------------
void vtkVegaDataLoader::Update()
{
  if (this->InputURI == NULL || *(this->InputURI) == '\0')
    {
    return;
    }

  if (this->IsFile(this->InputURI))
    {
    unsigned int len = std::strlen(vtkVegaDataLoader::FileProtocol);
    this->SetFilePath(this->InputURI + len);
    this->Modified();
    }
  else // At present only local data resources are handled.
    {
    vtkErrorMacro(<< "uri: '" << InputURI << "' not supported.");
    }
}
