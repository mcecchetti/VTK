/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaSpecReader.cxx

  Copyright (c) Marco Cecchetti
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaSpecReader.h"

#include "vtkCharArray.h"
#include "vtkObjectFactory.h"
#include "vtkVegaScene.h"
#include <vtksys/ios/sstream>

#include <ctype.h>
#include <sys/stat.h>




vtkStandardNewMacro(vtkVegaSpecReader);

vtkCxxSetObjectMacro(vtkVegaSpecReader, InputArray, vtkCharArray);


//------------------------------------------------------------------------------
void vtkVegaSpecReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";

  os << indent << "ReadFromInputString: " << (this->ReadFromInputString ? "On\n" : "Off\n");
  if ( this->InputString )
    {
    os << indent << "Input String: " << this->InputString << "\n";
    }
  else
    {
    os << indent << "Input String: (None)\n";
    }

  if ( this->InputArray )
    {
    os << indent << "Input Array: "  << "\n";
    this->InputArray->PrintSelf(os,indent.GetNextIndent());
    }
  else
    {
    os << indent << "Input String: (None)\n";
    }

  os << indent << "Input String Length: " << this->InputStringLength << endl;
}


//------------------------------------------------------------------------------
vtkVegaSpecReader::vtkVegaSpecReader()
{
  this->FileName = NULL;
  this->IS = NULL;
  this->BaseURL = NULL;
  this->InputString = NULL;
  this->InputStringLength = 0;
  this->ReadFromInputString = 0;
  this->InputArray = 0;
}


//------------------------------------------------------------------------------
vtkVegaSpecReader::~vtkVegaSpecReader()
{
  if (this->FileName)
    {
    delete [] this->FileName;
    }
  if (this->InputString)
    {
    delete [] this->InputString;
    }
  this->SetInputArray(0);
  if ( this->IS )
    {
    delete this->IS;
    }
}


//------------------------------------------------------------------------------
void vtkVegaSpecReader::SetInputString(const char *in)
{
  int len = 0;
  if (in != NULL)
    {
    len = static_cast<int>(strlen(in));
    }
  this->SetInputString(in, len);
}


//------------------------------------------------------------------------------
void vtkVegaSpecReader::SetInputString(const char *in, int len)
{
  if (this->Debug)
    {
    vtkDebugMacro(<< "SetInputString length: " << len
      << " in: " << (in ? in : "(null)"));
    }

  if (this->InputString && in && strncmp(in, this->InputString, len) == 0)
    {
    return;
    }

  if (this->InputString)
    {
    delete [] this->InputString;
    }

  if (in && len>0)
    {
    // Add a NULL terminator so that GetInputString
    // callers (from wrapped languages) get a valid
    // C string in *ALL* cases...
    //
    this->InputString = new char[len+1];
    memcpy(this->InputString,in,len);
    this->InputString[len] = 0;
    this->InputStringLength = len;
    }
   else
    {
    this->InputString = NULL;
    this->InputStringLength = 0;
    }

  this->Modified();
}


//------------------------------------------------------------------------------
int vtkVegaSpecReader::OpenSpecFile()
{
  if ( this->IS != NULL )
    {
    this->CloseSpecFile ();
    }
  if (this->ReadFromInputString)
    {
    if (this->InputArray)
      {
      vtkDebugMacro(<< "Reading from InputArray");
      std::string str(this->InputArray->GetPointer(0),
        static_cast<size_t>( this->InputArray->GetNumberOfTuples()  *
                             this->InputArray->GetNumberOfComponents()) );
      this->IS = new vtksys_ios::istringstream(str);
      return 1;
      }
    else if (this->InputString)
      {
      vtkDebugMacro(<< "Reading from InputString");
      std::string str(this->InputString, this->InputStringLength);
      this->IS = new vtksys_ios::istringstream(str);
      return 1;
      }
    }
  else
    {
    vtkDebugMacro(<< "Opening Vega spec file");

    if ( !this->FileName || (strlen(this->FileName) == 0))
      {
      vtkErrorMacro(<< "No file specified!");
      return 0;
      }

    // first make sure the file exists, this prevents an empty file from
    // being created on older compilers
    struct stat fs;
    if (stat(this->FileName, &fs) != 0)
      {
      vtkErrorMacro(<< "Unable to open file: "<< this->FileName);
      return 0;
      }
    this->IS = new ifstream(this->FileName, ios::in);
    if (this->IS->fail())
      {
      vtkErrorMacro(<< "Unable to open file: "<< this->FileName);
      delete this->IS;
      this->IS = NULL;
      return 0;
      }
    return 1;
    }

  return 0;
}


//------------------------------------------------------------------------------
void vtkVegaSpecReader::CloseSpecFile()
{
  vtkDebugMacro(<<"Closing Vega spec file");
  if ( this->IS != NULL )
    {
    delete this->IS;
    }
  this->IS = NULL;
}


//------------------------------------------------------------------------------
void vtkVegaSpecReader::Update()
{
  if (!this->OpenSpecFile())
    {
    this->CloseSpecFile();
    return;
    }

  if (this->VegaScene.GetPointer() == NULL)
    {
    vtkDebugMacro(<< "Creating VegaScene instance");
    this->VegaScene = vtkSmartPointer<vtkVegaScene>::New();
    if (this->VegaScene.GetPointer() == NULL)
      {
      vtkErrorMacro(<< "Unable to create VegaScene instance");
      return;
      }
    }

  this->VegaScene->SetBaseURL(this->GetBaseURL());
  vtkDebugMacro(<< "Base URL: " << this->VegaScene->GetBaseURL() << "\n");

  if (this->GetReadFromInputString())
    {
    vtksys_ios::istringstream* iss = dynamic_cast<vtksys_ios::istringstream*>(this->IS);
    if (iss == NULL)
      {
      vtkErrorMacro(<< "Unable to read from input string");
      this->CloseSpecFile();
      return;
      }
    this->VegaScene->LoadSpec(iss->str());
    }
  else
    {
    this->VegaScene->LoadSpecFromFile(this->GetFileName());
    }
  this->CloseSpecFile ();

  this->VegaScene->Render();
}


//------------------------------------------------------------------------------
std::string vtkVegaSpecReader::GetSceneString() const
{
  if (this->VegaScene != NULL)
    {
    return this->VegaScene->GetResult();
    }
  else
    {
    return std::string();
    }
}
