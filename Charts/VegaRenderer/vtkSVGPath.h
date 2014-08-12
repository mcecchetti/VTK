/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSVGPath.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSVGPath - Provide a representation for an SVG path and computes
// collection of polylines which approximate the path.
// .SECTION Description
// vtkSVGPath is an object that allow the user to defines an SVG path as a
// sequence of commands where each command is an instance of a class derived
// from vtkSVGPathCommand. Moreover it is able to compute a collection of
// arrays of 2D points. Each array contains the vertices of a polyline
// which approximates a sub-path defined into the path object.

#ifndef __vtkSVGPath_h
#define __vtkSVGPath_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkObject.h"
#include "vtkSmartPointer.h"

#include <vector>
#include <map>

class vtkPoints2D;
class vtkCubicBezierCurve;
struct vtkArcKey;
struct vtkArcSegmentData;

typedef float float_type;

//------------------------------------------------------------------------------
// Description:
// This data structure is used by `vtkSVGPath` methods for keeping track of
// path information while processing SVG path commands.
// The path information stored by an instance of `vtkSVGPathContext` is:
// - the type of the last executed command;
// - the current end point after executing the last command;
// - the last control point, if any, encountered while processing the path
//   commands.
// Even if such information can be accessed directly, the data structure
// provides some methods for helping in get/set the stored data which represents
// 2D points.
struct vtkSVGPathContext
{
  void SetCurrentEndPoint(float_type x, float_type y)
  {
    this->CurrentEndPoint[0] = x;
    this->CurrentEndPoint[1] = y;
  }

  void SetCurrentEndPoint(float_type point[2])
  {
      this->CurrentEndPoint[0] = point[0];
      this->CurrentEndPoint[1] = point[1];
  }

  void GetCurrentEndPoint(float_type point[2])
  {
      point[0] = this->CurrentEndPoint[0];
      point[1] = this->CurrentEndPoint[1];
  }

  void SetLastControlPoint(float_type x, float_type y)
  {
    this->LastControlPoint[0] = x;
    this->LastControlPoint[1] = y;
  }

  void SetLastControlPoint(float_type point[2])
  {
      this->LastControlPoint[0] = point[0];
      this->LastControlPoint[1] = point[1];
  }

  void GetLastControlPoint(float_type point[2])
  {
      point[0] = this->LastControlPoint[0];
      point[1] = this->LastControlPoint[1];
  }

  void Reset()
  {
    this->LastCommandType = 'M';
    this->SetCurrentEndPoint(0, 0);
    this->SetLastControlPoint(0, 0);
  }

  char LastCommandType;
  float_type CurrentEndPoint[2];
  float_type LastControlPoint[2];
};

//------------------------------------------------------------------------------
// Description:
// This is an abstract class which represents a generic SVG path command.
// It provides properties and behaviors common to all SVG path command classes.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathCommand : public vtkObject
{
public:
  vtkTypeMacro(vtkSVGPathCommand, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  // In this case it returns simply a space.
  virtual char GetType() { return ' '; }

  // Description:
  // This is a static method which create a new SVG path command by passing
  // the command type as a char. If the passed char is lower case a relative
  // command is created.
  static vtkSVGPathCommand* NewCommand(char t);

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathCommand* DeepCopy();

  // Description:
  // This pair of methods are used for set/get the context used by some
  // `vtkSVGPath` method while processing the command.
  // Some methods of the SVG path command class hierarchy need that a context
  // is set before being invoked.
  vtkSetMacro(Context, vtkSVGPathContext*);
  vtkGetMacro(Context, vtkSVGPathContext*);

  // Description:
  // Return true if a context has been set.
  bool HasContext()
  {
    return (this->Context != NULL);
  }

  // Description:
  // The following methods are used for set/get the end point of an SVG path
  // segment defined by a command. For the `closePath` command the end point is
  // the start point of the current sub-path.
  void SetEndPoint(float_type x, float_type y);
  void SetEndPoint(float_type point[2]);
  void GetEndPoint(float_type point[2]);

  // Description:
  // This methods are used for configuring the SVG path command as absolute
  // or relative.
  vtkSetMacro(UseAbsoluteCoordinate, bool);
  vtkGetMacro(UseAbsoluteCoordinate, bool);
  vtkBooleanMacro(UseAbsoluteCoordinate, bool);

  // Description:
  // Return true if this is an absolute command.
  bool IsAbsoluteCommand()
  {
    return this->GetUseAbsoluteCoordinate();
  }

  // Description:
  // Return true if this is a relative command.
  bool IsRelativeCommand()
  {
    return !this->GetUseAbsoluteCoordinate();
  }

  // Description:
  // Transform this command in an absolute command.
  // In order to work correctly it needs that the starting point of the path
  // segment defined by this command is passed or that the path context has
  // been set.
  // The method returns a pointer to the start point used for the transformation.
  virtual float_type* ToAbsoluteCommand(float_type startPoint[2] = NULL);

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int /*precision*/ = -1) { return std::string(); }

  // Description:
  // This method returns by the `vertices` passed parameter an array of
  // 2D points representing the vertices of a polyline which approximate
  // the path segment defined by the given command.
  // All vertices of the approximating polyline are inserted into the returned
  // array but the segment start point.
  // For `moveTo`, `lineTo`, `lineToHorizontal`, `lineToVertical`, `closePath`
  // and `curveToQuadraticSmooth` commands the `vertices` array contains
  // the end point only.
  // This method requires that a path context has been set before being invoked.
  virtual void GetVertices(vtkPoints2D* vertices);

protected:
  vtkSVGPathCommand();
  ~vtkSVGPathCommand();

protected:
  vtkSVGPathContext* Context;
  bool UseAbsoluteCoordinate;
  float_type EndPoint[2];

private:
  vtkSVGPathCommand(const vtkSVGPathCommand& );  // Not implemented.
  void operator=(const vtkSVGPathCommand& );  // Not implemented.
};

// Description:
// Returns true if and only if the two arguments are equal.
bool operator==(vtkSVGPathCommand& left, vtkSVGPathCommand& right);

//------------------------------------------------------------------------------
// Description:
// This class represent a `moveTo` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathMoveTo : public vtkSVGPathCommand
{
public:
  static vtkSVGPathMoveTo *New();
  vtkTypeMacro(vtkSVGPathMoveTo, vtkSVGPathCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'M'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathMoveTo* DeepCopy();

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

protected:
  vtkSVGPathMoveTo();
  ~vtkSVGPathMoveTo();

private:
  vtkSVGPathMoveTo(const vtkSVGPathMoveTo& );  // Not implemented.
  void operator=(const vtkSVGPathMoveTo& );  // Not implemented.
};

//------------------------------------------------------------------------------
// Description:
// This class represent a `lineTo` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathLineTo : public vtkSVGPathCommand
{
public:
  static vtkSVGPathLineTo *New();
  vtkTypeMacro(vtkSVGPathLineTo, vtkSVGPathCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'L'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathLineTo* DeepCopy();

  // Description:
  // Create a new `lineTo` command which is the absolute version of this command.
  // In order to work correctly it needs that the starting point of the path
  // segment defined by this command is passed or that the path context has
  // been set.
  // The method returns a pointer to the new created command.
  // The deletion of the returned `lineTo` command instance is up to the
  // method caller.
  virtual vtkSVGPathLineTo* GetAbsoluteLineTo(float_type startPoint[2] = NULL)
  {
    return this->GetAbsoluteLineToImpl(-1, startPoint);
  }

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

  // Description:
  // This method returns by the `vertices` passed parameter an array of
  // 2D points representing the vertices of a polyline which approximate
  // the path segment defined by the given command.
  // All vertices of the approximating polyline are inserted into the returned
  // array but the segment start point.
  // For a `lineTo` command the `vertices` array contains only the end point.
  // This method requires that a path context has been set before being invoked.
  virtual void GetVertices(vtkPoints2D* vertices);

protected:
  vtkSVGPathLineTo();
  ~vtkSVGPathLineTo();

  vtkSVGPathLineTo* GetAbsoluteLineToImpl(int orientation, float_type startPoint[2]);

private:
  vtkSVGPathLineTo(const vtkSVGPathLineTo& );  // Not implemented.
  void operator=(const vtkSVGPathLineTo& );  // Not implemented.
};

//------------------------------------------------------------------------------
// Description:
// This class represent a horizontal `lineTo` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathHLineTo : public vtkSVGPathLineTo
{
public:
  static vtkSVGPathHLineTo *New();
  vtkTypeMacro(vtkSVGPathHLineTo, vtkSVGPathLineTo);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'H'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathHLineTo* DeepCopy();

  // Description:
  // Create a new `lineTo` command which is equivalent to the absolute version
  // of this command.
  // In order to work correctly it needs that the starting point of the path
  // segment defined by this command is passed or that the path context has
  // been set.
  // The method returns a pointer to the new created command.
  // The deletion of the returned `lineTo` command instance is up to the
  // method caller.
  virtual vtkSVGPathLineTo* GetAbsoluteLineTo(float_type startPoint[2] = NULL);

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

protected:
  vtkSVGPathHLineTo();
  ~vtkSVGPathHLineTo();

private:
  vtkSVGPathHLineTo(const vtkSVGPathHLineTo& );  // Not implemented.
  void operator=(const vtkSVGPathHLineTo& );  // Not implemented.
};

//------------------------------------------------------------------------------
// Description:
// This class represent a vertical `lineTo` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathVLineTo : public vtkSVGPathLineTo
{
public:
  static vtkSVGPathVLineTo *New();
  vtkTypeMacro(vtkSVGPathVLineTo, vtkSVGPathLineTo);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'V'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathVLineTo* DeepCopy();

  // Description:
  // Create a new `lineTo` command which is equivalent to the absolute version
  // of this command.
  // In order to work correctly it needs that the starting point of the path
  // segment defined by this command is passed or that the path context has
  // been set.
  // The method returns a pointer to the new created command.
  // The deletion of the returned `lineTo` command instance is up to the
  // method caller.
  virtual vtkSVGPathLineTo* GetAbsoluteLineTo(float_type startPoint[2] = NULL);

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

protected:
  vtkSVGPathVLineTo();
  ~vtkSVGPathVLineTo();

private:
  vtkSVGPathVLineTo(const vtkSVGPathVLineTo& );  // Not implemented.
  void operator=(const vtkSVGPathVLineTo& );  // Not implemented.
};

//------------------------------------------------------------------------------
// Description:
// This class represent a `arcTo` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathArcTo : public vtkSVGPathCommand
{
public:
  static vtkSVGPathArcTo *New();
  vtkTypeMacro(vtkSVGPathArcTo, vtkSVGPathCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'A'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathArcTo* DeepCopy();

  // Description:
  // These methods set/get the ray along the X axis of the ellipse this arc
  // belong to.
  vtkSetMacro(RayX, float_type);
  vtkGetMacro(RayX, float_type);

  // Description:
  // These methods set/get the ray along the Y axis of the ellipse this arc
  // belong to.
  vtkSetMacro(RayY, float_type);
  vtkGetMacro(RayY, float_type);

  // Description:
  // These methods set/get the angle by which is rotated the X axis of
  // the ellipse this arc belong to.
  // The angle is represented in degree.
  vtkSetMacro(XAxisRotation, int);
  vtkGetMacro(XAxisRotation, int);

  // Description:
  // These methods set/get the `large arc` boolean flag which is used to select
  // the larger or the smaller arc which is compatible with this command.
  vtkSetMacro(LargeArcFlag, bool);
  vtkGetMacro(LargeArcFlag, bool);
  vtkBooleanMacro(LargeArcFlag, bool);

  // Description:
  // These methods set/get the `sweep` boolean flag which is used to select
  // the ccw or the cw arc which is compatible with this command.
  vtkSetMacro(SweepFlag, bool);
  vtkGetMacro(SweepFlag, bool);
  vtkBooleanMacro(SweepFlag, bool);

  // Description
  // This method is used for setting all command arguments at once.
  void SetAll(float_type rayX, float_type rayY, int rotAngle,
              bool largeArcFlag, bool sweepFlag,
              float_type endX, float_type endY);

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

  // Description:
  // This method returns by the `vertices` passed parameter an array of
  // 2D points representing the vertices of a polyline which approximate
  // the path segment defined by the given command.
  // All vertices of the approximating polyline are inserted into the returned
  // array but the segment start point.
  // This method requires that a path context has been set before being invoked.
  virtual void GetVertices(vtkPoints2D* vertices);

protected:
  vtkSVGPathArcTo();
  ~vtkSVGPathArcTo();

protected:
  float_type RayX;
  float_type RayY;
  int XAxisRotation; // Degree
  bool LargeArcFlag;
  bool SweepFlag;

protected:
  typedef std::vector<vtkArcSegmentData> SegmentDataArray;
  typedef std::vector<vtkCubicBezierCurve> BezierCurveArray;
  typedef std::map<vtkArcKey, BezierCurveArray> ArcToBezierCurvesCacheType;

  // This cache is used for avoiding to compute the set of Bezier
  // curves used for approximating a given arc, again.
  static ArcToBezierCurvesCacheType ArcToBezierCurvesCache;

  void ArcToSegments(SegmentDataArray& segments,
                     const vtkSVGPathArcTo& arc,
                     float_type ox, float_type oy);

  void SegmentToBezier(vtkCubicBezierCurve& curve,
                       const vtkArcSegmentData& seg,
                       float_type startX, float_type startY);

  BezierCurveArray& ToBezierCurves(vtkSVGPathArcTo* arc,
                                   float_type startX, float_type startY);

private:
  vtkSVGPathArcTo(const vtkSVGPathArcTo& );  // Not implemented.
  void operator=(const vtkSVGPathArcTo& );  // Not implemented.
};

//------------------------------------------------------------------------------
// Description:
// This class represent a quadratic `curveTo` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathQuadraticCurveTo : public vtkSVGPathCommand
{
public:
  static vtkSVGPathQuadraticCurveTo *New();
  vtkTypeMacro(vtkSVGPathQuadraticCurveTo, vtkSVGPathCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'Q'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathQuadraticCurveTo* DeepCopy();

  // Description:
  // This methods are used for set/get the only control point of the curve.
  void SetControlPoint(float_type x, float_type y);
  void SetControlPoint(float_type point[2]);
  void GetControlPoint(float_type point[2]);

  // Description:
  // This method is used for setting all command arguments at once.
  void SetAll(float_type controlPointX, float_type controlPointY, float_type endX, float_type endY);

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

  // Description:
  // Transform this command in an absolute command.
  // In order to work correctly it needs that the starting point of the path
  // segment defined by this command is passed or that the path context has
  // been set.
  // The method returns a pointer to the start point used for the transformation.
  virtual float_type* ToAbsoluteCommand(float_type startPoint[2] = NULL);

  // Description:
  // This method returns by the `vertices` passed parameter an array of
  // 2D points representing the vertices of a polyline which approximates
  // the path segment defined by the given command.
  // All vertices of the approximating polyline are inserted into the returned
  // array but the segment start point.
  // This method requires that a path context has been set before being invoked.
  virtual void GetVertices(vtkPoints2D* vertices);

protected:
  vtkSVGPathQuadraticCurveTo();
  ~vtkSVGPathQuadraticCurveTo();

protected:
  float_type ControlPoint[2];

private:
  vtkSVGPathQuadraticCurveTo(const vtkSVGPathQuadraticCurveTo& );  // Not implemented.
  void operator=(const vtkSVGPathQuadraticCurveTo& );  // Not implemented.
};

//------------------------------------------------------------------------------
// Description:
// This class represent a quadratic smooth `curveTo` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathSmoothQuadraticCurveTo : public vtkSVGPathCommand
{
public:
  static vtkSVGPathSmoothQuadraticCurveTo *New();
  vtkTypeMacro(vtkSVGPathSmoothQuadraticCurveTo, vtkSVGPathCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'T'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathSmoothQuadraticCurveTo* DeepCopy();

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

  // Description:
  // Create a new quadratic `curveTo` command which is equivalent to
  // the absolute version of this command.
  // In order to work correctly it needs that the path context has been set.
  // The method returns a pointer to the new created command.
  // The deletion of the returned command instance is up to the method caller.
  vtkSVGPathQuadraticCurveTo* GetAbsoluteQuadraticCurveTo();

  // Description:
  // This method returns by the `vertices` passed parameter an array of
  // 2D points representing the vertices of a polyline which approximates
  // the path segment defined by the given command.
  // All vertices of the approximating polyline are inserted into the returned
  // array but the segment start point.
  // This method requires that a path context has been set before being invoked.
  virtual void GetVertices(vtkPoints2D* vertices);

protected:
  vtkSVGPathSmoothQuadraticCurveTo();
  ~vtkSVGPathSmoothQuadraticCurveTo();

private:
  vtkSVGPathSmoothQuadraticCurveTo(const vtkSVGPathSmoothQuadraticCurveTo& );  // Not implemented.
  void operator=(const vtkSVGPathSmoothQuadraticCurveTo& );  // Not implemented.
};

//------------------------------------------------------------------------------
// Description:
// This class represent a cubic `curveTo` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathCubicCurveTo : public vtkSVGPathCommand
{
public:
  static vtkSVGPathCubicCurveTo *New();
  vtkTypeMacro(vtkSVGPathCubicCurveTo, vtkSVGPathCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'C'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathCubicCurveTo* DeepCopy();

  // Description:
  // Get/Set the first control point of this cubic curve.
  void SetFirstControlPoint(float_type x, float_type y);
  void SetFirstControlPoint(float_type point[2]);
  void GetFirstControlPoint(float_type point[2]);

  // Description:
  // Get/Set the second control point of this cubic curve.
  void SetSecondControlPoint(float_type x, float_type y);
  void SetSecondControlPoint(float_type point[2]);
  void GetSecondControlPoint(float_type point[2]);

  // Description
  // This method is used for setting all command arguments at once.
  void SetAll(float_type firstX, float_type firstY,
              float_type secondX, float_type secondY,
              float_type endX, float_type endY);

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

  // Description:
  // Transform this command in an absolute command.
  // In order to work correctly it needs that the starting point of the path
  // segment defined by this command is passed or that the path context has
  // been set.
  // The method returns a pointer to the start point used for the transformation.
  virtual float_type* ToAbsoluteCommand(float_type startPoint[2] = NULL);

  // Description:
  // This method returns by the `vertices` passed parameter an array of
  // 2D points representing the vertices of a polyline which approximates
  // the path segment defined by the given command.
  // All vertices of the approximating polyline are inserted into the returned
  // array but the segment start point.
  // This method requires that a path context has been set before being invoked.
  virtual void GetVertices(vtkPoints2D* vertices);

protected:
  vtkSVGPathCubicCurveTo();
  ~vtkSVGPathCubicCurveTo();

protected:
  float_type FirstControlPoint[2];
  float_type SecondControlPoint[2];

private:
  vtkSVGPathCubicCurveTo(const vtkSVGPathCubicCurveTo& );  // Not implemented.
  void operator=(const vtkSVGPathCubicCurveTo& );  // Not implemented.
};

//------------------------------------------------------------------------------
// Description:
// This class represent a cubic smooth `curveTo` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathSmoothCubicCurveTo : public vtkSVGPathCubicCurveTo
{
public:
  static vtkSVGPathSmoothCubicCurveTo *New();
  vtkTypeMacro(vtkSVGPathSmoothCubicCurveTo, vtkSVGPathCubicCurveTo);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'S'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathSmoothCubicCurveTo* DeepCopy();

  // Description
  // This method is used for setting all command arguments at once.
  void SetAll(float_type secondX, float_type secondY,
              float_type endX, float_type endY);

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

  // Description:
  // Create a new cubic `curveTo` command which is equivalent to
  // the absolute version of this command.
  // In order to work correctly it needs that the path context has been set.
  // The method returns a pointer to the new created command.
  // The deletion of the returned command instance is up to the method caller.
  vtkSVGPathCubicCurveTo* GetAbsoluteCubicCurveTo();

  // Description:
  // This method returns by the `vertices` passed parameter an array of
  // 2D points representing the vertices of a polyline which approximates
  // the path segment defined by the given command.
  // All vertices of the approximating polyline are inserted into the returned
  // array but the segment start point.
  // This method requires that a path context has been set before being invoked.
  virtual void GetVertices(vtkPoints2D* vertices);

protected:
  vtkSVGPathSmoothCubicCurveTo();
  ~vtkSVGPathSmoothCubicCurveTo();

private:
  void SetFirstControlPoint(float_type x, float_type y); // Not implemented.
  void SetFirstControlPoint(float_type point[2]); // Not implemented.
  void GetFirstControlPoint(float_type point[2]); // Not implemented.

  void SetAll(float_type firstX, float_type firstY,
              float_type secondX, float_type secondY,
              float_type endX, float_type endY); // Not implemented.

private:
  vtkSVGPathSmoothCubicCurveTo(const vtkSVGPathSmoothCubicCurveTo& );  // Not implemented.
  void operator=(const vtkSVGPathSmoothCubicCurveTo& );  // Not implemented.
};

//------------------------------------------------------------------------------
// Description:
// This class represent a `closePath` SVG path command.
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPathClosePath : public vtkSVGPathCommand
{
public:
  static vtkSVGPathClosePath *New();
  vtkTypeMacro(vtkSVGPathClosePath, vtkSVGPathCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

public:
  // Description:
  // This function returns the type of the SVG command as an upper case char.
  virtual char GetType() { return 'Z'; }

  // Description:
  // Perform a deep exact copy of the current command instance.
  vtkSVGPathClosePath* DeepCopy();

  // Description:
  // Return a valid SVG path command string representing this command.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  virtual std::string ToString(int precision = -1);

  // Description:
  // Transform this command in an absolute command.
  // For the `closePath` command it does nothing and returns NULL.
  virtual float_type* ToAbsoluteCommand(float_type startPoint[2] = NULL);

protected:
  vtkSVGPathClosePath();
  ~vtkSVGPathClosePath();

private:
  vtkSVGPathClosePath(const vtkSVGPathClosePath& );  // Not implemented.
  void operator=(const vtkSVGPathClosePath& );  // Not implemented.
};


//------------------------------------------------------------------------------
class VTKCHARTSVEGARENDERER_EXPORT vtkSVGPath : public vtkObject
{
public:
  typedef std::vector< vtkSmartPointer<vtkPoints2D> > ContourArrayType;

public:
  static vtkSVGPath *New();
  vtkTypeMacro(vtkSVGPath, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Return the number of SVG path commands this path is made up of.
  unsigned int GetNumberOfCommands();

  // Description:
  // Replace the command at position `i` with the passed command.
  // A range check on the passed index is performed.
  void SetCommand(unsigned int i, vtkSVGPathCommand* command);

  // Return a pointer to the command at position `i` or NULL if `i` greater
  // or equal to the number of commands.
  vtkSVGPathCommand* GetCommand(unsigned int i);

  // Description:
  // Insert the passed command before the command at position `i`.
  // A range check on the passed index is performed.
  // If `i` is equal to the current number of commands the passed command is
  // appended to the path.
  void InsertCommand(unsigned int i, vtkSVGPathCommand* command);

  // Description:
  // Create a new command of type `type` with arguments the passed arguments
  // and insert it before the command at position `i`.
  // A range check on the passed index is performed. If `i` is equal to
  // the current number of commands the created command is appended to the path.
  void InsertCommand(unsigned int i, char type);
  void InsertCommand(unsigned int i, char type, float_type end);
  void InsertCommand(unsigned int i, char type,
                     float_type endX, float_type endY);
  void InsertCommand(unsigned int i, char type,
                     float_type controlPointX, float_type controlPointY,
                     float_type endX, float_type endY);
  void InsertCommand(unsigned int i, char type,
                     float_type firstX, float_type firstY,
                     float_type secondX, float_type secondY,
                     float_type endX, float_type endY);
  void InsertCommand(unsigned int i, char type,
                     float_type rayX, float_type rayY, int rotAngle,
                     bool largeArcFlag, bool sweepFlag,
                     float_type endX, float_type endY);

  // Description:
  // Append the passed command to the path.
  void AppendCommand(vtkSVGPathCommand* command);

  // Description:
  // Create a new command of type `type` with arguments the passed arguments
  // and append it to the path.
  void AppendCommand(char type);
  void AppendCommand(char type, float_type end);
  void AppendCommand(char type, float_type endX, float_type endY);
  void AppendCommand(char type,
                     float_type controlPointX, float_type controlPointY,
                     float_type endX, float_type endY);
  void AppendCommand(char type,
                     float_type firstX, float_type firstY,
                     float_type secondX, float_type secondY,
                     float_type endX, float_type endY);
  void AppendCommand(char type,
                     float_type rayX, float_type rayY, int rotAngle,
                     bool largeArcFlag, bool sweepFlag,
                     float_type endX, float_type endY);

  // Description:
  // Remove the command at position `i`, if any.
  // Return true if the remove operation is successful.
  bool RemoveCommand(unsigned int i);

  // Description:
  // Remove all commands from the path.
  void RemoveAllCommands();

  // Description:
  // Remove redundant commands, such as consecutive `moveTo` or `closePath`
  // commands.
  void Simplify();

  // Description:
  // The passed argument `commandstr` must be an SVG path string representing
  // one or more commands. For each command defined in the passed argument
  // a new command instance is created and the sequence of commands is inserted
  // before the command at position `i`.
  // A range check on the passed index is performed.
  // If `i` is equal to the current number of commands the commands are
  // appended to the path.
  // Note that, at present, the parsing skill of this method are limited.
  // Point coordinates must be separated by a comma.
  // Support for shortcuts, such as "L 10,10 20,30" are not available.
  // You have to write "L 10,10 L 20,30".
  void InsertCommandsFromString(unsigned int i, const std::string& commandstr);

  // Description:
  // The passed argument `commandstr` must be an SVG path string representing
  // one or more commands. For each command defined in the passed argument
  // a new command instance is appended to the path.
  // Note that, at present, the parsing skill of this method are limited.
  // Point coordinates must be separated by a comma.
  // Support for shortcuts, such as "L 10,10 20,30" are not available.
  // You have to write "L 10,10 L 20,30".
  void AppendCommandsFromString(const std::string& commandstr);

  // Description:
  // Return a valid SVG path command string representing this path.
  // The `precision` argument if passed represent the precision used for
  // representing the point coordinates. Default is to not use a fixed
  // precision.
  std::string ToString(int precision = -1);

  // Description:
  // Return the number of sub-paths included into this path.
  // A new sub-path starts after a `closePath` command or with a `moveTo`
  // command, except when the `moveTo` command move to the current point.
  // This means that for getting the real number of sub-paths a call to
  // `Simplify` must be performed, before invoking this method.
  unsigned int GetNumberOfSubPaths();

  // Description:
  // Through the `contours` parameter, this method returns a collection of
  // arrays of 2D points. Each array contains the vertices of a polyline
  // approximating a sub-path included into this path instance.
  // A sub-path is closed if and only if the first and the last vertices
  // in the associated array are equal.
  void GetContours(ContourArrayType& contours);

protected:
  vtkSVGPath();
  ~vtkSVGPath();

protected:
  typedef std::vector< vtkSmartPointer<vtkSVGPathCommand> > CommandListType;
  CommandListType CommandList;
  vtkSVGPathContext CurrentContext;

private:
  vtkSVGPath(const vtkSVGPath& );  // Not implemented.
  void operator=(const vtkSVGPath& );  // Not implemented.
};

#endif
