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

#include "vtkVegaSpecReader.h"
#include "vtkSmartPointer.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QGuiApplication>

#include <iostream>
#include <string>
#include <vector>

typedef std::string String;

//------------------------------------------------------------------------------
void printUsage()
{
  std::cout
      << "Usage:\n"
      << "  SceneString vega_json_file \n"
      << "  The output scene graph is written to stdout.\n\n"
      << "To load data, you may need to set a base directory:\n"
      << "  For web retrieval, use `-b http://host/data/`. \n"
      << "  For files, use `-b file:///dir/data/` (absolute) or `-b data/` (relative).\n\n"
      << "In order to test input string, use `-is`.\n";
}

//------------------------------------------------------------------------------
bool ReadFile(const String& filePath, String& fileContent)
{
  QFile scriptFile(QString::fromStdString(filePath));
  if (!scriptFile.open(QIODevice::ReadOnly))
    {
    return false;
    }
  QTextStream stream(&scriptFile);
  QString contents = stream.readAll();
  scriptFile.close();
  fileContent = contents.toStdString();
  return true;
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  // for font metrics support
  QGuiApplication app(argc, argv);

  String optionInputString("-is");
  bool useInputString = false;
  String optionBase("-b");
  String baseURL("");

  std::vector<String> argList;

  for (int i = 1; i < argc; ++i)
    {
    String arg(argv[i]);
    if (arg == optionBase)
      {
      ++i;
      if (i < argc)
        {
        baseURL = String(argv[i]);
        }
      else
        {
        printUsage();
        return EXIT_FAILURE;
        }
      }
    else if (arg == optionInputString)
      {
      useInputString = true;
      }
    else
      {
      argList.push_back(arg);
      }
    }

  if (argList.size() < 1 || argList.size() > 2)
    {
    printUsage();
    return EXIT_FAILURE;
    }

  vtkSmartPointer<vtkVegaSpecReader> vsr =
      vtkSmartPointer<vtkVegaSpecReader>::New();

  vsr->SetBaseURL(baseURL.c_str());

  if (useInputString)
    {
    String fileContent;
    ReadFile(argList[0], fileContent);
    vsr->ReadFromInputStringOn();
    vsr->SetInputString(fileContent.c_str());
    }
  else
    {
    vsr->SetFileName(argList[0].c_str());
    }

  vsr->Update();
  std::string sceneString = vsr->GetSceneString();

  std::cout << "Vega Scene Graph: \n" << sceneString << std::endl;

  return 0;
}
