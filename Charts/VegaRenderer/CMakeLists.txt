set(Module_SRCS
  vtkVegaRenderer.cxx
  vtkVegaDataLoader.cxx
  vtkVegaImageLoader.cxx
  vtkVegaMarkItemCollection.cxx
  vtkVegaMarkItem.cxx
  vtkVegaSceneItem.cxx
  vtkVegaArcItem.cxx
  vtkVegaAreaItem.cxx
  vtkVegaGroupItem.cxx
  vtkVegaImageItem.cxx
  vtkVegaLineItem.cxx
  vtkVegaPathItem.cxx
  vtkVegaRectItem.cxx
  vtkVegaRuleItem.cxx
  vtkVegaSymbolItem.cxx
  vtkVegaTextItem.cxx
  )

set_source_files_properties(
  vtkVegaDataLoader
  vtkVegaMarkItem
  ABSTRACT
  )


vtk_module_library(vtkChartsVegaRenderer ${Module_SRCS})
