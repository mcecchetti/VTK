vtk_module(vtkChartsVegaRenderer
  GROUPS
     StandAlone
  DEPENDS
    vtkRenderingContext2D
    vtkRenderingOpenGL
    vtkCommonSystem
    vtkCommonMisc
    vtkCommonCore
    vtkCommonColor
    vtkIOCore
    vtkIOImage
  PRIVATE_DEPENDS
    vtkjsoncpp
    vtksys
  TEST_DEPENDS
    vtkIOImage
    vtkCommonExecutionModel
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkRenderingContext${VTK_RENDERING_BACKEND}
    vtkViewsContext2D
    vtkTestingCore
    vtkTestingRendering
    vtkInteractionStyle
    vtkInteractionImage
  )
