/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestSVGPath.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkSVGPath.h"
#include "vtkSmartPointer.h"
#include "vtkTestDriver.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>




bool TestAppendCommandsFromString()
{
  bool testResult = true;

  static const char*  DataSet1[] = {
      "Z", "H 25", "V 3.78",
      "M 20,50.2", "L 25.527,32", "T -3,-1256",
      "Q 0.5,77 0,0", "S -0.7,7126 900,12",
      "C 11,-14 12.7,0 0.76,74", "A 12.11,100 -12 0 1 12,98",
      "h 25", "v 3.78",
      "m 20,50.2", "l 25.527,32", "t -3,-1256",
      "q 0.5,77 0,0", "s -0.7,7126 900,12",
      "c 11,-14 12.7,0 0.76,74", "a 12.11,100 -12 0 1 12,98",
      "Z H 25 V 3.78 M 20,50.2 L 25.527,32 T -3,-1256 Q 0.5,77 0,0 "
      "S -0.7,7126 900,12 C 11,-14 12.7,0 0.76,74 A 12.11,100 -12 0 1 12,98",
      "h 25 v 3.78 m 20,50.2 l 25.527,32 t -3,-1256 q 0.5,77 0,0 "
      "s -0.7,7126 900,12 c 11,-14 12.7,0 0.76,74 a 12.11,100 -12 0 1 12,98",
      "Z H 25 V 3.78 M 20,50.2 L 25.527,32 T -3,-1256 Q 0.5,77 0,0 "
      "s -0.7,7126 900,12 c 11,-14 12.7,0 0.76,74 a 12.11,100 -12 0 1 12,98",
      "\0"
  };

  static const char*  DataSet2[] = {
      "z", "Z",
      "H  25", "H 25",
      "M20,50.2", "M 20,50.2",
      " M 20,50.2", "M 20,50.2",
      "M 20,50.2 ", "M 20,50.2",
      "L  25.527,32", "L 25.527,32",
      "T -3 ,-1256", "T -3,-1256",
      "T -3, -1256", "T -3,-1256",
      "Q 0.5,77  0,0", "Q 0.5,77 0,0",
      " S  -0.7 , 7126  900 , 12 ", "S -0.7,7126 900,12",
      "C 11, -14 12.7, 0  0.76, 74", "C 11,-14 12.7,0 0.76,74",
      "A 12.11, 100  -12  0  1  12, 98", "A 12.11,100 -12 0 1 12,98",
      "ZH 25V 3.78M 20,50.2L 25.527,32T -3,-1256Q 0.5,77 0,0"
      "S -0.7,7126 900,12C 11,-14 12.7,0 0.76,74A 12.11,100 -12 0 1 12,98",
      "Z H 25 V 3.78 M 20,50.2 L 25.527,32 T -3,-1256 Q 0.5,77 0,0 "
      "S -0.7,7126 900,12 C 11,-14 12.7,0 0.76,74 A 12.11,100 -12 0 1 12,98",
      "Z  H 25 V 3.78  M 20,50.2  L 25.527,32  T -3,-1256  Q 0.5,77 0,0  "
      "S -0.7,7126 900,12  C 11,-14 12.7,0 0.76,74  A 12.11,100 -12 0 1 12,98",
      "Z H 25 V 3.78 M 20,50.2 L 25.527,32 T -3,-1256 Q 0.5,77 0,0 "
      "S -0.7,7126 900,12 C 11,-14 12.7,0 0.76,74 A 12.11,100 -12 0 1 12,98",
      "\0"
  };

  static const char*  DataSet3[] = {
      "", "20,50.2", "G 25.527,32", "T ,-1256",
      "M 20 -50.2", "Z 20,50.2", "H 20,50.2", "L 20",
      "Q ,77,0,0", "S -0.7,7126, 900,12",
      "C 11, -14 12.7,0 0.76-74", "A 12.11,100 -12 0 8 12,98",
      "A 12.11,100 -12 0 1.0 12,98", "A 12.11,100 -12 0.0 1 12,98",
      "A 12.11,100 -12 -4 1 12,98", "A 12.11,100 -12.8 0 1 12,98",
      "\0"
  };


  vtkSmartPointer<vtkSVGPath> path = vtkSmartPointer<vtkSVGPath>::New();
  std::string input, output, expected;

  unsigned int i = 0;
  while (true)
    {
    input = std::string(DataSet1[i++]);
    if (input[0] == '\0')
      {
      break;
      }
    expected = input;
    path->AppendCommandsFromString(input);
    output = path->ToString();
    if (output.compare(expected) != 0)
      {
      testResult = false;
      vtkGenericWarningMacro(
        << "Fail: input '" << input  << "'"
        << ", found '" << output
        << "', expected '" << expected << "' instead."
        );
      }
    path->RemoveAllCommands();
    }

  i = 0;
  while (true)
    {
    input = std::string(DataSet2[i++]);
    if (input[0] == '\0')
      {
      break;
      }
    expected = std::string(DataSet2[i++]);
    path->AppendCommandsFromString(input);
    output = path->ToString();
    if (output.compare(expected) != 0)
      {
      testResult = false;
      vtkGenericWarningMacro(
        << "Fail: input '" << input  << "'"
        << ", found '" << output
        << "', expected '" << expected << "' instead."
        );
      }
    path->RemoveAllCommands();
    }

  i = 0;
  expected = std::string("");
  while (true)
    {
    input = std::string(DataSet3[i++]);
    if (input[0] == '\0')
      {
      break;
      }
    path->AppendCommandsFromString(input);
    output = path->ToString();
    if (output.compare(expected) != 0)
      {
      testResult = false;
      vtkGenericWarningMacro(
        << "Fail: input '" << input  << "'"
        << ", found '" << output
        << "', expected '" << expected << "' instead."
        );
      }
    path->RemoveAllCommands();
    }

  return testResult;
}


//-----------------------------------------------------------------------------
bool TestSimplify()
{
  bool testResult = true;
  static const char*  DataSet1[] = {
      "M 100,100 l 100,0 m 20,20 m 30,50 m 40,10 l 0,100",
      "M 100,100 l 100,0 m 90,80 l 0,100",
      "M 100,100 l 100,0 M 20,20 m 30,50 l 0,100",
      "M 100,100 l 100,0 M 50,70 l 0,100",
      "M 100,100 l 100,0 M 20,20 M 30,50 m 30,50 M 80,90 l 0,100",
      "M 100,100 l 100,0 M 80,90 l 0,100",
      "M 100,100 l 100,0 Z Z l 0,100 Z l 0,100",
      "M 100,100 l 100,0 Z l 0,100 Z l 0,100",
      "M 100,100 L 100,0 M 100,0 L 200,100 m 0,0 l 0,100 m 0,100 l 0,100 M 0,100 l 0,100",
      "M 100,100 L 100,0 L 200,100 l 0,100 m 0,100 l 0,100 M 0,100 l 0,100",
      "M 100,100 M 20,20 m 30,50 m 60,80 M 0,100 m 30,50 l 100,0 M 20,20 m 30,50 m 60,80 M 0,100 m 30,50",
      "M 30,150 l 100,0",
      "M 100,100 l 100,0 m 20,20 M 30,50 l 0,100 Z Z",
      "M 100,100 l 100,0 M 30,50 l 0,100 Z",
      "\0"
  };

  vtkSmartPointer<vtkSVGPath> path = vtkSmartPointer<vtkSVGPath>::New();
  std::string input, output, expected;

  unsigned int i = 0;
  while (true)
    {
    input = std::string(DataSet1[i++]);
    if (input[0] == '\0')
      {
      break;
      }
    expected = std::string(DataSet1[i++]);
    path->AppendCommandsFromString(input);
    path->Simplify();
    output = path->ToString();
    if (output.compare(expected) != 0)
      {
      testResult = false;
      vtkGenericWarningMacro(
        << "Fail: input '" << input  << "'"
        << ", found '" << output
        << "', expected '" << expected << "' instead."
        );
      }
    path->RemoveAllCommands();
    }

  return testResult;
}


//-----------------------------------------------------------------------------
bool TestNumberOfSubPaths()
{
  bool testResult = true;
  struct DataType
  {
    const char* input;
    unsigned int expected;
  };

  static const DataType  DataSet1[] = {
      {"M 100,100 l 100,0", 1},
      {"M 100,100 l 100,0 M 100,100 l 100,0", 2},
      {"M 100,100 l 100,0 Z", 1},
      {"Z", 1},
      {"M 100,100", 0},
      {"l 100,0", 1},
      {"M 100,100 l 100,0 Z M 100,100 l 100,0 Z", 2},
      {"M 100,100 l 100,0 M 100,100 l 100,0 Z l 100,0 Z", 3},
      {"M 100,100 l 100,0 M 100,100 Z l 100,0 Z", 3},
      {"\0", 0}
  };


  vtkSmartPointer<vtkSVGPath> path = vtkSmartPointer<vtkSVGPath>::New();
  std::string input;
  unsigned int output, expected;

  unsigned int i = 0;
  while (true)
    {
    input = std::string(DataSet1[i].input);
    if (input[0] == '\0')
      {
      break;
      }
    expected = DataSet1[i++].expected;
    path->AppendCommandsFromString(input);
    path->Simplify();
    output = path->GetNumberOfSubPaths();
    if (output != expected)
      {
      testResult = false;
      vtkGenericWarningMacro(
        << "Fail: input '" << input  << "'"
        << ", found '" << output
        << "', expected '" << expected << "' instead."
        );
      }
    path->RemoveAllCommands();
    }

  return testResult;
}


//-----------------------------------------------------------------------------
bool TestNumberOfContours()
{
  bool testResult = true;
  struct DataType
  {
    const char* input;
    unsigned int expected;
  };

  static const DataType  DataSet1[] = {
      {"M 100,100 l 100,0", 1},
      {"M 100,100 l 100,0 M 100,100 l 100,0", 2},
      {"M 100,100 l 100,0 Z", 1},
      {"Z", 1},
      {"M 100,100", 0},
      {"l 100,0", 1},
      {"M 100,100 l 100,0 Z M 100,100 l 100,0 Z", 2},
      {"M 100,100 l 100,0 M 100,100 l 100,0 Z l 100,0 Z", 3},
      {"M 100,100 l 100,0 M 100,100 Z l 100,0 Z", 3},
      {"\0", 0}
  };


  vtkSmartPointer<vtkSVGPath> path = vtkSmartPointer<vtkSVGPath>::New();
  vtkSVGPath::ContourArrayType contours;
  std::string input;
  unsigned int output, expected;

  unsigned int i = 0;
  while (true)
    {
    input = std::string(DataSet1[i].input);
    if (input[0] == '\0')
      {
      break;
      }
    expected = DataSet1[i++].expected;
    path->AppendCommandsFromString(input);
    path->Simplify();
    path->GetContours(contours);
    output = contours.size();
    if (output != expected)
      {
      testResult = false;
      vtkGenericWarningMacro(
        << "Fail: input '" << input  << "'"
        << ", found '" << output
        << "', expected '" << expected << "' instead."
        );
      }
    path->RemoveAllCommands();
    contours.clear();
    }

  return testResult;
}


//-----------------------------------------------------------------------------
int TestSVGPath(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  bool testResult = true;

  testResult &= TestAppendCommandsFromString();
  if ( !testResult )
    {
    vtkGenericWarningMacro(
      << "Fail: TestAppendCommandsFromString()"
      );
    }

  testResult &= TestSimplify();
  if ( !testResult )
    {
    vtkGenericWarningMacro(
      << "Fail: TestSimplify()"
      );
    }

  testResult &= TestNumberOfSubPaths();
  if ( !testResult )
    {
    vtkGenericWarningMacro(
      << "Fail: TestNumberOfSubPaths()"
      );
    }

  testResult &= TestNumberOfContours();
  if ( !testResult )
    {
    vtkGenericWarningMacro(
      << "Fail: TestNumberOfContours()"
      );
    }

  if ( !testResult )
    {
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;

}
