/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaPathItem.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaPathItem.h"
#include "vtkObjectFactory.h"
#include "vtkContext2D.h"
#include "vtkTransform2D.h"
#include "vtkPen.h"
#include "vtkBrush.h"
#include "vtkSVGPath.h"
#include "vtkPoints2D.h"
#include "vtkFloatArray.h"

#include "vtkgl.h"
#include "vtkOpenGLError.h"

#include <GL/glu.h>

#include <vector>
#include <list>

#ifndef CALLBACK
#define CALLBACK
#endif


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaPathItem);


//------------------------------------------------------------------------------
void vtkVegaPathItem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkVegaPathItem::vtkVegaPathItem()
{
  this->Path = vtkSmartPointer<vtkSVGPath>::New();
}


//------------------------------------------------------------------------------
vtkVegaPathItem::~vtkVegaPathItem()
{}


//------------------------------------------------------------------------------
void vtkVegaPathItem::SetPath(vtkSVGPath *path)
{
  if (path == NULL)
    {
    return;
    }
  if (this->Path.GetPointer() != path)
    {
    this->Path = path;
    this->Modified();
    }
}


//------------------------------------------------------------------------------
vtkSVGPath* vtkVegaPathItem::GetPath()
{
  return this->Path.GetPointer();
}



//------------------------------------------------------------------------------
void CALLBACK tessBeginCB(GLenum which);
void CALLBACK tessEndCB();
void CALLBACK tessErrorCB(GLenum errorCode);
void CALLBACK tessVertexCB(const GLvoid *data);
void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble *neighborVertex[4],
                            const GLfloat neighborWeight[4], GLdouble **outData);

//------------------------------------------------------------------------------
bool vtkVegaPathItem::Paint(vtkContext2D* painter)
{
  if (this->GetOpacity() == 0 ||
      this->Path.GetPointer() == NULL ||
      this->Path->GetNumberOfCommands() == 0)
    {
    return true;
    }

  painter->PushMatrix();
  vtkSmartPointer<vtkTransform2D> transform = vtkSmartPointer<vtkTransform2D>::New();
  transform->Translate(this->X, this->Y);
  painter->AppendTransform(transform);

  if (this->Brush->GetOpacity() != 0)
    {
    this->ApplyBrush(painter);
    painter->GetPen()->SetOpacityF(0.0);

    typedef std::vector<GLdouble> Vertex3DArrayType;
    typedef std::vector<Vertex3DArrayType> Contour3DArrayType;
    typedef vtkSVGPath::ContourArrayType::iterator Iterator;

    static unsigned int NUM_COMP = 3;

    vtkSVGPath::ContourArrayType contours;
    Contour3DArrayType contours3D;
    vtkPoints2D *vertices;

    GLdouble vertex[2] = {0, 0};
    this->Path->GetContours(contours);
    contours3D.reserve(contours.size());

    for (Iterator pos = contours.begin(), end = contours.end(); pos != end; ++pos)
      {
      vertices = pos->GetPointer();
      if (vertices != NULL)
        {
        unsigned int numOfVertices = vertices->GetNumberOfPoints() - 1;
        if (numOfVertices >= 3)
          {
          double first[2];
          double last[2];
          vertices->GetPoint(0, first);
          vertices->GetPoint(numOfVertices, last);
          if (first[0] == last[0] && first[1] == last[1])
            {
            contours3D.push_back(Vertex3DArrayType());
            Vertex3DArrayType& vertices3D = contours3D.back();
            vertices3D.reserve(NUM_COMP * numOfVertices);
            for (unsigned int i = 0; i < numOfVertices; ++i)
              {
              vertices->GetPoint(i, vertex);
              vertices3D.push_back(vertex[0]);
              vertices3D.push_back(vertex[1]);
              vertices3D.push_back(0);
              }
            }
          }
        }
      }

    vtkOpenGLClearErrorMacro();
    GLUtesselator *tess = gluNewTess(); // create a tessellator
    if(!tess) return false;         // failed to create tessellation object, return 0
    // register callback functions
    gluTessCallback(tess, GLU_TESS_BEGIN, (GLvoid (*)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (GLvoid (*)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid (*)())tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (*)())tessVertexCB);
    gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (*)())tessCombineCB);

    glColor4ubv(this->Brush->GetColor());

    gluTessBeginPolygon(tess, 0);
    unsigned int numOfContours = contours3D.size();
    unsigned int numOfVertices;
    for (unsigned int i = 0; i < numOfContours; ++i)
      {
      gluTessBeginContour(tess);
      Vertex3DArrayType& vertices3D = contours3D[i];
      numOfVertices = vertices3D.size() / NUM_COMP;
      GLdouble* vertex3D = &(vertices3D[0]);
      for (unsigned int j = 0; j < numOfVertices; ++j, vertex3D += NUM_COMP)
        {
        gluTessVertex(tess, vertex3D, vertex3D);
        }
      gluTessEndContour(tess);
      }
    gluTessEndPolygon(tess);

    gluDeleteTess(tess);
    vtkOpenGLCheckErrorMacro("failed after tesselation");
    }

  if (this->IsPenVisible())
    {
    this->ApplyPen(painter);
    painter->GetBrush()->SetOpacityF(0.0);
    vtkPoints2D *vertices;
    vtkSVGPath::ContourArrayType contours;
    this->Path->GetContours(contours);

    typedef vtkSVGPath::ContourArrayType::iterator Iterator;
    for (Iterator pos = contours.begin(), end = contours.end(); pos != end; ++pos)
      {
      vertices = pos->GetPointer();
      if (vertices == NULL)
        {
        vtkErrorMacro(<< "contour[" << (int)(pos - contours.begin()) << "] is NULL");
        }
      else if (vertices->GetNumberOfPoints() == 0)
        {
        vtkWarningMacro(<< "contour[" << (int)(pos - contours.begin()) << "] is empty");
        }
      else if (vertices->GetNumberOfPoints() == 1)
        {
        vtkWarningMacro(<< "contour[" << (int)(pos - contours.begin()) << "] is s single point");
        }
      else
        {
        painter->DrawPoly(vertices);
        }
      }
    }

  painter->PopMatrix();

  return true;
}

//------------------------------------------------------------------------------
// GLU_TESS CALLBACKS


void CALLBACK tessBeginCB(GLenum which)
{
    glBegin(which);
}

void CALLBACK tessEndCB()
{
    glEnd();
}

void CALLBACK tessVertexCB(const GLvoid *data)
{
    const GLdouble *ptr = (const GLdouble*)data;
    glVertex3dv(ptr);
}

void CALLBACK tessErrorCB(GLenum errorCode)
{
    const GLubyte *errorStr;

    errorStr = gluErrorString(errorCode);
    std::cerr << "[ERROR]: " << errorStr << endl;
}

void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble *[4],
                            const GLfloat [4], GLdouble **outData)
{
  typedef std::vector<GLdouble> VertexType;
  static std::list<VertexType> vertices;

  vertices.push_back(VertexType(3));
  VertexType& vertex = vertices.back();
  vertex[0] = newVertex[0];
  vertex[1] = newVertex[1];
  vertex[2] = newVertex[2];

  *outData = &(vertex[0]);
}
