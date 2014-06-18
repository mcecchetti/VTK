include(vtkQt)

set(Module_SRCS
  vtkVegaSpecReader.cxx
  )


if(VTK_QT_VERSION VERSION_GREATER "4")
  find_package(Qt5Core REQUIRED QUIET)
  find_package(Qt5Concurrent REQUIRED QUIET)
  find_package(Qt5Qml REQUIRED QUIET)
  find_package(Qt5Gui REQUIRED QUIET)

  set(_qt_include_dirs ${Qt5Core_INCLUDE_DIRS} ${Qt5Concurrent_INCLUDE_DIRS} ${Qt5Qml_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})
  list(REMOVE_DUPLICATES _qt_include_dirs)
  include_directories(${_qt_include_dirs})

  set(_qt_definitions ${Qt5Core_DEFINITIONS} ${Qt5Concurrent_DEFINITIONS} ${Qt5Qml_DEFINITIONS} ${Qt5Gui_DEFINITIONS})
  list(REMOVE_DUPLICATES _qt_definitions)
  add_definitions(${_qt_definitions})

  vtk_module_export_code_find_package(Qt5Core)
  vtk_module_export_code_find_package(Qt5Concurrent)
  vtk_module_export_code_find_package(Qt5Qml)
  vtk_module_export_code_find_package(Qt5Gui)
else()
    message("Qt 5 required")
endif()

vtk_module_library(vtkChartsQtVega ${Module_SRCS})
