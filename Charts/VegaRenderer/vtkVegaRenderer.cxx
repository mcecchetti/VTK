/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVegaRenderer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkVegaRenderer.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkNamedColors.h"
#include "vtkSVGPath.h"
#include "vtkVegaMarkItemCollection.h"
#include "vtkVegaSceneItem.h"
#include "vtkVegaArcItem.h"
#include "vtkVegaAreaItem.h"
#include "vtkVegaGroupItem.h"
#include "vtkVegaImageItem.h"
#include "vtkVegaLineItem.h"
#include "vtkVegaPathItem.h"
#include "vtkVegaRuleItem.h"
#include "vtkVegaSymbolItem.h"
#include "vtkVegaTextItem.h"

#include "vtkPen.h"
#include "vtkBrush.h"
#include "vtkTextProperty.h"

#include "vtk_jsoncpp.h"

#include <cstdlib>  // Header to ensure std::strtoul is present
#include <cctype>   // Header to ensure std::tolower is present
#include <sstream>


//------------------------------------------------------------------------------
struct DefaultValues
{
  DefaultValues();

  Json::Value X;
  Json::Value Y;
  Json::Value X2;
  Json::Value Y2;
  Json::Value Width;
  Json::Value Height;
  Json::Value OuterRadius;
  Json::Value InnerRadius;
  Json::Value StartAngle;
  Json::Value EndAngle;
  Json::Value TextAngle;
  Json::Value TextOffsetX;
  Json::Value TextOffsetY;
  Json::Value TextOffsetRadius;
  Json::Value TextOffsetAngle;

  Json::Value Opacity;
  Json::Value FillColor;
  Json::Value FillOpacity;
  Json::Value StrokeColor;
  Json::Value StrokeOpacity;
  Json::Value StrokeWidth;

  Json::Value FontFamily;
  Json::Value FontSize;
  Json::Value FontStyle;
  Json::Value FontWeight;
  Json::Value TextAlign;
  Json::Value TextBaseline;
  Json::Value Text;

  Json::Value ImageAlign;
  Json::Value ImageBaseline;
  Json::Value Url;

  Json::Value Size;
  Json::Value Shape;
};


//------------------------------------------------------------------------------
DefaultValues::DefaultValues()
  : X(0.0),
    Y(0.0),
    X2(0.0),
    Y2(0.0),
    Width(0.0),
    Height(0.0),
    OuterRadius(0.0),
    InnerRadius(0.0),
    StartAngle(0.0),
    EndAngle(0.0),
    TextAngle(0.0),
    TextOffsetX(0.0),
    TextOffsetY(0.0),
    TextOffsetRadius(0.0),
    TextOffsetAngle(0.0),
    Opacity(1.0),
    FillColor("rgba(0, 0, 0, 0"),
    FillOpacity(1.0),
    StrokeColor("rgba(0, 0, 0, 0"),
    StrokeOpacity(1.0),
    StrokeWidth(1.0),
    FontFamily("sans-serif"),
    FontSize(12),
    FontStyle("normal"),
    FontWeight("normal"),
    TextAlign("left"),
    TextBaseline("middle"),
    Text(""),
    ImageAlign("left"),
    ImageBaseline("top"),
    Url(""),
    Size(100.0),
    Shape("circle")
{
}


//------------------------------------------------------------------------------
static int GetMarkType(const char* name)
{
  static const char* MarkTypes[] = {
      "",
      "arc",
      "area",
      "group",
      "image",
      "line",
      "path",
      "rect",
      "rule",
      "symbol",
      "text"
  };

  int i;
  for (i = 1; i < vtkVegaMarkItem::UNKNOWN; ++i)
    {
    if (name == strstr(name, MarkTypes[i]))
      {
      break;
      }
    }
  return i;
}


//------------------------------------------------------------------------------
static bool IsMultiDataSingleItem(int markType)
{
  switch (markType)
    {
    case vtkVegaMarkItem::AREA:
    case vtkVegaMarkItem::LINE:
      {
      return true;
      }
    default:
      {
      return false;
      }
    }
}


//------------------------------------------------------------------------------
static std::string RemoveTrailingSpaces(const std::string& str)
{
  if (str.empty())
    {
    return str;
    }

  std::string::size_type start = str.find_first_not_of(" \t");
  if (start == std::string::npos)
    {
    start = 0;
    }

  std::string::size_type end = str.find_last_not_of(" \t");
  if (end == std::string::npos)
    {
    end = str.size() - 1;
    }

  return str.substr(start, end - start + 1);
}


//------------------------------------------------------------------------------
static bool IsUnsignedInt(const std::string& str)
{
  std::string s = RemoveTrailingSpaces(str);
  return (!s.empty() && s.find_first_not_of("0123456789") == std::string::npos);
}


//------------------------------------------------------------------------------
static std::string ToLowerCase(const std::string& str)
{
  unsigned int n = str.size();
  std::string s;
  s.reserve(n);
  for (unsigned int i = 0; i < n; ++i)
    {
    s.push_back(std::tolower(str[i]));
    }
  return s;
}


//------------------------------------------------------------------------------
static vtkSVGPath* MakePathFromCache(const Json::Value& pathCache)
{
  vtkSVGPath* path = vtkSVGPath::New();

  unsigned int n = pathCache.size();
  for (unsigned int i = 0; i < n; ++i)
    {
    const Json::Value& currentData = pathCache[i];
    unsigned char type = *(currentData[0].asCString());
    switch (type)
      {
      case 'Z':
        {
        path->AppendCommand(type);
        break;
        }
      case 'h':
      case 'H':
        {
        float e = currentData[1].asFloat();
        path->AppendCommand(type, e);
        break;
        }
      case 'v':
      case 'V':
        {
        float e = currentData[1].asFloat();
        path->AppendCommand(type, e);
        break;
        }
      case 'l':
      case 'L':
      case 'm':
      case 'M':
      case 't':
      case 'T':
        {
        float ex = currentData[1].asFloat();
        float ey = currentData[2].asFloat();
        path->AppendCommand(type, ex, ey);
        break;
        }
      case 'q':
      case 'Q':
      case 's':
      case 'S':
        {
        float cx = currentData[1].asFloat();
        float cy = currentData[2].asFloat();
        float ex = currentData[3].asFloat();
        float ey = currentData[4].asFloat();
        path->AppendCommand(type, cx, cy, ex, ey);
        break;
        }
      case 'c':
      case 'C':
        {
        float c1x = currentData[1].asFloat();
        float c1y = currentData[2].asFloat();
        float c2x = currentData[3].asFloat();
        float c2y = currentData[4].asFloat();
        float ex = currentData[5].asFloat();
        float ey = currentData[6].asFloat();
        path->AppendCommand(type, c1x, c1y, c2x, c2y, ex, ey);
        break;
        }
      case 'a':
      case 'A':
        {
        float cx = currentData[1].asFloat();
        float cy = currentData[2].asFloat();
        int a = currentData[2].asInt();
        bool lf = currentData[4].asBool();
        bool sf = currentData[5].asBool();
        float ex = currentData[6].asFloat();
        float ey = currentData[7].asFloat();
        path->AppendCommand(type, cx, cy, a, lf, sf, ex, ey);
        break;
        }
      }
    }
  return path;
}


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVegaRenderer);


//------------------------------------------------------------------------------
void vtkVegaRenderer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//------------------------------------------------------------------------------
vtkVegaRenderer::vtkVegaRenderer()
{
  this->InputScene = NULL;
  this->InputSceneModified = false;
  this->BaseDataURL = NULL;
  this->SceneItem = vtkSmartPointer<vtkVegaSceneItem>::New();
  this->StringColorMapper = vtkSmartPointer<vtkNamedColors>::New();
  this->Defaults = new DefaultValues();
}


//------------------------------------------------------------------------------
vtkVegaRenderer::~vtkVegaRenderer()
{
  if (this->Defaults)
    {
    delete (this->Defaults);
    }

  if (this->InputScene)
    {
    delete (this->InputScene);
    }
  if (this->BaseDataURL)
    {
    delete (this->BaseDataURL);
    }
}


//------------------------------------------------------------------------------
std::string vtkVegaRenderer::GetInputSceneString() const
{
  if (this->InputScene)
    {
    Json::StyledWriter writer;
    return writer.write(*this->InputScene);
    }
  else
    {
    return std::string();
    }
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::SetInputSceneString(const std::string& scene)
{
  if (this->Debug)
    {
    vtkDebugMacro(<< "SetInputSceneString: " << scene << "\n");
    }

  if (scene.empty())
    {
    return;
    }
  Json::Reader reader;
  Json::Value* root = new Json::Value();
  if (root && reader.parse(scene, *root, false))
    {
    if (!this->InputScene || !this->InputScene->compare(*root))
      {
      if (this->InputScene)
        {
        delete (this->InputScene);
        }
      this->InputScene = root;
      this->InputSceneModified = true;
      this->Modified();
      }
    else
      {
      delete root;
      }
    }
  else
    {
    if (root)
      {
      delete root;
      }
    vtkErrorMacro(
        << "Vega scene parsing is failed.\n"
        << reader.getFormattedErrorMessages() << "\n");
    }
}


//------------------------------------------------------------------------------
const Json::Value& vtkVegaRenderer::GetInputScene() const
{
  return (*this->InputScene);
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::SetInputScene(const Json::Value& root)
{
  if (!this->InputScene || !this->InputScene->compare(root))
    {
    (*this->InputScene) = root;
    this->InputSceneModified = true;
    this->Modified();
    }
}


//------------------------------------------------------------------------------
vtkVegaSceneItem* vtkVegaRenderer::GetSceneItem()
{
  return this->SceneItem.GetPointer();
}

//------------------------------------------------------------------------------
void vtkVegaRenderer::Update()
{
  if (this->InputSceneModified)
    {
    if (this->UpdateProperties(this->GetInputScene(), this->SceneItem))
      {
      this->UpdateDispatch(this->SceneItem, this->GetInputScene(), 0);
      }
    this->InputSceneModified = false;
    }
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateDispatch(vtkVegaGroupItem* parentGroup,
                                     const Json::Value& sceneFragment,
                                     int index)
{
  if (sceneFragment.isObject() && sceneFragment.isMember("marktype"))
    {
    std::string markType = sceneFragment["marktype"].asString();
    vtkDebugMacro(<< "UpdateDispatch: marktype: " << markType << ".\n");

    switch (GetMarkType(markType.c_str()))
      {
      case vtkVegaMarkItem::ARC:
        {
        this->UpdateMarkItem<vtkVegaArcItem>(parentGroup, sceneFragment, index);
        break;
        }
      case vtkVegaMarkItem::AREA:
        {
        this->UpdateMarkItem<vtkVegaAreaItem>(parentGroup, sceneFragment, index);
        break;
        }
      case vtkVegaMarkItem::GROUP:
        {
        this->UpdateGroupItem(parentGroup, sceneFragment, index);
        break;
        }
      case vtkVegaMarkItem::IMAGE:
        {
        this->UpdateMarkItem<vtkVegaImageItem>(parentGroup, sceneFragment, index);
        break;
        }
      case vtkVegaMarkItem::LINE:
        {
        this->UpdateMarkItem<vtkVegaLineItem>(parentGroup, sceneFragment, index);
        break;
        }
      case vtkVegaMarkItem::PATH:
        {
        this->UpdateMarkItem<vtkVegaPathItem>(parentGroup, sceneFragment, index);
        break;
        }
      case vtkVegaMarkItem::RECT:
        {
        this->UpdateMarkItem<vtkVegaRectItem>(parentGroup, sceneFragment, index);
        break;
        }
      case vtkVegaMarkItem::RULE:
        {
        this->UpdateMarkItem<vtkVegaRuleItem>(parentGroup, sceneFragment, index);
        break;
        }
      case vtkVegaMarkItem::SYMBOL:
        {
        this->UpdateMarkItem<vtkVegaSymbolItem>(parentGroup, sceneFragment, index);
        break;
        }
      case vtkVegaMarkItem::TEXT:
        {
        this->UpdateMarkItem<vtkVegaTextItem>(parentGroup, sceneFragment, index);
        break;
        }
      default:
        {
        vtkWarningMacro(<< "Unknown mark type: '" << markType << "'\n");
        }
      }
    }
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateGroupItem(vtkVegaGroupItem* parentGroup,
                                      const Json::Value& sceneFragment,
                                      int index)
{
  static const Json::Value defaultValue(Json::arrayValue);

  if (!this->UpdateMarkItem<vtkVegaGroupItem>(parentGroup, sceneFragment, index))
    {
    return;
    }

  vtkVegaMarkItemCollection* collection = parentGroup->GetMarkItemCollection(index);

  const Json::Value& sceneGroups = sceneFragment["items"];
  unsigned int n = sceneGroups.size();

  for (unsigned int i = 0; i < n; ++i)
    {
    int idx = 0;

    const Json::Value& items = sceneGroups[i]["items"];
    if (!items.isArray())
      {
      vtkErrorMacro(<< "'items' property is not an array.");
      return;
      }

    const Json::Value& legends = sceneGroups[i].get("legendItems", defaultValue);
    if (!legends.isArray())
      {
      vtkErrorMacro(<< "'legendItems' property is not an array.");
      return;
      }

    const Json::Value& axes = sceneGroups[i].get("axisItems", defaultValue);
    if (!axes.isArray())
      {
      vtkErrorMacro(<< "'axisItems' property is not an array.");
      return;
      }

    vtkVegaGroupItem* group = vtkVegaGroupItem::SafeDownCast(collection->GetMarkItem(i));

    for (unsigned int j = 0, m = axes.size(); j < m; ++j)
      {
      this->UpdateGroupItem(group, axes[j], idx++);
      }

    for (unsigned int j = 0, m = items.size(); j < m; ++j)
      {
      this->UpdateDispatch(group, items[j], idx++);
      }

    for (unsigned int j = 0, m = legends.size(); j < m; ++j)
      {
      this->UpdateGroupItem(group, legends[j], idx++);
      }
    }
}


//------------------------------------------------------------------------------
template<typename MarkItemType>
bool vtkVegaRenderer::UpdateMarkItem(vtkVegaGroupItem* parentGroup,
                                      const Json::Value& sceneFragment,
                                      int index)
{

  if (!sceneFragment.isMember("items"))
    {
    vtkErrorMacro(<< "'items' property not present.");
    return false;
    }

  const Json::Value& sceneItems = sceneFragment["items"];

  vtkVegaMarkItemCollection* collection = parentGroup->GetMarkItemCollection(index);
  if (collection == NULL)
    {
    vtkErrorMacro(<< "It is not possible to get mark item collection "
                  << "for index: " << index << ".");
    return false;
    }

  collection->SetMarkType(MarkItemType::Type);

  if (IsMultiDataSingleItem(MarkItemType::Type))
    {
    collection->FlagAllAsNotUpdated();
    // A single mark item is created/updated.
    MarkItemType* item = MarkItemType::SafeDownCast(collection->GetMarkItem(0));
    if (item == NULL)
      {
      vtkErrorMacro(<< "It is not possible to get mark item for index: 0.");
      return false;
      }
    // All scene items are passed to the update routine.
    this->UpdateProperties(sceneItems, item);
    collection->FlagMarkItemAsUpdated(0);
    collection->RemoveNotUpdatedMarkItems();
    }
  else
    {
    unsigned int n = sceneItems.size();
    collection->FlagAllAsNotUpdated();
    for (unsigned int i = 0; i < n; ++i)
      {
      MarkItemType* item = MarkItemType::SafeDownCast(collection->GetMarkItem(i));
      if (item == NULL)
        {
        vtkErrorMacro(<< "It is not possible to get mark item "
                      << "for index: " << i << ".");
        return false;
        }
      this->UpdateProperties(sceneItems[i], item);
      collection->FlagMarkItemAsUpdated(i);
      }
    collection->RemoveNotUpdatedMarkItems();
    }
  return true;
}


//------------------------------------------------------------------------------
bool vtkVegaRenderer::UpdateProperties(const Json::Value& scene,
                                       vtkVegaSceneItem* item)
{
  if (scene.isObject() && scene.isMember("marktype"))
    {
    std::string markType = scene["marktype"].asString();
    int type = GetMarkType(markType.c_str());
    if (type == vtkVegaMarkItem::GROUP)
      {
      if (scene.isMember("items"))
        {
        const Json::Value& sceneGroups = scene["items"];
        unsigned int n = sceneGroups.size();
        if (n > 0)
          {
          const Json::Value& rootGroup = sceneGroups[0];
          if (rootGroup.isMember("width") && rootGroup.isMember("height"))
            {
            float width = rootGroup["width"].asFloat();
            float height = rootGroup["height"].asFloat();
            if (width > 0 && height > 0)
              {
              float top = 0;
              float bottom = 0;
              float left = 0;
              float right = 0;
              if (rootGroup.isMember("padding"))
                {
                const Json::Value& padding = rootGroup["padding"];
                top = padding["top"].asFloat();
                bottom = padding["bottom"].asFloat();
                left = padding["left"].asFloat();
                right = padding["right"].asFloat();

                item->SetPadding(top, left, bottom, right);
                }

              width += (left + right);
              height += (top + bottom);
              item->SetWidth(width);
              item->SetHeight(height);

              vtkColor4ub color(0, 0, 0, 0);
              item->GetBrush()->SetColor(color);
              color = vtkColor4ub(0, 0, 0, 255);
              item->GetPen()->SetColor(color);
              return true;
              }
            }
          }
        }
      }
    }
  vtkErrorMacro(<< "input scene graph is not valid.");
  return false;
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateProperties(const Json::Value& sceneItem,
                                       vtkVegaArcItem* item)
{
  float x = sceneItem.get("x", this->Defaults->X).asFloat();
  float y = sceneItem.get("y", this->Defaults->Y).asFloat();
  float outerRadius = sceneItem.get("outerRadius", this->Defaults->OuterRadius).asFloat();
  float innerRadius = sceneItem.get("innerRadius", this->Defaults->InnerRadius).asFloat();
  float startAngle = sceneItem.get("startAngle", this->Defaults->StartAngle).asFloat();
  float endAngle = sceneItem.get("endAngle", this->Defaults->EndAngle).asFloat();

  item->SetX(x);
  item->SetY(y);
  item->SetOuterRadius(outerRadius);
  item->SetInnerRadius(innerRadius);
  item->SetStartAngle(startAngle);
  item->SetEndAngle(endAngle);


  double opacity = sceneItem.get("opacity", this->Defaults->Opacity).asDouble();
  std::string fillColor = sceneItem.get("fill", this->Defaults->FillColor).asString();
  double fillOpacity = sceneItem.get("fillOpacity", this->Defaults->FillOpacity).asDouble();
  std::string strokeColor = sceneItem.get("stroke", this->Defaults->StrokeColor).asString();
  double strokeOpacity = sceneItem.get("strokeOpacity", this->Defaults->StrokeOpacity).asDouble();
  float strokeWidth = sceneItem.get("strokeWidth", this->Defaults->StrokeWidth).asFloat();

  item->SetOpacity(opacity);
  vtkColor4ub color = this->StringColorMapper->HTMLColorToRGBA(fillColor);
  item->GetBrush()->SetColor(color);
  if (color[3] != 0)
    {
    item->GetBrush()->SetOpacityF(fillOpacity);
    }
  color = this->StringColorMapper->HTMLColorToRGBA(strokeColor);
  item->GetPen()->SetColor(color);
  if (color[3] != 0)
    {
    item->GetPen()->SetOpacityF(strokeOpacity);
    }
  item->GetPen()->SetWidth(strokeWidth);
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateProperties(const Json::Value& sceneItems,
                                       vtkVegaAreaItem* item)
{
  vtkVegaLineItem* base = item;
  this->UpdateProperties(sceneItems, base);
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateProperties(const Json::Value& sceneItem,
                                       vtkVegaImageItem* item)
{
  float x = sceneItem.get("x", this->Defaults->X).asFloat();
  float y = sceneItem.get("y", this->Defaults->Y).asFloat();
  float width = sceneItem.get("width", this->Defaults->Width).asFloat();
  float height = sceneItem.get("height", this->Defaults->Height).asFloat();

  item->SetX(x);
  item->SetY(y);
  item->SetWidth(width);
  item->SetHeight(height);


  // Set horizontal and vertical alignment.
  std::string align = sceneItem.get("align", this->Defaults->ImageAlign).asString();
  std::string baseline = sceneItem.get("baseline", this->Defaults->ImageBaseline).asString();

  if (align.compare("center") == 0)
    {
    item->SetAlign(vtkVegaImageItem::CENTER);
    }
  else if (align.compare("right") == 0)
    {
    item->SetAlign(vtkVegaImageItem::RIGHT);
    }
  else
    {
    item->SetAlign(vtkVegaImageItem::LEFT);
    }

  if (baseline.compare("middle") == 0)
    {
    item->SetBaseline(vtkVegaImageItem::MIDDLE);
    }
  else if (baseline.compare("bottom") == 0)
    {
    item->SetBaseline(vtkVegaImageItem::BOTTOM);
    }
  else
    {
    item->SetBaseline(vtkVegaImageItem::TOP);
    }


  // Set image url.
  std::string url = sceneItem.get("url", this->Defaults->Url).asString();
  url.insert(0, "/");
  url.insert(0, this->GetBaseDataURL());

  item->SetUrl(url.c_str());
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateProperties(const Json::Value& sceneItems,
                                       vtkVegaLineItem* item)
{
  if (sceneItems.size() > 0)
    {
    vtkVegaPathItem* base = item;
    this->UpdateProperties(sceneItems[0], base);
    }
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateProperties(const Json::Value& sceneItem,
                                       vtkVegaPathItem* item)
{
  float x = sceneItem.get("x", this->Defaults->X).asFloat();
  float y = sceneItem.get("y", this->Defaults->Y).asFloat();

  item->SetX(x);
  item->SetY(y);


  double opacity = sceneItem.get("opacity", this->Defaults->Opacity).asDouble();
  std::string fillColor = sceneItem.get("fill", this->Defaults->FillColor).asString();
  double fillOpacity = sceneItem.get("fillOpacity", this->Defaults->FillOpacity).asDouble();
  std::string strokeColor = sceneItem.get("stroke", this->Defaults->StrokeColor).asString();
  double strokeOpacity = sceneItem.get("strokeOpacity", this->Defaults->StrokeOpacity).asDouble();
  float strokeWidth = sceneItem.get("strokeWidth", this->Defaults->StrokeWidth).asFloat();

  item->SetOpacity(opacity);
  vtkColor4ub color = this->StringColorMapper->HTMLColorToRGBA(fillColor);
  item->GetBrush()->SetColor(color);
  if (color[3] != 0)
    {
    item->GetBrush()->SetOpacityF(fillOpacity);
    }
  color = this->StringColorMapper->HTMLColorToRGBA(strokeColor);
  item->GetPen()->SetColor(color);
  if (color[3] != 0)
    {
    item->GetPen()->SetOpacityF(strokeOpacity);
    }
  item->GetPen()->SetWidth(strokeWidth);

  vtkSmartPointer<vtkSVGPath> path;
  if (sceneItem.isMember("pathCache"))
    {
    const Json::Value& pathCache = sceneItem["pathCache"];
    path = MakePathFromCache(pathCache);
    }
  else if (sceneItem.isMember("path"))
    {
    std::string commands = sceneItem["path"].asString();
    path = vtkSVGPath::New();
    path->AppendCommandsFromString(commands);
    }
  else
    {
    path = vtkSVGPath::New();
    }
  item->SetPath(path);
  path->Delete();
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateProperties(const Json::Value& sceneItem,
                                       vtkVegaRectItem* item)
{
  float x = sceneItem.get("x", this->Defaults->X).asFloat();
  float y = sceneItem.get("y", this->Defaults->Y).asFloat();
  float width = sceneItem.get("width", this->Defaults->Width).asFloat();
  float height = sceneItem.get("height", this->Defaults->Height).asFloat();

  item->SetX(x);
  item->SetY(y);
  item->SetWidth(width);
  item->SetHeight(height);


  double opacity = sceneItem.get("opacity", this->Defaults->Opacity).asDouble();
  std::string fillColor = sceneItem.get("fill", this->Defaults->FillColor).asString();
  double fillOpacity = sceneItem.get("fillOpacity", this->Defaults->FillOpacity).asDouble();
  std::string strokeColor = sceneItem.get("stroke", this->Defaults->StrokeColor).asString();
  double strokeOpacity = sceneItem.get("strokeOpacity", this->Defaults->StrokeOpacity).asDouble();
  float strokeWidth = sceneItem.get("strokeWidth", this->Defaults->StrokeWidth).asFloat();

  item->SetOpacity(opacity);
  vtkColor4ub color = this->StringColorMapper->HTMLColorToRGBA(fillColor);
  item->GetBrush()->SetColor(color);
  if (color[3] != 0)
    {
    item->GetBrush()->SetOpacityF(fillOpacity);
    }
  color = this->StringColorMapper->HTMLColorToRGBA(strokeColor);
  item->GetPen()->SetColor(color);
  if (color[3] != 0)
    {
    item->GetPen()->SetOpacityF(strokeOpacity);
    }
  item->GetPen()->SetWidth(strokeWidth);
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateProperties(const Json::Value& sceneItem,
                                       vtkVegaRuleItem* item)
{
  const Json::Value& itemX = sceneItem.get("x", this->Defaults->X);
  float x = itemX.asFloat();
  const Json::Value& itemY = sceneItem.get("y", this->Defaults->Y);
  float y = itemY.asFloat();
  float x2 = sceneItem.get("x2", itemX).asFloat();
  float y2 = sceneItem.get("y2", itemY).asFloat();

  item->SetX(x);
  item->SetY(y);
  item->SetX2(x2);
  item->SetY2(y2);


  double opacity = sceneItem.get("opacity", this->Defaults->Opacity).asDouble();
  std::string strokeColor = sceneItem.get("stroke", this->Defaults->StrokeColor).asString();
  double strokeOpacity = sceneItem.get("strokeOpacity", this->Defaults->StrokeOpacity).asDouble();
  float strokeWidth = sceneItem.get("strokeWidth", this->Defaults->StrokeWidth).asFloat();

  item->SetOpacity(opacity);
  vtkColor4ub color = this->StringColorMapper->HTMLColorToRGBA(strokeColor);
  item->GetPen()->SetColor(color);
  if (color[3] != 0)
    {
    item->GetPen()->SetOpacityF(strokeOpacity);
    }
  item->GetPen()->SetWidth(strokeWidth);
}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateProperties(const Json::Value& sceneItem,
                                       vtkVegaSymbolItem* item)
{
  float x = sceneItem.get("x", this->Defaults->X).asFloat();
  float y = sceneItem.get("y", this->Defaults->Y).asFloat();
  float size = sceneItem.get("size", this->Defaults->Size).asFloat();

  item->SetX(x);
  item->SetY(y);
  item->SetSize(size);


  double opacity = sceneItem.get("opacity", this->Defaults->Opacity).asDouble();
  std::string fillColor = sceneItem.get("fill", this->Defaults->FillColor).asString();
  double fillOpacity = sceneItem.get("fillOpacity", this->Defaults->FillOpacity).asDouble();
  std::string strokeColor = sceneItem.get("stroke", this->Defaults->StrokeColor).asString();
  double strokeOpacity = sceneItem.get("strokeOpacity", this->Defaults->StrokeOpacity).asDouble();
  float strokeWidth = sceneItem.get("strokeWidth", this->Defaults->StrokeWidth).asFloat();

  item->SetOpacity(opacity);
  vtkColor4ub color = this->StringColorMapper->HTMLColorToRGBA(fillColor);
  item->GetBrush()->SetColor(color);
  if (color[3] != 0)
    {
    item->GetBrush()->SetOpacityF(fillOpacity);
    }
  color = this->StringColorMapper->HTMLColorToRGBA(strokeColor);
  item->GetPen()->SetColor(color);
  if (color[3] != 0)
    {
    item->GetPen()->SetOpacityF(strokeOpacity);
    }
  item->GetPen()->SetWidth(strokeWidth);


  std::string shape = sceneItem.get("shape", this->Defaults->Shape).asString();

  int shapeId = vtkVegaSymbolItem::CIRCLE;
  if (shape.compare("circle") == 0)
    {
    shapeId = vtkVegaSymbolItem::CIRCLE;
    }
  else if (shape.compare("square") == 0)
    {
    shapeId = vtkVegaSymbolItem::SQUARE;
    }
  else if (shape.compare("cross") == 0)
    {
    shapeId = vtkVegaSymbolItem::CROSS;
    }
  else if (shape.compare("diamond") == 0)
    {
    shapeId = vtkVegaSymbolItem::DIAMOND;
    }
  else if (shape.compare("triangle-up") == 0)
    {
    shapeId = vtkVegaSymbolItem::TRIANGLE_UP;
    }
  else if (shape.compare("triangle-down") == 0)
    {
    shapeId = vtkVegaSymbolItem::TRIANGLE_DOWN;
    }

  item->SetShape(shapeId);

}


//------------------------------------------------------------------------------
void vtkVegaRenderer::UpdateProperties(const Json::Value& sceneItem,
                                       vtkVegaTextItem* item)
{
  float x = sceneItem.get("x", this->Defaults->X).asFloat();
  float y = sceneItem.get("y", this->Defaults->Y).asFloat();
  float dx = sceneItem.get("dx", this->Defaults->TextOffsetX).asFloat();
  float dy = sceneItem.get("dy", this->Defaults->TextOffsetY).asFloat();
  float offsetRadius = sceneItem.get("radius", this->Defaults->TextOffsetRadius).asFloat();
  double offsetAngle = sceneItem.get("theta", this->Defaults->TextOffsetAngle).asDouble();

  item->SetX(x);
  item->SetY(y);
  item->SetDx(dx);
  item->SetDy(dy);
  item->SetRadius(offsetRadius);
  item->SetTheta(offsetAngle);


  double angle = sceneItem.get("angle", this->Defaults->TextAngle).asDouble();

  vtkTextProperty* textProp = item->GetTextProperty();
  // We need the opposite angle probably for having swapped Y axis direction.
  textProp->SetOrientation(-angle);


  double opacity = sceneItem.get("opacity", this->Defaults->Opacity).asDouble();
  std::string fillColor = sceneItem.get("fill", this->Defaults->FillColor).asString();
  double fillOpacity = sceneItem.get("fillOpacity", this->Defaults->FillOpacity).asDouble();
//  std::string strokeColor = sceneItem.get("stroke", this->Defaults->StrokeColor).asString();
//  double strokeOpacity = sceneItem.get("strokeOpacity", this->Defaults->StrokeOpacity).asDouble();
//  float strokeWidth = sceneItem.get("strokeWidth", this->Defaults->StrokeWidth).asFloat();


  vtkColor4ub color = this->StringColorMapper->HTMLColorToRGBA(fillColor);
  textProp->SetOpacity(opacity * fillOpacity * (color[3] / 255.0));
  textProp->SetColor(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0);


  std::string fontFamily = sceneItem.get("font", this->Defaults->FontFamily).asString();
  double fontSize = sceneItem.get("fontSize", this->Defaults->FontSize).asDouble();
  std::string fontStyle = ToLowerCase(sceneItem.get("fontStyle", this->Defaults->FontStyle).asString());
  std::string fontWeight = ToLowerCase(sceneItem.get("fontWeight", this->Defaults->FontWeight).asString());
  std::string textAlign = ToLowerCase(sceneItem.get("align", this->Defaults->TextAlign).asString());
  std::string textBaseline = ToLowerCase(sceneItem.get("baseline", this->Defaults->TextBaseline).asString());

  // Set font family
  if (fontFamily.compare("serif") == 0)
    {
    textProp->SetFontFamilyToTimes();
    }
  else if (fontFamily.compare("monospace") == 0)
    {
    textProp->SetFontFamilyToCourier();
    }
  else
    {
    textProp->SetFontFamilyToArial();
    }

  // Set font size.
  textProp->SetFontSize(static_cast<int>(fontSize));

  // Set font style.
  if (fontStyle.compare("italic") == 0 ||
      fontStyle.compare("oblique") == 0)
    {
    textProp->ItalicOn();
    }
  else
    {
    textProp->ItalicOff();
    }

  // Set font weight.
  if (fontWeight.compare("bold") == 0 || fontWeight.compare("bolder") == 0 ||
      (IsUnsignedInt(fontWeight) && std::strtoul(fontWeight.c_str(), NULL, 10) > 500))

    {
    textProp->BoldOn();
    }
  else
    {
    textProp->BoldOff();
    }

  // Set horizontal alignment.
  if (textAlign.compare("right") == 0)
    {
    textProp->SetJustificationToRight();
    }
  else if (textAlign.compare("center") == 0)
    {
    textProp->SetJustificationToCentered();
    }
  else
    {
    textProp->SetJustificationToLeft();
    }

  // Set vertical alignment.
  if (textBaseline.compare("top") == 0)
    {
    textProp->SetVerticalJustificationToTop();
    }
  else if (textBaseline.compare("bottom") == 0)
    {
    textProp->SetVerticalJustificationToBottom();
    }
  else
    {
    textProp->SetVerticalJustificationToCentered();
    }

  // Set item text.
  const Json::Value& itemText = sceneItem.get("text", Defaults->Text);

  std::string text;
  if (itemText.isString())
    {
    text = itemText.asString();
    }
  else if (itemText.isInt())
    {
    std::stringstream ss;
    ss << itemText.asInt();
    text = ss.str();
    }
  else if (itemText.isDouble())
    {
    std::stringstream ss;
    ss << itemText.asDouble();
    text = ss.str();
    }

  item->SetText(text.c_str());
}
