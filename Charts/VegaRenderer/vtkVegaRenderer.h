/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaRenderer.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVegaRenderer - Render a Vega scene starting from a scene
// representation in JSON format as a string .
// .SECTION Description
// vtkVegaRenderer

#ifndef __vtkVegaRenderer_h
#define __vtkVegaRenderer_h

#include "vtkChartsVegaRendererModule.h" // For export macro
#include "vtkObject.h"
#include "vtkSmartPointer.h"

#include <string>


namespace Json
{
  class Value;
}

class vtkVegaSceneItem;
class vtkVegaArcItem;
class vtkVegaAreaItem;
class vtkVegaGroupItem;
class vtkVegaImageItem;
class vtkVegaLineItem;
class vtkVegaPathItem;
class vtkVegaRectItem;
class vtkVegaRuleItem;
class vtkVegaSymbolItem;
class vtkVegaTextItem;
class vtkNamedColors;
struct DefaultValues;


class VTKCHARTSVEGARENDERER_EXPORT vtkVegaRenderer : public vtkObject
{
public:
  static vtkVegaRenderer *New();
  vtkTypeMacro(vtkVegaRenderer, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  std::string GetInputSceneString() const;
  void SetInputSceneString(const std::string& scene);

  void SetInputScene(const Json::Value& root);
  const Json::Value& GetInputScene() const;

  vtkSetStringMacro(BaseDataURL);
  vtkGetStringMacro(BaseDataURL);

  virtual void Update();
  vtkVegaSceneItem* GetSceneItem();

protected:
  vtkVegaRenderer();
  ~vtkVegaRenderer();

  void UpdateDispatch(vtkVegaGroupItem* parentGroup,
                      const Json::Value& sceneFragment,
                      int index);
  void UpdateGroupItem(vtkVegaGroupItem* parentGroup,
                       const Json::Value& sceneFragment,
                       int index);
  template<typename MarkItemType>
  bool UpdateMarkItem(vtkVegaGroupItem* parentGroup,
                      const Json::Value& sceneFragment,
                      int index);
  bool UpdateProperties(const Json::Value& scene, vtkVegaSceneItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaArcItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaAreaItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaImageItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaLineItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaPathItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaRectItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaRuleItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaSymbolItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaTextItem* item);

protected:
  Json::Value* InputScene;
  bool InputSceneModified;
  char *BaseDataURL;
  vtkSmartPointer<vtkVegaSceneItem> SceneItem;
  vtkSmartPointer<vtkNamedColors> StringColorMapper;
  DefaultValues* Defaults;

private:
  vtkVegaRenderer(const vtkVegaRenderer&);  // Not implemented.
  void operator=(const vtkVegaRenderer&);  // Not implemented.
};


#endif
