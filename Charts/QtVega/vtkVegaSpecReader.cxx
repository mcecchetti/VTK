
#include "vtkVegaSpecReader.h"

#include "vtkCharArray.h"
#include "vtkObjectFactory.h"




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



