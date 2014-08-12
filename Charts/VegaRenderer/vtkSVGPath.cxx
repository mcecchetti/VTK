/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSVGPath.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkSVGPath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints2D.h"
#include "vtkMath.h"

#include <ctype.h>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>


//------------------------------------------------------------------------------
static const unsigned int X = 0, Y = 1;


//------------------------------------------------------------------------------
// Helper function for translating a 2D point.
inline
static void TranslatePoint(float_type P[2], float_type V[2])
{
    P[X] += V[X];
    P[Y] += V[Y];
}


//------------------------------------------------------------------------------
// Helper function for translating a 2D point.
inline
static void TranslatePoint(float_type P[2], float_type x, float_type y)
{
    P[X] += x;
    P[Y] += y;
}


//------------------------------------------------------------------------------
// This class is used for storing the control points of a cubic Bezier curve.
// Moreover it provides a method which computes a polyline approximating
// the curve.
class vtkCubicBezierCurve
{
public:
  // Split this curves into two new cubic Bezier curves. Each new curve
  // represents half of this curve.
  void Split(vtkCubicBezierCurve& firstHalf, vtkCubicBezierCurve& secondHalf);

  // Trough the `vertices` parameter this method returns an array of 2D points
  // representing the vertices of a polyline which approximates this curve.
  void GetVertices(vtkPoints2D* vertices, double flatnessThreshold = 0.5);

  // Return a string which contains the value of the 4 control points
  // in a pretty format.
  std::string ToString();

public:
  float_type P0[2];
  float_type P1[2];
  float_type P2[2];
  float_type P3[2];
};


//------------------------------------------------------------------------------
// This implementation is an adaptation of the one used by the Qt library.
void vtkCubicBezierCurve::Split(vtkCubicBezierCurve& firstHalf,
                                vtkCubicBezierCurve& secondHalf)
{
  static const double t = 0.5;

  float_type c = (P1[X] + P2[X]) * t;
  firstHalf.P1[X] = (P0[X] + P1[X]) * t;
  secondHalf.P2[X] = (P2[X] + P3[X]) * t;
  firstHalf.P0[X] = P0[X];
  secondHalf.P3[X] = P3[X];
  firstHalf.P2[X] = (firstHalf.P1[X] + c) * t;
  secondHalf.P1[X] = (secondHalf.P2[X] + c) * t;
  firstHalf.P3[X] = secondHalf.P0[X] = (firstHalf.P2[X] + secondHalf.P1[X]) * t;

  c = (P1[Y] + P2[Y]) * t;
  firstHalf.P1[Y] = (P0[Y] + P1[Y]) * t;
  secondHalf.P2[Y] = (P2[Y] + P3[Y]) * t;
  firstHalf.P0[Y] = P0[Y];
  secondHalf.P3[Y] = P3[Y];
  firstHalf.P2[Y] = (firstHalf.P1[Y] + c) * t;
  secondHalf.P1[Y] = (secondHalf.P2[Y] + c) * t;
  firstHalf.P3[Y] = secondHalf.P0[Y] = (firstHalf.P2[Y] + secondHalf.P1[Y]) * t;
}


//------------------------------------------------------------------------------
// This implementation is an adaptation of the one used by the Qt library.
void vtkCubicBezierCurve::GetVertices(vtkPoints2D* vertices,
                                      double flatnessThreshold)
{
  using std::fabs;

  vtkCubicBezierCurve beziers[10];
  int levels[10];
  beziers[0] = *this;
  levels[0] = 9;
  vtkCubicBezierCurve* b = beziers;
  int *lvl = levels;

  while (b >= beziers)
    {
    // check if we can pop the top bezier curve from the stack
    float_type V[2];
    V[X] = b->P3[X] - b->P0[X];
    V[Y] = b->P3[Y] - b->P0[Y];
    float_type L = fabs(V[X]) + fabs(V[Y]);
    float_type d;

    if (L > 1.0)
      {
      d = fabs(V[X] * (b->P0[Y] - b->P1[Y]) - V[Y] * (b->P0[X] - b->P1[X])) +
          fabs(V[X] * (b->P0[Y] - b->P2[Y]) - V[Y] * (b->P0[X] - b->P2[X]));
      }
    else
      {
      d = fabs(b->P0[Y] - b->P1[Y]) + fabs(b->P0[X] - b->P1[X]) +
          fabs(b->P0[Y] - b->P2[Y]) + fabs(b->P0[X] - b->P2[X]);
      L = 1.0;
      }
    if (d < flatnessThreshold || *lvl == 0)
      {
      vertices->InsertNextPoint(b->P3);
      --b;
      --lvl;
      }
    else
      {
      b->Split(*(b+1), *b);
      lvl[1] = --lvl[0];
      ++b;
      ++lvl;
      }
    }
}


//------------------------------------------------------------------------------
std::string vtkCubicBezierCurve::ToString()
{
  std::stringstream ss;
  ss << "[(" << P0[X] << "," << P0[Y] << "), "
     << "(" << P1[X] << "," << P1[Y] << "), "
     << "(" << P2[X] << "," << P2[Y] << "), "
     << "(" << P3[X] << "," << P3[Y] << ")]";

  return ss.str();
}


//------------------------------------------------------------------------------
void vtkSVGPathCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Command type: " << this->GetType() << "\n";
  os << indent << "Is absolute command: " << std::boolalpha
     << this->UseAbsoluteCoordinate << "\n";
  os << indent << "End Point: (" << this->EndPoint[0]
     << ", " << this->EndPoint[1] << ")\n";
}


//------------------------------------------------------------------------------
vtkSVGPathCommand::vtkSVGPathCommand()
{
  this->Context = NULL;
  this->UseAbsoluteCoordinate = true;
  this->EndPoint[0] = 0;
  this->EndPoint[1] = 0;
}


//------------------------------------------------------------------------------
vtkSVGPathCommand::~vtkSVGPathCommand()
{}


//------------------------------------------------------------------------------
bool operator==(vtkSVGPathCommand& left, vtkSVGPathCommand& right)
{
  return (left.ToString(8).compare(right.ToString(8)) == 0);
}


//------------------------------------------------------------------------------
vtkSVGPathCommand* vtkSVGPathCommand::NewCommand(char t)
{
  bool isRelCommand = std::islower(t);
  t = std::toupper(t);

  vtkSVGPathCommand* newCommand;
  switch (t)
    {
    case 'M':
      newCommand = vtkSVGPathMoveTo::New();
      break;
    case 'L':
      newCommand = vtkSVGPathLineTo::New();
      break;
    case 'H':
      newCommand = vtkSVGPathHLineTo::New();
      break;
    case 'V':
      newCommand = vtkSVGPathVLineTo::New();
      break;
    case 'A':
      newCommand = vtkSVGPathArcTo::New();
      break;
    case 'Q':
      newCommand = vtkSVGPathQuadraticCurveTo::New();
      break;
    case 'T':
      newCommand = vtkSVGPathSmoothQuadraticCurveTo::New();
      break;
    case 'C':
      newCommand = vtkSVGPathCubicCurveTo::New();
      break;
    case 'S':
      newCommand = vtkSVGPathSmoothCubicCurveTo::New();
      break;
    case 'Z':
      return vtkSVGPathClosePath::New();
      break;
    default:
      return NULL;
    }
  if (newCommand == NULL)
    {
    std::cerr << "SVG Path command creation failed. Requested command type: '"
        << t << "'\n";
    return NULL;
    }
  if (isRelCommand)
    {
    newCommand->UseAbsoluteCoordinateOff();
    }
  return newCommand;
}


//------------------------------------------------------------------------------
vtkSVGPathCommand* vtkSVGPathCommand::DeepCopy()
{
  vtkSVGPathCommand *newCommand =
      vtkSVGPathCommand::SafeDownCast(this->NewInstanceInternal());
  if (newCommand != NULL)
    {
    newCommand->SetContext(this->Context);
    newCommand->SetUseAbsoluteCoordinate(this->UseAbsoluteCoordinate);
    newCommand->SetEndPoint(this->EndPoint);
    }
  else
    {
    vtkErrorMacro(<< "Cannot create a new SVG path command instance.\n");
    }
  return newCommand;
}


//------------------------------------------------------------------------------
void vtkSVGPathCommand::SetEndPoint(float_type x, float_type y)
{
  if (this->EndPoint[0] != x || this->EndPoint[1] != y)
    {
    this->EndPoint[0] = x;
    this->EndPoint[1] = y;
    this->Modified();
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathCommand::SetEndPoint(float_type point[2])
{
  if (point != NULL)
    {
    this->SetEndPoint(point[0], point[1]);
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathCommand::GetEndPoint(float_type point[2])
{
  if (point != NULL)
    {
    point[0] = this->EndPoint[0];
    point[1] = this->EndPoint[1];
    }
}


//------------------------------------------------------------------------------
float_type* vtkSVGPathCommand::ToAbsoluteCommand(float_type startPoint[2])
{
  if (this->IsAbsoluteCommand())
    {
    return NULL;
    }
  float_type *sp = NULL;
  if (startPoint != NULL)
    {
    sp = startPoint;
    }
  else if (this->HasContext())
    {
    sp = this->Context->CurrentEndPoint;
    }
  else
    {
    vtkErrorMacro(<< "It is not possible to get the absolute command.\n");
    return NULL;
    }
  TranslatePoint(this->EndPoint, sp);
  this->UseAbsoluteCoordinateOn();
  this->Modified();
  return sp;
}


//------------------------------------------------------------------------------
void vtkSVGPathCommand::GetVertices(vtkPoints2D* vertices)
{
  if (vertices == NULL || this->Context == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get vertices.\n");
    return;
    }

  this->Context->LastCommandType = this->GetType();

  vtkSmartPointer<vtkSVGPathCommand> absoluteCommand;
  if (this->IsAbsoluteCommand())
    {
    absoluteCommand = this;
    }
  else
    {
    absoluteCommand = this->DeepCopy();
    if (absoluteCommand.GetPointer() != NULL)
      {
      absoluteCommand->Delete();
      absoluteCommand->ToAbsoluteCommand();
      }
    else
      {
      vtkErrorMacro(<< "Deep copy failed.\n");
      }
    }

  if (absoluteCommand.GetPointer() != NULL)
    {
    vertices->InsertNextPoint(absoluteCommand->EndPoint);
    this->Context->SetCurrentEndPoint(absoluteCommand->EndPoint);
    }
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathMoveTo);


//------------------------------------------------------------------------------
void vtkSVGPathMoveTo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkSVGPathMoveTo::vtkSVGPathMoveTo()
{
}


//------------------------------------------------------------------------------
vtkSVGPathMoveTo::~vtkSVGPathMoveTo()
{}


//------------------------------------------------------------------------------
vtkSVGPathMoveTo* vtkSVGPathMoveTo::DeepCopy()
{
  return vtkSVGPathMoveTo::SafeDownCast(this->Superclass::DeepCopy());
}


//------------------------------------------------------------------------------
std::string vtkSVGPathMoveTo::ToString(int precision)
{
  std::stringstream ss;
  if (precision >= 0)
    {
    ss.precision(precision);
    }
  char label = (this->IsAbsoluteCommand()) ? 'M' : 'm';
  ss << label << ' ' << this->EndPoint[X] << ',' << this->EndPoint[Y];
  return ss.str();
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathLineTo);


//------------------------------------------------------------------------------
void vtkSVGPathLineTo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkSVGPathLineTo::vtkSVGPathLineTo()
{
}


//------------------------------------------------------------------------------
vtkSVGPathLineTo::~vtkSVGPathLineTo()
{}


//------------------------------------------------------------------------------
vtkSVGPathLineTo* vtkSVGPathLineTo::DeepCopy()
{
  return vtkSVGPathLineTo::SafeDownCast(this->Superclass::DeepCopy());
}


//------------------------------------------------------------------------------
// Create a new `lineTo` command which is the absolute version of this command.
// In order to work correctly it needs that the starting point of the path
// segment defined by this command is passed or that the path context has
// been set. The orientation parameter is used for discriminating between
// horizontal (0), vertical (1) and generic `lineTo` command.
// The method returns a pointer to the new created command.
// The deletion of the returned `lineTo` command instance is up to the
// method caller.
vtkSVGPathLineTo* vtkSVGPathLineTo::GetAbsoluteLineToImpl(int orientation,
                                                          float_type startPoint[2])
{
  float_type *sp = NULL;
  if (startPoint != NULL)
    {
    sp = startPoint;
    }
  else if (this->HasContext())
    {
    sp = this->Context->CurrentEndPoint;
    }
  else
    {
    vtkErrorMacro(<< "It is not possible to get the absolute `lineTo` command.\n");
    return NULL;
    }

  vtkSVGPathLineTo* lineTo = vtkSVGPathLineTo::New();
  lineTo->UseAbsoluteCoordinateOn();
  float_type endPoint[2];
  this->GetEndPoint(endPoint);
  if (this->IsRelativeCommand())
    {
    TranslatePoint(endPoint, sp);
    }
  if (orientation == X)  // Horizontal `lineTo`
    {
    endPoint[Y] = sp[Y];
    }
  else if (orientation == Y)  // Vertical `lineTo`
    {
    endPoint[X] = sp[X];
    }
  lineTo->SetEndPoint(endPoint);

  return lineTo;
}


//------------------------------------------------------------------------------
std::string vtkSVGPathLineTo::ToString(int precision)
{
  std::stringstream ss;
  if (precision >= 0)
    {
    ss.precision(precision);
    }
  char label = (this->IsAbsoluteCommand()) ? 'L' : 'l';
  ss << label << ' ' << this->EndPoint[X] << ',' << this->EndPoint[Y];
  return ss.str();
}


//------------------------------------------------------------------------------
void vtkSVGPathLineTo::GetVertices(vtkPoints2D* vertices)
{
  if (vertices == NULL || this->Context == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get vertices.\n");
    return;
    }

  this->Context->LastCommandType = this->GetType();

  vtkSmartPointer<vtkSVGPathLineTo> lineToCommand = this->GetAbsoluteLineTo();
  if (lineToCommand.GetPointer() != NULL)
    {
    lineToCommand->Delete();
    vertices->InsertNextPoint(lineToCommand->EndPoint);
    this->Context->SetCurrentEndPoint(lineToCommand->EndPoint);
    }
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathHLineTo);


//------------------------------------------------------------------------------
void vtkSVGPathHLineTo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkSVGPathHLineTo::vtkSVGPathHLineTo()
{
}


//------------------------------------------------------------------------------
vtkSVGPathHLineTo::~vtkSVGPathHLineTo()
{}


//------------------------------------------------------------------------------
vtkSVGPathHLineTo* vtkSVGPathHLineTo::DeepCopy()
{
  return vtkSVGPathHLineTo::SafeDownCast(this->Superclass::DeepCopy());
}


//------------------------------------------------------------------------------
vtkSVGPathLineTo* vtkSVGPathHLineTo::GetAbsoluteLineTo(float_type startPoint[2])
{
  this->EndPoint[Y] = 0;
  vtkSVGPathLineTo* lineToCommand =
      this->Superclass::GetAbsoluteLineToImpl(X, startPoint);
  return lineToCommand;
}


//------------------------------------------------------------------------------
std::string vtkSVGPathHLineTo::ToString(int precision)
{
  std::stringstream ss;
  if (precision >= 0)
    {
    ss.precision(precision);
    }
  char label = (this->IsAbsoluteCommand()) ? 'H' : 'h';
  ss << label << ' ' << this->EndPoint[X];
  return ss.str();
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathVLineTo);


//------------------------------------------------------------------------------
void vtkSVGPathVLineTo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkSVGPathVLineTo::vtkSVGPathVLineTo()
{
}


//------------------------------------------------------------------------------
vtkSVGPathVLineTo::~vtkSVGPathVLineTo()
{}


//------------------------------------------------------------------------------
vtkSVGPathVLineTo* vtkSVGPathVLineTo::DeepCopy()
{
  return vtkSVGPathVLineTo::SafeDownCast(this->Superclass::DeepCopy());
}


//------------------------------------------------------------------------------
vtkSVGPathLineTo* vtkSVGPathVLineTo::GetAbsoluteLineTo(float_type startPoint[2])
{
  this->EndPoint[X] = 0;
  vtkSVGPathLineTo* lineToCommand =
      this->Superclass::GetAbsoluteLineToImpl(Y, startPoint);
  return lineToCommand;
}


//------------------------------------------------------------------------------
std::string vtkSVGPathVLineTo::ToString(int precision)
{
  std::stringstream ss;
  if (precision >= 0)
    {
    ss.precision(precision);
    }
  char label = (this->IsAbsoluteCommand()) ? 'V' : 'v';
  ss << label << ' ' << this->EndPoint[Y];
  return ss.str();
}


//------------------------------------------------------------------------------
// This cache is used for avoiding to compute the set of Bezier
// curves used for approximating a given arc, again.
vtkSVGPathArcTo::ArcToBezierCurvesCacheType
vtkSVGPathArcTo::ArcToBezierCurvesCache =
    vtkSVGPathArcTo::ArcToBezierCurvesCacheType();


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathArcTo);


//------------------------------------------------------------------------------
void vtkSVGPathArcTo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Ray X: " << this->RayX << ", Ray Y: " << this->RayY << "\n";
  os << indent << "X-axis rotation: " << this->XAxisRotation << "\n";
  os << indent << "Large arc flag: " << std::boolalpha << this->LargeArcFlag << "\n";
  os << indent << "Sweep flag: " << std::boolalpha << this->SweepFlag << "\n";
}


//------------------------------------------------------------------------------
vtkSVGPathArcTo::vtkSVGPathArcTo()
{
  this->RayX = 0;
  this->RayY = 0;
  this->XAxisRotation = 0;
  this->LargeArcFlag = false;
  this->SweepFlag = false;
}


//------------------------------------------------------------------------------
vtkSVGPathArcTo::~vtkSVGPathArcTo()
{}


//------------------------------------------------------------------------------
vtkSVGPathArcTo* vtkSVGPathArcTo::DeepCopy()
{
  vtkSVGPathArcTo *newCommand =
      vtkSVGPathArcTo::SafeDownCast(this->Superclass::DeepCopy());
  if (newCommand != NULL)
    {
    newCommand->SetRayX(this->RayX);
    newCommand->SetRayY(this->RayY);
    newCommand->SetXAxisRotation(this->XAxisRotation);
    newCommand->SetLargeArcFlag(this->LargeArcFlag);
    newCommand->SetSweepFlag(this->SweepFlag);
    }
  return newCommand;
}


//------------------------------------------------------------------------------
std::string vtkSVGPathArcTo::ToString(int precision)
{
  std::stringstream ss;
  if (precision >= 0)
    {
    ss.precision(precision);
    }
  char label = (this->IsAbsoluteCommand()) ? 'A' : 'a';
  ss << label << ' '
     << this->RayX << ',' << this->RayY << ' ' << this->XAxisRotation << ' '
     << this->LargeArcFlag << ' ' << this->SweepFlag << ' '
     << this->EndPoint[X] << ',' << this->EndPoint[Y];
  return ss.str();
}


//------------------------------------------------------------------------------
void vtkSVGPathArcTo::SetAll(float_type rayX, float_type rayY, int rotAngle,
                             bool largeArcFlag, bool sweepFlag,
                             float_type endX, float_type endY)
{
  this->SetRayX(rayX);
  this->SetRayY(rayY);
  this->SetXAxisRotation(rotAngle);
  this->SetLargeArcFlag(largeArcFlag);
  this->SetSweepFlag(sweepFlag);
  this->SetEndPoint(endX, endY);
}

//------------------------------------------------------------------------------
// In order to compute a polyline which approximates an elliptical arc defined
// through an SVG path `arcTo` command, several helper data structures and
// functions are implemented. Given an elliptical arc, it is split into smaller
// arc segments. For each segment, represented by an instance of
// the `vtkArcSegmentData` data structure, is computed a cubic Bezier curve
// which is a good approximation of the segment itself. So we can summarize by
// stating that each instance of an `arcTo` command is approximated by a set
// of cubic Bezier curves.
// In order to avoid to have to compute such a set of Bezier curves twice for
// the same elliptical arc a cache, implemented as an std::map, is used.
// This map use as key type the `vtkArcKey` helper data structure and as data
// type an array of cubic Bezier curves (`vtkCubicBezierCurve`).
//
//------------------------------------------------------------------------------
// This data structure stores information about a segment of an ellipse:
// - (cx, cy) is the ellipse center;
// - rx, ry are the ellipse rays;
// - th0, th1, are the angles values that are associated to the start and
//   the end point of the segment.
// - sin_th and cos_th are the sin and the cos of the angle by which is rotated
//   the ellipse X axis.
// Such arc segment representation is used for computing a cubic Bezier curve
// which approximates the segment itself. For being a good approximation
// the angle (th1 - th0) associated with the segment must be <= Pi/2.
struct vtkArcSegmentData
{
  double cx;
  double cy;
  double th0;
  double th1;
  double rx;
  double ry;
  double sin_th;
  double cos_th;
};


//------------------------------------------------------------------------------
// Helper data structure used as a key for the std::map which implements the
// cache for `arcTo` commands.
// Each `vtkArcKey` instance stores a pointer to an `arcTo` command and
// the coordinates of the start point of the arc segment.
// In such a way there is a unique key for each arc segment.
// The data structure provides a `ToString` method which is used later
// for implementing a simple comparison operator needed by std::map.
struct vtkArcKey
{
  vtkArcKey(vtkSVGPathArcTo* arc, float_type sx, float_type sy)
    : ArcTo(arc), StartX(sx), StartY(sy)
  {}

  vtkSmartPointer<vtkSVGPathArcTo> ArcTo;
  float_type StartX;
  float_type StartY;

  // Return a string representing the `arcTo` command followed by
  // the coordinates of the arc segment start point.
  std::string ToString() const
  {
    if (this->ArcTo.GetPointer() == NULL)
      {
      std::cerr << "vtkArcKey.ArcTo is NULL.\n";
      return "";
      }
    std::stringstream ss;
    ss.precision(7);
    ss << this->ArcTo->ToString(7) << ' ' << this->StartX << ' ' << this->StartY;
    return ss.str();
  }
};


//------------------------------------------------------------------------------
bool operator==(const vtkArcKey& left, const vtkArcKey& right)
{
  return (left.ToString().compare(right.ToString()) == 0);
}


//------------------------------------------------------------------------------
// Lexicographic ordering.
bool operator<(const vtkArcKey& left, const vtkArcKey& right)
{
  return (left.ToString().compare(right.ToString()) < 0);
}


//------------------------------------------------------------------------------
// In the `segments` array is returned a set of small segments into which
// the arc has been subdivided. The input parameter `ox`, `oy` represent
// the start point of the arc.
// This implementation is a porting of the one used by the Vega library.
void vtkSVGPathArcTo::ArcToSegments(vtkSVGPathArcTo::SegmentDataArray& segments,
                                    const vtkSVGPathArcTo& arc,
                                    float_type ox, float_type oy)
{
  segments.clear();

  double x = arc.EndPoint[X];
  double y = arc.EndPoint[Y];
  double rx = arc.RayX;
  double ry = arc.RayY;
  double rotateX = arc.XAxisRotation;
  bool large = arc.LargeArcFlag;
  bool sweep = arc.SweepFlag;

  double th = rotateX * (vtkMath::Pi() / 180);
  double sin_th = std::sin(th);
  double cos_th = std::cos(th);
  rx = std::fabs(rx);
  ry = std::fabs(ry);
  double px = cos_th * (ox - x) * 0.5 + sin_th * (oy - y) * 0.5;
  double py = cos_th * (oy - y) * 0.5 - sin_th * (ox - x) * 0.5;
  double pl = (px * px) / (rx * rx) + (py * py) / (ry * ry);
  if (pl > 1)
    {
    pl = std::sqrt(pl);
    rx *= pl;
    ry *= pl;
    }

  double a00 = cos_th / rx;
  double a01 = sin_th / rx;
  double a10 = (-sin_th) / ry;
  double a11 = (cos_th) / ry;
  double x0 = a00 * ox + a01 * oy;
  double y0 = a10 * ox + a11 * oy;
  double x1 = a00 * x + a01 * y;
  double y1 = a10 * x + a11 * y;

  double d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
  double sfactor_sq = 1 / d - 0.25;
  if (sfactor_sq < 0)
    sfactor_sq = 0;
  double sfactor = std::sqrt(sfactor_sq);
  if (sweep == large)
    sfactor = -sfactor;
  double xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
  double yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);

  double th0 = std::atan2(y0 - yc, x0 - xc);
  double th1 = std::atan2(y1 - yc, x1 - xc);

  double th_arc = th1 - th0;

  if (th_arc < 0 && sweep == 1)
    {
    th_arc += 2 * vtkMath::Pi();
    }
  else if (th_arc > 0 && sweep == 0)
    {
    th_arc -= 2 * vtkMath::Pi();
    }

  double numberOfSegments = std::ceil(
      std::fabs(th_arc / (vtkMath::Pi() * 0.5 + 0.001)));
  vtkDebugMacro( << "The number of segments is " << numberOfSegments << ".\n");

  unsigned int n = (numberOfSegments > 0)
      ? static_cast<unsigned int>(numberOfSegments)
      : 1;

  double th2, th3;
  for (unsigned int i = 0; i < n; ++i)
    {
    th2 = th0 + i * th_arc / n;
    th3 = th0 + (i + 1) * th_arc / n;
    vtkArcSegmentData segment = { xc, yc, th2, th3, rx, ry, sin_th, cos_th };
    segments.push_back(segment);
    }
}


//------------------------------------------------------------------------------
// In the `curve` output parameter is returned a Bezier curve which approximates
// an enough small arc segment. The `startX`, `startY` parameters represent
// the start point of the arc defined by the `arcTo` command.
// This implementation is a porting of the one used by the Vega library.
void vtkSVGPathArcTo::SegmentToBezier(vtkCubicBezierCurve& curve,
                                      const vtkArcSegmentData& seg,
                                      float_type startX, float_type startY)
{
  vtkDebugMacro( << "th0: " << seg.th0 << "\n");
  vtkDebugMacro( << "th1: " << seg.th1 << "\n");

  double a00 = seg.cos_th * seg.rx;
  double a01 = -seg.sin_th * seg.ry;
  double a10 = seg.sin_th * seg.rx;
  double a11 = seg.cos_th * seg.ry;

  double cos_th0 = std::cos(seg.th0);
  double sin_th0 = std::sin(seg.th0);
  double cos_th1 = std::cos(seg.th1);
  double sin_th1 = std::sin(seg.th1);

  double th_half = 0.5 * (seg.th1 - seg.th0);
  double sin_th_h2 = std::sin(th_half * 0.5);
  double t = (8.0 / 3.0) * sin_th_h2 * sin_th_h2 / std::sin(th_half);

  double x1 = seg.cx + cos_th0 - t * sin_th0;
  double y1 = seg.cy + sin_th0 + t * cos_th0;
  double x3 = seg.cx + cos_th1;
  double y3 = seg.cy + sin_th1;
  double x2 = x3 + t * sin_th1;
  double y2 = y3 - t * cos_th1;


  curve.P0[X] = startX;
  curve.P0[Y] = startY;
  curve.P1[X] = a00 * x1 + a01 * y1;
  curve.P1[Y] = a10 * x1 + a11 * y1;
  curve.P2[X] = a00 * x2 + a01 * y2;
  curve.P2[Y] = a10 * x2 + a11 * y2;
  curve.P3[X] = a00 * x3 + a01 * y3;
  curve.P3[Y] = a10 * x3 + a11 * y3;

  vtkDebugMacro( << "P0(" << curve.P0[X] << "," << curve.P0[Y] << ")\n");
  vtkDebugMacro( << "P1(" << curve.P1[X] << "," << curve.P1[Y] << ")\n");
  vtkDebugMacro( << "P2(" << curve.P2[X] << "," << curve.P2[Y] << ")\n");
  vtkDebugMacro( << "P3(" << curve.P3[X] << "," << curve.P3[Y] << ")\n");
}


//------------------------------------------------------------------------------
// This method returns an array of Bezier curves which approximates
// the passed arc. The `startX`, `startY` parameters represent
// the start point of the arc.
vtkSVGPathArcTo::BezierCurveArray&
vtkSVGPathArcTo::ToBezierCurves(vtkSVGPathArcTo* arc,
                                float_type startX, float_type startY)
{
  vtkSVGPathArcTo::ArcToBezierCurvesCacheType& cache =
      vtkSVGPathArcTo::ArcToBezierCurvesCache;

  // First off we check if the set of Bezier curves has already been computed
  // for this arc.
  vtkArcKey key(arc, startX, startY);
  vtkDebugMacro(<< "key: '" << key.ToString() << "'\n");
  vtkSVGPathArcTo::ArcToBezierCurvesCacheType::iterator p = cache.find(key);
  if (p != cache.end())
    {
    vtkDebugMacro(<< "found entry: '" << p->first.ToString() << "'\n");
    return p->second;
    }

  vtkSVGPathArcTo::SegmentDataArray segments;
  this->ArcToSegments(segments, *arc, startX, startY);

  unsigned int n = segments.size();
  // A new cache entry with key `key` is default constructed and returned.
  vtkSVGPathArcTo::BezierCurveArray& bcurves = cache[key];
  bcurves.resize(n);
  for (unsigned int i = 0; i < n; ++i)
    {
    this->SegmentToBezier(bcurves[i], segments[i], startX, startY);
    startX = bcurves[i].P3[X];
    startY = bcurves[i].P3[Y];
    }
  return cache[key];
}


//------------------------------------------------------------------------------
void vtkSVGPathArcTo::GetVertices(vtkPoints2D* vertices)
{
  if (vertices == NULL || this->Context == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get vertices.\n");
    return;
    }

  this->Context->LastCommandType = this->GetType();

  vtkSmartPointer<vtkSVGPathArcTo> absoluteCommand;
  if (this->IsAbsoluteCommand())
    {
    absoluteCommand = this;
    }
  else
    {
    absoluteCommand = this->DeepCopy();
    if (absoluteCommand.GetPointer() != NULL)
      {
      absoluteCommand->Delete();
      absoluteCommand->ToAbsoluteCommand();
      }
    else
      {
      vtkErrorMacro(<< "Deep copy failed.\n");
      }
    }

  if (absoluteCommand.GetPointer() != NULL)
    {

    vtkSVGPathArcTo::BezierCurveArray& curves =
        this->ToBezierCurves(absoluteCommand,
                             this->Context->CurrentEndPoint[X],
                             this->Context->CurrentEndPoint[Y]);
    unsigned int n = curves.size();

    for (unsigned int i = 0; i < n; ++i)
      {
      curves[i].GetVertices(vertices);
      }
    this->Context->SetCurrentEndPoint(curves.back().P3);
    }
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathQuadraticCurveTo);


//------------------------------------------------------------------------------
void vtkSVGPathQuadraticCurveTo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Control Point: (" << this->ControlPoint[0]
     << ", " << this->ControlPoint[1] << ")\n";
}


//------------------------------------------------------------------------------
vtkSVGPathQuadraticCurveTo::vtkSVGPathQuadraticCurveTo()
{
  this->ControlPoint[0] = 0;
  this->ControlPoint[1] = 0;
}


//------------------------------------------------------------------------------
vtkSVGPathQuadraticCurveTo::~vtkSVGPathQuadraticCurveTo()
{}


//------------------------------------------------------------------------------
vtkSVGPathQuadraticCurveTo* vtkSVGPathQuadraticCurveTo::DeepCopy()
{
  vtkSVGPathQuadraticCurveTo *newCommand =
      vtkSVGPathQuadraticCurveTo::SafeDownCast(this->Superclass::DeepCopy());
  if (newCommand)
    {
    newCommand->SetControlPoint(this->ControlPoint);
    }
  return newCommand;
}


//------------------------------------------------------------------------------
std::string vtkSVGPathQuadraticCurveTo::ToString(int precision)
{
  std::stringstream ss;
  if (precision >= 0)
    {
    ss.precision(precision);
    }
  char label = (this->IsAbsoluteCommand()) ? 'Q' : 'q';
  ss << label << ' '
     << this->ControlPoint[X] << ',' << this->ControlPoint[Y] << ' '
     << this->EndPoint[X] << ',' << this->EndPoint[Y];
  return ss.str();
}


//------------------------------------------------------------------------------
void vtkSVGPathQuadraticCurveTo::SetControlPoint(float_type x, float_type y)
{
  if (this->ControlPoint[0] != x || this->ControlPoint[1] != y)
    {
    this->ControlPoint[0] = x;
    this->ControlPoint[1] = y;
    this->Modified();
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathQuadraticCurveTo::SetControlPoint(float_type point[2])
{
  if (point != NULL)
    {
    this->SetControlPoint(point[0], point[1]);
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathQuadraticCurveTo::GetControlPoint(float_type point[2])
{
  if (point != NULL)
    {
    point[0] = this->ControlPoint[0];
    point[1] = this->ControlPoint[1];
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathQuadraticCurveTo::SetAll(float_type controlPointX, float_type controlPointY,
                                        float_type endX, float_type endY)
{
  this->SetControlPoint(controlPointX, controlPointY);
  this->SetEndPoint(endX, endY);
}


//------------------------------------------------------------------------------
float_type* vtkSVGPathQuadraticCurveTo::ToAbsoluteCommand(float_type startPoint[2])
{
  float_type *sp = this->Superclass::ToAbsoluteCommand(startPoint);
  if (sp != NULL)
    {
    TranslatePoint(this->ControlPoint, sp);
    }
  return sp;
}


//------------------------------------------------------------------------------
void vtkSVGPathQuadraticCurveTo::GetVertices(vtkPoints2D* vertices)
{
  static const float_type a = (1.0 / 3.0);
  static const float_type b = (2.0 / 3.0);

  if (vertices == NULL || this->Context == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get vertices.\n");
    return;
    }

  this->Context->LastCommandType = this->GetType();

  vtkSmartPointer<vtkSVGPathQuadraticCurveTo> absoluteCommand;
  if (this->IsAbsoluteCommand())
    {
    absoluteCommand = this;
    }
  else
    {
    absoluteCommand = this->DeepCopy();
    if (absoluteCommand.GetPointer() != NULL)
      {
      absoluteCommand->Delete();
      absoluteCommand->ToAbsoluteCommand();
      }
    else
      {
      vtkErrorMacro(<< "Deep copy failed.\n");
      }
    }

  if (absoluteCommand.GetPointer() != NULL)
    {
    // We elevate the Bezier curve order from quadratic to cubic.
    // If we have a curve with three control points, then we can create
    // a four control point curve that exactly reproduce the original curve
    // as long as we give it the same start and end points,
    // and for its two control points we pick:
    // "1/3rd start + 2/3rd control" and "2/3rd control + 1/3rd end",
    // and now we have exactly the same curve as before, except represented
    // as a cubic curve, rather than a quadratic curve.
    vtkCubicBezierCurve curve;
    absoluteCommand->Context->GetCurrentEndPoint(curve.P0);
    absoluteCommand->GetEndPoint(curve.P3);

    float_type controlX23 = b * absoluteCommand->ControlPoint[X];
    float_type controlY23 = b * absoluteCommand->ControlPoint[Y];

    curve.P1[X] = a * curve.P0[X] + controlX23;
    curve.P1[Y] = a * curve.P0[Y] + controlY23;
    curve.P2[X] = a * curve.P3[X] + controlX23;
    curve.P2[Y] = a * curve.P3[Y] + controlY23;

    curve.GetVertices(vertices);
    this->Context->SetCurrentEndPoint(absoluteCommand->EndPoint);
    this->Context->SetLastControlPoint(absoluteCommand->ControlPoint);
    }
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathSmoothQuadraticCurveTo);


//------------------------------------------------------------------------------
void vtkSVGPathSmoothQuadraticCurveTo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkSVGPathSmoothQuadraticCurveTo::vtkSVGPathSmoothQuadraticCurveTo()
{
}


//------------------------------------------------------------------------------
vtkSVGPathSmoothQuadraticCurveTo::~vtkSVGPathSmoothQuadraticCurveTo()
{}


//------------------------------------------------------------------------------
vtkSVGPathSmoothQuadraticCurveTo* vtkSVGPathSmoothQuadraticCurveTo::DeepCopy()
{
  vtkSVGPathSmoothQuadraticCurveTo *newCommand =
      vtkSVGPathSmoothQuadraticCurveTo::SafeDownCast(this->Superclass::DeepCopy());
  return newCommand;
}


//------------------------------------------------------------------------------
std::string vtkSVGPathSmoothQuadraticCurveTo::ToString(int precision)
{
  std::stringstream ss;
  if (precision >= 0)
    {
    ss.precision(precision);
    }
  char label = (this->IsAbsoluteCommand()) ? 'T' : 't';
  ss << label << ' '
     << this->EndPoint[X] << ',' << this->EndPoint[Y];
  return ss.str();
}


//------------------------------------------------------------------------------
vtkSVGPathQuadraticCurveTo* vtkSVGPathSmoothQuadraticCurveTo::GetAbsoluteQuadraticCurveTo()
{
  if (this->Context == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get the absolute quadratic `curveTo` command.\n");
    return NULL;
    }

  vtkDebugMacro(<< "command: '" << this->ToString() << "'\n");
  vtkSVGPathQuadraticCurveTo* quadraticCurveTo =
      vtkSVGPathQuadraticCurveTo::New();
  if (quadraticCurveTo != NULL)
    {
    quadraticCurveTo->SetUseAbsoluteCoordinate(this->UseAbsoluteCoordinate);
    quadraticCurveTo->SetContext(this->Context);
    quadraticCurveTo->SetEndPoint(this->EndPoint);

    quadraticCurveTo->ToAbsoluteCommand();

    // If the previous command is still a quadratic `curveTo` command,
    // the control point for this `curveTo` command is the reflection of
    // the control point of the previous command respect with the current
    // path end point. Else it is the current end point.
    float_type reflectionPoint[2];
    if (this->Context->LastCommandType == 'Q' || this->Context->LastCommandType == 'T')
      {
      float_type* end = this->Context->CurrentEndPoint;
      float_type* control = this->Context->LastControlPoint;
      reflectionPoint[X] = 2 * end[X] - control[X];
      reflectionPoint[Y] = 2 * end[Y] - control[Y];
      }
    else
      {
      this->Context->GetCurrentEndPoint(reflectionPoint);
      }

    quadraticCurveTo->SetControlPoint(reflectionPoint);
    vtkDebugMacro(<< "quadraticCurveTo command: '"
        << quadraticCurveTo->ToString() << "'\n");
    }
  return quadraticCurveTo;
}


//------------------------------------------------------------------------------
void vtkSVGPathSmoothQuadraticCurveTo::GetVertices(vtkPoints2D* vertices)
{
  if (vertices == NULL || this->Context == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get vertices.\n");
    return;
    }
  vtkSmartPointer<vtkSVGPathQuadraticCurveTo> quadraticCurveToCommand =
      this->GetAbsoluteQuadraticCurveTo();
  if (quadraticCurveToCommand.GetPointer() != NULL)
    {
    quadraticCurveToCommand->Delete();
    quadraticCurveToCommand->GetVertices(vertices);
    }
  this->Context->LastCommandType = this->GetType();
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathCubicCurveTo);


//------------------------------------------------------------------------------
void vtkSVGPathCubicCurveTo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "First Control Point: (" << this->FirstControlPoint[0]
     << ", " << this->FirstControlPoint[1] << ")\n";
  os << indent << "Second Control Point: (" << this->SecondControlPoint[0]
     << ", " << this->SecondControlPoint[1] << ")\n";
}


//------------------------------------------------------------------------------
vtkSVGPathCubicCurveTo::vtkSVGPathCubicCurveTo()
{
  this->FirstControlPoint[0] = 0;
  this->FirstControlPoint[1] = 0;
  this->SecondControlPoint[0] = 0;
  this->SecondControlPoint[1] = 0;
}


//------------------------------------------------------------------------------
vtkSVGPathCubicCurveTo::~vtkSVGPathCubicCurveTo()
{}


//------------------------------------------------------------------------------
vtkSVGPathCubicCurveTo* vtkSVGPathCubicCurveTo::DeepCopy()
{
  vtkSVGPathCubicCurveTo *newCommand =
      vtkSVGPathCubicCurveTo::SafeDownCast(this->Superclass::DeepCopy());
  if (newCommand)
    {
    newCommand->SetFirstControlPoint(this->FirstControlPoint);
    newCommand->SetSecondControlPoint(this->SecondControlPoint);
    }
  return newCommand;
}


//------------------------------------------------------------------------------
std::string vtkSVGPathCubicCurveTo::ToString(int precision)
{
  std::stringstream ss;
  if (precision >= 0)
    {
    ss.precision(precision);
    }
  char label = (this->IsAbsoluteCommand()) ? 'C' : 'c';
  ss << label << ' '
     << this->FirstControlPoint[X] << ',' << this->FirstControlPoint[Y] << ' '
     << this->SecondControlPoint[X] << ',' << this->SecondControlPoint[Y] << ' '
     << this->EndPoint[X] << ',' << this->EndPoint[Y];
  return ss.str();
}


//------------------------------------------------------------------------------
void vtkSVGPathCubicCurveTo::SetFirstControlPoint(float_type x, float_type y)
{
  if (this->FirstControlPoint[0] != x || this->FirstControlPoint[1] != y)
    {
    this->FirstControlPoint[0] = x;
    this->FirstControlPoint[1] = y;
    this->Modified();
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathCubicCurveTo::SetFirstControlPoint(float_type point[2])
{
  if (point != NULL)
    {
    this->SetFirstControlPoint(point[0], point[1]);
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathCubicCurveTo::GetFirstControlPoint(float_type point[2])
{
  if (point != NULL)
    {
    point[0] = this->FirstControlPoint[0];
    point[1] = this->FirstControlPoint[1];
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathCubicCurveTo::SetSecondControlPoint(float_type x, float_type y)
{
  if (this->SecondControlPoint[0] != x || this->SecondControlPoint[1] != y)
    {
    this->SecondControlPoint[0] = x;
    this->SecondControlPoint[1] = y;
    this->Modified();
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathCubicCurveTo::SetSecondControlPoint(float_type point[2])
{
  if (point != NULL)
    {
    this->SetSecondControlPoint(point[0], point[1]);
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathCubicCurveTo::GetSecondControlPoint(float_type point[2])
{
  if (point != NULL)
    {
    point[0] = this->SecondControlPoint[0];
    point[1] = this->SecondControlPoint[1];
    }
}


//------------------------------------------------------------------------------
void vtkSVGPathCubicCurveTo::SetAll(float_type firstX, float_type firstY,
                                    float_type secondX, float_type secondY,
                                    float_type endX, float_type endY)
{
  this->SetFirstControlPoint(firstX, firstY);
  this->SetSecondControlPoint(secondX, secondY);
  this->SetEndPoint(endX, endY);
}


//------------------------------------------------------------------------------
float_type* vtkSVGPathCubicCurveTo::ToAbsoluteCommand(float_type startPoint[2])
{
  float_type *sp = this->Superclass::ToAbsoluteCommand(startPoint);
  if (sp != NULL)
    {
    TranslatePoint(this->FirstControlPoint, sp);
    TranslatePoint(this->SecondControlPoint, sp);
    }
  return sp;
}


//------------------------------------------------------------------------------
void vtkSVGPathCubicCurveTo::GetVertices(vtkPoints2D* vertices)
{
  if (vertices == NULL || this->Context == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get vertices.\n");
    return;
    }

  this->Context->LastCommandType = this->GetType();

  vtkSmartPointer<vtkSVGPathCubicCurveTo> absoluteCommand;
  if (this->IsAbsoluteCommand())
    {
    absoluteCommand = this;
    }
  else
    {
    absoluteCommand = this->DeepCopy();
    if (absoluteCommand.GetPointer() != NULL)
      {
      absoluteCommand->Delete();
      absoluteCommand->ToAbsoluteCommand();
      }
    else
      {
      vtkErrorMacro(<< "Deep copy failed.\n");
      }
    }

  if (absoluteCommand.GetPointer() != NULL)
    {
    vtkCubicBezierCurve curve;
    absoluteCommand->Context->GetCurrentEndPoint(curve.P0);
    absoluteCommand->GetFirstControlPoint(curve.P1);
    absoluteCommand->GetSecondControlPoint(curve.P2);
    absoluteCommand->GetEndPoint(curve.P3);
    curve.GetVertices(vertices);
    this->Context->SetCurrentEndPoint(curve.P3);
    this->Context->SetLastControlPoint(curve.P2);
    }
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathSmoothCubicCurveTo);


//------------------------------------------------------------------------------
void vtkSVGPathSmoothCubicCurveTo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Second Control Point: (" << this->SecondControlPoint[0]
     << ", " << this->SecondControlPoint[1] << ")\n";
}


//------------------------------------------------------------------------------
vtkSVGPathSmoothCubicCurveTo::vtkSVGPathSmoothCubicCurveTo()
{
  this->FirstControlPoint[0] = 0;
  this->FirstControlPoint[1] = 0;
  this->SecondControlPoint[0] = 0;
  this->SecondControlPoint[1] = 0;
}


//------------------------------------------------------------------------------
vtkSVGPathSmoothCubicCurveTo::~vtkSVGPathSmoothCubicCurveTo()
{}


//------------------------------------------------------------------------------
vtkSVGPathSmoothCubicCurveTo* vtkSVGPathSmoothCubicCurveTo::DeepCopy()
{
  vtkSVGPathSmoothCubicCurveTo *newCommand =
      vtkSVGPathSmoothCubicCurveTo::SafeDownCast(this->Superclass::DeepCopy());
  return newCommand;
}


//------------------------------------------------------------------------------
std::string vtkSVGPathSmoothCubicCurveTo::ToString(int precision)
{
  std::stringstream ss;
  if (precision >= 0)
    {
    ss.precision(precision);
    }
  char label = (this->IsAbsoluteCommand()) ? 'S' : 's';
  ss << label << ' '
     << this->SecondControlPoint[X] << ',' << this->SecondControlPoint[Y] << ' '
     << this->EndPoint[X] << ',' << this->EndPoint[Y];
  return ss.str();
}


//------------------------------------------------------------------------------
void vtkSVGPathSmoothCubicCurveTo::SetAll(float_type secondX, float_type secondY,
                                          float_type endX, float_type endY)
{
  this->SetSecondControlPoint(secondX, secondY);
  this->SetEndPoint(endX, endY);
}


//------------------------------------------------------------------------------
vtkSVGPathCubicCurveTo* vtkSVGPathSmoothCubicCurveTo::GetAbsoluteCubicCurveTo()
{
  if (this->Context == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get the absolute `cubicCurveTo` command.\n");
    return NULL;
    }
  vtkDebugMacro(<< "command: '" << this->ToString() << "'\n");
  vtkSVGPathCubicCurveTo* cubicCurveTo = vtkSVGPathCubicCurveTo::New();
  if (cubicCurveTo != NULL)
    {
    cubicCurveTo->SetUseAbsoluteCoordinate(this->UseAbsoluteCoordinate);
    cubicCurveTo->SetContext(this->Context);
    cubicCurveTo->SetEndPoint(this->EndPoint);
    cubicCurveTo->SetSecondControlPoint(this->SecondControlPoint);

    cubicCurveTo->ToAbsoluteCommand();

    float_type reflectionPoint[2];
    if (this->Context->LastCommandType == 'C' || this->Context->LastCommandType == 'S')
      {
      float_type* end = this->Context->CurrentEndPoint;
      float_type* control = this->Context->LastControlPoint;
      reflectionPoint[X] = 2 * end[X] - control[X];
      reflectionPoint[Y] = 2 * end[Y] - control[Y];
      }
    else
      {
      this->Context->GetCurrentEndPoint(reflectionPoint);
      }

    cubicCurveTo->SetFirstControlPoint(reflectionPoint);
    }
  vtkDebugMacro(<< "cubicCurveTo command: '" << cubicCurveTo->ToString() << "'\n");
  return cubicCurveTo;
}


//------------------------------------------------------------------------------
void vtkSVGPathSmoothCubicCurveTo::GetVertices(vtkPoints2D* vertices)
{
  if (vertices == NULL || this->Context == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get vertices.\n");
    return;
    }

  vtkSmartPointer<vtkSVGPathCubicCurveTo> cubicCurveToCommand =
      this->GetAbsoluteCubicCurveTo();
  if (cubicCurveToCommand.GetPointer() != NULL)
    {
    cubicCurveToCommand->Delete();
    cubicCurveToCommand->GetVertices(vertices);
    }
  this->Context->LastCommandType = this->GetType();
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPathClosePath);


//------------------------------------------------------------------------------
void vtkSVGPathClosePath::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
vtkSVGPathClosePath::vtkSVGPathClosePath()
{
}


//------------------------------------------------------------------------------
vtkSVGPathClosePath::~vtkSVGPathClosePath()
{}


//------------------------------------------------------------------------------
vtkSVGPathClosePath* vtkSVGPathClosePath::DeepCopy()
{
  return vtkSVGPathClosePath::SafeDownCast(this->Superclass::DeepCopy());
}


//------------------------------------------------------------------------------
std::string vtkSVGPathClosePath::ToString(int )
{
  return std::string("Z");
}


//------------------------------------------------------------------------------
float_type* vtkSVGPathClosePath::ToAbsoluteCommand(float_type* )
{
  return NULL;
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSVGPath);


//------------------------------------------------------------------------------
void vtkSVGPath::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  vtkIndent nextIndent = indent.GetNextIndent();
  unsigned int n = this->GetNumberOfCommands();
  for (unsigned int i = 0; i < n; ++i)
    {
    os << indent << "command " << i << ":\n";
    this->CommandList[i]->PrintSelf(os, nextIndent);
    }
}


//------------------------------------------------------------------------------
vtkSVGPath::vtkSVGPath()
{
}


//------------------------------------------------------------------------------
vtkSVGPath::~vtkSVGPath()
{}


//------------------------------------------------------------------------------
unsigned int vtkSVGPath::GetNumberOfCommands()
{
  return this->CommandList.size();
}

//------------------------------------------------------------------------------
void vtkSVGPath::SetCommand(unsigned int i, vtkSVGPathCommand* command)
{
  if (command == NULL)
    {
    vtkErrorMacro(<< "Passed command is NULL.\n");
    return;
    }
  if (i < this->GetNumberOfCommands())
    {
    if (this->CommandList[i].GetPointer() != command)
      {
      this->CommandList[i] = command;
      this->Modified();
      }
    }
  else
    {
    vtkErrorMacro(<< "Passed index is out of bounds: " << i << ".\n");
    }
}


//------------------------------------------------------------------------------
vtkSVGPathCommand* vtkSVGPath::GetCommand(unsigned int i)
{
  if (i < this->GetNumberOfCommands())
    {
    return this->CommandList[i].GetPointer();
    }
  else
    {
    return NULL;
    }
}


//------------------------------------------------------------------------------
void vtkSVGPath::InsertCommand(unsigned int i, vtkSVGPathCommand* command)
{
  if (command == NULL)
    {
    vtkErrorMacro(<< "Passed command is NULL.\n");
    return;
    }
  if (i <= this->GetNumberOfCommands())
    {
    vtkSmartPointer<vtkSVGPathCommand> c = command;
    this->CommandList.insert(this->CommandList.begin() + i, c);
    this->Modified();
    }
  else
    {
    vtkErrorMacro(<< "Passed index is out of bounds: " << i << ".\n");
    }
}


//------------------------------------------------------------------------------
void vtkSVGPath::InsertCommand(unsigned int i, char type)
{
  if (type == 'Z' || type == 'z')
    {
    vtkSmartPointer<vtkSVGPathClosePath> command =
        vtkSmartPointer<vtkSVGPathClosePath>::New();
    this->InsertCommand(i, command.GetPointer());
    }
  else
    {
    vtkErrorMacro(<< "Wrong SVG path command type: '" << type << "'.");
    }
}


//------------------------------------------------------------------------------
void vtkSVGPath::InsertCommand(unsigned int i, char type, float_type end)
{
  static const std::string validTypes("HhVv");

  if (validTypes.find(type) != std::string::npos)
    {
    vtkSmartPointer<vtkSVGPathCommand> command =
        vtkSVGPathCommand::NewCommand(type);
    command->Delete();
    if (command != NULL)
      {
      if (command->GetType() == 'H')
        {
        command->SetEndPoint(end, 0);
        }
      else
        {
        command->SetEndPoint(0, end);
        }
      this->InsertCommand(i, command.GetPointer());
      }
    }
  else
    {
    vtkErrorMacro(<< "Wrong SVG path command type: '" << type << "'.");
    }
}


//------------------------------------------------------------------------------
void vtkSVGPath::InsertCommand(unsigned int i, char type,
                               float_type endX, float_type endY)
{
  static const std::string validTypes("MmLlTt");

  if (validTypes.find(type) != std::string::npos)
    {
    vtkSmartPointer<vtkSVGPathCommand> command =
        vtkSVGPathCommand::NewCommand(type);
    command->Delete();
    if (command != NULL)
      {
      command->SetEndPoint(endX, endY);
      this->InsertCommand(i, command.GetPointer());
      }
    else
      {
      vtkErrorMacro(<< "Returned SVG path command is NULL.\n");
      }
    }
  else
    {
    vtkErrorMacro(<< "Wrong SVG path command type: '" << type << "'.");
    }
}


//------------------------------------------------------------------------------
void vtkSVGPath::InsertCommand(unsigned int i, char type,
                               float_type controlPointX, float_type controlPointY,
                               float_type endX, float_type endY)
{
  static const std::string validTypes("QqSs");

  if (validTypes.find(type) != std::string::npos)
    {
    vtkSmartPointer<vtkSVGPathCommand> command =
        vtkSVGPathCommand::NewCommand(type);
    if (command != NULL)
      {
      command->Delete();
      if (command->GetType() == 'Q')
        {
        vtkSVGPathQuadraticCurveTo* c =
            vtkSVGPathQuadraticCurveTo::SafeDownCast(command.GetPointer());
        c->SetControlPoint(controlPointX, controlPointY);
        c->SetEndPoint(endX, endY);
        this->InsertCommand(i, command.GetPointer());
        }
      else
        {
        vtkSVGPathSmoothCubicCurveTo* c =
            vtkSVGPathSmoothCubicCurveTo::SafeDownCast(command.GetPointer());
        c->SetSecondControlPoint(controlPointX, controlPointY);
        c->SetEndPoint(endX, endY);
        this->InsertCommand(i, command.GetPointer());
        }
      }
    }
  else
    {
    vtkErrorMacro(<< "Wrong SVG path command type: '" << type << "'.");
    }
}


//------------------------------------------------------------------------------
void vtkSVGPath::InsertCommand(unsigned int i, char type,
                               float_type firstX, float_type firstY,
                               float_type secondX, float_type secondY,
                               float_type endX, float_type endY)
{
  static const std::string validTypes("Cc");

  if (validTypes.find(type) != std::string::npos)
    {
    vtkSmartPointer<vtkSVGPathCubicCurveTo> command =
        vtkSVGPathCubicCurveTo::SafeDownCast(vtkSVGPathCommand::NewCommand(type));
    command->Delete();
    if (command != NULL)
      {
      command->SetAll(firstX, firstY, secondX, secondY, endX, endY);
      this->InsertCommand(i, command.GetPointer());
      }
    }
  else
    {
    vtkErrorMacro(<< "Wrong SVG path command type: '" << type << "'.");
    }
}


//------------------------------------------------------------------------------
void vtkSVGPath::InsertCommand(unsigned int i, char type,
                               float_type rayX, float_type rayY, int rotAngle,
                               bool largeArcFlag, bool sweepFlag,
                               float_type endX, float_type endY)
{
  static const std::string validTypes("Aa");

  if (validTypes.find(type) != std::string::npos)
    {
    vtkSmartPointer<vtkSVGPathArcTo> command =
        vtkSVGPathArcTo::SafeDownCast(vtkSVGPathCommand::NewCommand(type));
    command->Delete();
    if (command != NULL)
      {
      command->SetAll(rayX, rayY, rotAngle, largeArcFlag, sweepFlag, endX, endY);
      this->InsertCommand(i, command.GetPointer());
      }
    }
  else
    {
    vtkErrorMacro(<< "Wrong SVG path command type: '" << type << "'.");
    }

}


//------------------------------------------------------------------------------
void vtkSVGPath::InsertCommandsFromString(unsigned int i,
                                          const std::string& commandstr)
{

  if (commandstr.empty())
    {
    return;
    }

  std::stringstream ss;
  ss.str(commandstr);

  char type;
  float_type ex, ey;
  char sep1, sep2, sep3;

  while (ss)
    {
    ss >> type;
    switch (type)
      {
      case 'Z':
      case 'z':
        {
        if (ss)
          {
          this->InsertCommand(i, type);
          }
        break;
        }
      case 'h':
      case 'H':
      case 'v':
      case 'V':
        {
        float_type e;
        ss >> e;
        if (ss)
          {
          this->InsertCommand(i, type, e);
          }
        break;
        }
      case 'l':
      case 'L':
      case 'm':
      case 'M':
      case 't':
      case 'T':
        {
        ss >> ex >> sep1 >> ey;
        if (ss && sep1 == ',')
          {
          this->InsertCommand(i, type, ex, ey);
          }
        break;
        }
      case 'q':
      case 'Q':
      case 's':
      case 'S':
        {
        float_type cx, cy;
        ss >> cx >> sep1 >> cy >> ex >> sep2 >> ey;
        if (ss && sep1 == ',' && sep2 == ',')
          {
          this->InsertCommand(i, type, cx, cy, ex, ey);
          }
        break;
        }
      case 'c':
      case 'C':
        {
        float_type c1x, c1y, c2x, c2y;
        ss >> c1x >> sep1 >> c1y >> c2x >> sep2 >> c2y >> ex >> sep3 >> ey;
        if (ss && sep1 == ',' && sep2 == ',' && sep3 == ',')
          {
          this->InsertCommand(i, type, c1x, c1y, c2x, c2y, ex, ey);
          }
        break;
        }
      case 'a':
      case 'A':
        {
        float_type cx, cy;
        int a;
        bool lf, sf;
        ss >> cx >> sep1 >> cy >> a >> lf >> sf >> ex >> sep2 >> ey;
        if (ss && sep1 == ',' && sep2 == ',')
          {
          this->InsertCommand(i, type, cx, cy, a, lf, sf, ex, ey);
          }
        break;
        }
      default:
        break;
      }
    ++i;
    }
}


//------------------------------------------------------------------------------
void vtkSVGPath::AppendCommand(vtkSVGPathCommand* command)
{
  this->InsertCommand(this->GetNumberOfCommands(), command);
}


//------------------------------------------------------------------------------
void vtkSVGPath::AppendCommand(char type)
{
  this->InsertCommand(this->GetNumberOfCommands(), type);
}


//------------------------------------------------------------------------------
void vtkSVGPath::AppendCommand(char type, float_type end)
{
  this->InsertCommand(this->GetNumberOfCommands(), type, end);
}


//------------------------------------------------------------------------------
void vtkSVGPath::AppendCommand(char type, float_type endX, float_type endY)
{
  this->InsertCommand(this->GetNumberOfCommands(), type, endX, endY);
}


//------------------------------------------------------------------------------
void vtkSVGPath::AppendCommand(char type,
                               float_type controlPointX, float_type controlPointY,
                               float_type endX, float_type endY)
{
  this->InsertCommand(this->GetNumberOfCommands(), type,
                      controlPointX, controlPointY,
                      endX, endY);
}


//------------------------------------------------------------------------------
void vtkSVGPath::AppendCommand(char type,
                               float_type firstX, float_type firstY,
                               float_type secondX, float_type secondY,
                               float_type endX, float_type endY)
{
  this->InsertCommand(this->GetNumberOfCommands(), type,
                      firstX, firstY,
                      secondX, secondY,
                      endX, endY);
}


//------------------------------------------------------------------------------
void vtkSVGPath::AppendCommand(char type,
                               float_type rayX, float_type rayY, int rotAngle,
                               bool largeArcFlag, bool sweepFlag,
                               float_type endX, float_type endY)
{
  this->InsertCommand(this->GetNumberOfCommands(), type,
                      rayX, rayY, rotAngle,
                      largeArcFlag, sweepFlag,
                      endX, endY);
}


//------------------------------------------------------------------------------
void vtkSVGPath::AppendCommandsFromString(const std::string& commandstr)
{
  this->InsertCommandsFromString(this->GetNumberOfCommands(), commandstr);
}


//------------------------------------------------------------------------------
bool vtkSVGPath::RemoveCommand(unsigned int i)
{
  if (i < this->GetNumberOfCommands())
    {
    this->CommandList.erase(this->CommandList.begin() + i);
    this->Modified();
    return true;
    }
  else
    {
    return false;
    }
}


//------------------------------------------------------------------------------
void vtkSVGPath::RemoveAllCommands()
{
  this->CommandList.clear();
  this->Modified();
}


//------------------------------------------------------------------------------
std::string vtkSVGPath::ToString(int precision)
{
  std::string s;
  unsigned int n = this->GetNumberOfCommands();
  for (unsigned int i = 0; i < n; ++i)
    {
    s.append(this->CommandList[i]->ToString(precision));
    s.push_back(' ');
    }
  return s.substr(0, s.size() - 1);
}

//------------------------------------------------------------------------------
// Follow a bunch of helper function objects used as functors in STL algorithms
// for simplifying a path by removing redundant commands.
//
//------------------------------------------------------------------------------
// Predicate which returns true iff the type of both the current and
// the previous commands are of the same type passed to the predicate
// instance constructor.
class vtkSVGPathSimplifyIsSameTypeOfPreviousCommand
{
public:
  vtkSVGPathSimplifyIsSameTypeOfPreviousCommand(char type)
    : Type(type)
  {}

  bool operator()(vtkSVGPathCommand* previousCommand,
                  vtkSVGPathCommand* currentCommand) const
  {
    return (currentCommand->GetType() == this->Type &&
            previousCommand->GetType() == this->Type);
  }

private:
  char Type;
};

//------------------------------------------------------------------------------
// Functor which returns true iff the current and the previous commands are
// both relative `moveTo` commands. The previous command is changed by
// translating its end point by a vector equal to the end point of
// the current command. In such a way we get a relative `moveTo` command
// which has the same effect of both commands. Later the current command is
// discarded by the std::unique algorithm this functor is passed to.
class vtkSVGPathSimplifyIsRelMoveToPair
{
public:
  bool operator()(vtkSVGPathCommand* previousCommand,
                  vtkSVGPathCommand* currentCommand) const
  {
    if (currentCommand->GetType() == 'M' && currentCommand->IsRelativeCommand() &&
        previousCommand->GetType() == 'M' && previousCommand->IsRelativeCommand())
      {
      float_type pend[2];
      previousCommand->GetEndPoint(pend);
      float_type cend[2];
      currentCommand->GetEndPoint(cend);
      previousCommand->SetEndPoint(pend[X] + cend[X], pend[Y] + cend[Y]);
      return true;
      }
    else
      {
      return false;
      }
  }
};

//------------------------------------------------------------------------------
// Predicate which returns true iff the current command is a `moveTo` command
// whose effect is to move to the current point.
struct vtkSVGPathSimplifyIsMoveToCurrentPoint
{
  bool operator()(vtkSVGPathCommand* previousCommand,
                  vtkSVGPathCommand* currentCommand) const
  {
    if (currentCommand->GetType() == 'M')
      {
      if (currentCommand->IsAbsoluteCommand())
        {
        if (previousCommand->IsAbsoluteCommand())
          {
          float_type previousEndPoint[2];
          previousCommand->GetEndPoint(previousEndPoint);
          float_type currentEndPoint[2];
          currentCommand->GetEndPoint(currentEndPoint);
          return (currentEndPoint[0] == previousEndPoint[0] &&
                  currentEndPoint[1] == previousEndPoint[1]);
          }
        else
          {
          return false;
          }
        }
      else
        {
        float_type currentEndPoint[2];
        currentCommand->GetEndPoint(currentEndPoint);
        return (currentEndPoint[0] == 0 && currentEndPoint[1] == 0);
        }
      }
    else
      {
      return false;
      }
  }
};

//------------------------------------------------------------------------------
// Functor which transform the current command in an absolute command when
// the current command is a relative `moveTo` command the previous command is
// an absolute `moveTo` command.
struct vtkSVGPathSimplifyToAbsoluteCommand
{
  vtkSVGPathSimplifyToAbsoluteCommand()
    : previousCommand(NULL)
  {}

  void operator()(vtkSVGPathCommand* currentCommand)
  {
    if (currentCommand->GetType() == 'M' && currentCommand->IsRelativeCommand())
      {
      if (this->previousCommand && this->previousCommand->GetType() == 'M' &&
          this->previousCommand->IsAbsoluteCommand())
        {
        float_type pend[2];
        previousCommand->GetEndPoint(pend);
        currentCommand->ToAbsoluteCommand(pend);
        }
      }
    this->previousCommand = currentCommand;
  }
private:
  vtkSVGPathCommand* previousCommand;
};


//------------------------------------------------------------------------------
void vtkSVGPath::Simplify()
{
  if (this->CommandList.empty())
    {
    return;
    }

  // We collapse any relative `moveTo` command sequence
  // to a single relative `moveTo` command.
  this->CommandList.erase(
      std::unique(this->CommandList.begin(),
                  this->CommandList.end(),
                  vtkSVGPathSimplifyIsRelMoveToPair()),
      this->CommandList.end());

  // We transform each relative `moveTo` command whose previous command is
  // an absolute `moveTo` into an absolute command.
  std::for_each(this->CommandList.begin(),
                this->CommandList.end(),
                vtkSVGPathSimplifyToAbsoluteCommand());

  // We remove all consecutive `moveTo` commands except the last one
  // in the sequence. Because of the previous processing of the command list,
  // for each pair of two consecutive `moveTo` commands the second one is
  // always an absolute command. After the following processing there will be
  // only isolated absolute or relative `moveTo` commands.
  // Note: we use reverse iterators here; elements to be removed are moved to
  // the beginning of the STL container instead of the end; `erase` method does
  // not accept reverse iterator as arguments.
  this->CommandList.erase(
      this->CommandList.begin(),
      std::unique(this->CommandList.rbegin(),
                  this->CommandList.rend(),
                  vtkSVGPathSimplifyIsSameTypeOfPreviousCommand('M')).base());

  // We remove all consecutive `closePath` commands.
  this->CommandList.erase(
      std::unique(this->CommandList.begin(),
                  this->CommandList.end(),
                  vtkSVGPathSimplifyIsSameTypeOfPreviousCommand('Z')),
      this->CommandList.end());

  // We remove all `moveTo` commands which move to the current point.
  this->CommandList.erase(
      std::unique(this->CommandList.begin(),
                  this->CommandList.end(),
                  vtkSVGPathSimplifyIsMoveToCurrentPoint()),
      this->CommandList.end());
  // We remove the last element if it is a `moveTo` command.
  if (this->CommandList.back()->GetType() == 'M')
    {
    this->CommandList.pop_back();
    }
}


//------------------------------------------------------------------------------
unsigned int vtkSVGPath::GetNumberOfSubPaths()
{
  unsigned int n = this->GetNumberOfCommands();
  if (n == 0)
    {
    return 0;
    }

  vtkSVGPathCommand* command = NULL;
  vtkSVGPathCommand* previousCommand = this->GetCommand(0);

  // n != 0 --> we have at least one sub-path.
  unsigned int total = 1;

  // We start from the second command since if the first command is
  // `moveTo` the counter would be erroneously increased.
  // Note that even if the last command is `closePath` the counter is not
  // increased. This is the correct behavior since no new sub-path starts.
  for (unsigned int i = 1; i < n; ++i)
    {
    command = this->GetCommand(i);
    if (command->GetType() == 'M' || previousCommand->GetType() == 'Z')
      {
      ++total;
      }
    previousCommand = command;
    }
  return total;
}

//------------------------------------------------------------------------------
void vtkSVGPath::GetContours(ContourArrayType& contours)
{
  contours.clear();

  this->Simplify();

  if (this->CommandList.empty())
    {
    return;
    }

  this->CurrentContext.Reset();

  contours.reserve(this->GetNumberOfSubPaths());
  vtkSmartPointer<vtkPoints2D> vertices;
  vtkSVGPathCommand* command = this->GetCommand(0);
  if (command->GetType() != 'M')
    {
    vertices = vtkSmartPointer<vtkPoints2D>::New();
    contours.push_back(vertices);
    vertices->InsertNextPoint(this->CurrentContext.CurrentEndPoint);
    }

  float_type currentSubPathStartPoint[2] = {0, 0};

  unsigned int n = this->GetNumberOfCommands();
  for (unsigned int i = 0; i < n; ++i)
    {
    command = this->GetCommand(i);
    if (command != NULL)
      {
      command->SetContext(&(this->CurrentContext));
      if (command->GetType() == 'Z')
        {
        command->SetEndPoint(currentSubPathStartPoint);
        }
      else if (command->GetType() == 'M')
        {
        command->GetEndPoint(currentSubPathStartPoint);
        }
      if (command->GetType() == 'M' || this->CurrentContext.LastCommandType == 'Z')
        {
        vertices = vtkSmartPointer<vtkPoints2D>::New();
        contours.push_back(vertices);
        }
      command->GetVertices(vertices.GetPointer());
      }
    else
      {
      std::cerr << "vtkSVGPath::GetContours: command[" << i << "] is NULL.\n";
      }
    }
}
