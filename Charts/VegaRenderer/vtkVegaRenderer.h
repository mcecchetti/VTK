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
// vtkVegaRenderer is responsible for mapping a Vega scene graph in JSON format
// to a scene graph made up of objects whose type is a subclass of
// vtkVegaMarkItem.
// The generated scene graph is returned as a pointer to an object of type
// vtkVegaSceneItem which represent the root of the scene graph.

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

  // Description:
  // Return a string representation in JSON format of the scene graph.
  std::string GetInputSceneString() const;

  // Description:
  // This method receives a string as input, which represents a Vega scene
  // graph in JSON format. The method parses the passed string argument and
  // generates a JSON data structure which is stored in a private data member
  // of this object.
  void SetInputSceneString(const std::string& scene);

  // Description:
  // These methods are used for get/set the internal JSON data structure
  // where the Vega scene graph is stored.
  void SetInputScene(const Json::Value& root);
  const Json::Value& GetInputScene() const;

  // Description:
  // Methods for get/set a base url for data retrieving, such as images.
  vtkSetStringMacro(BaseDataURL);
  vtkGetStringMacro(BaseDataURL);

  // Description:
  // This is the core method of the renderer class. Walking through the Vega
  // scene graph structure, produced by SetInputSceneString, it generates
  // a scene graph whose elements are instances of some subtype of the
  // vtkVegaMarkItem class. An object of type vtkVegaSceneItem, which represents
  // the root of the scene graph, is stored in a private data member of
  // this object and can be retrieved through the GetSceneItem method.
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
  void UpdateProperties(const Json::Value& sceneItems, vtkVegaAreaItem* item);
  void UpdateProperties(const Json::Value& sceneItem, vtkVegaImageItem* item);
  void UpdateProperties(const Json::Value& sceneItems, vtkVegaLineItem* item);
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
