

#include "vtkVegaSpecReader.h"
#include "vtkSmartPointer.h"

#include <QApplication>

#include <iostream>


#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()



int main( int argc, char * argv [] )
{
  // Qt initialization
  QApplication app(argc, argv);

  VTK_CREATE(vtkVegaSpecReader, reader);

  reader->SetFileName("pippo");

  reader->PrintSelf(std::cout, vtkIndent(2));

  return 0;
}
