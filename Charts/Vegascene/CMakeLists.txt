
include(vtkQt)

set(LibSrcs
    data.cxx
    jscallback.cxx
    jscallbackmanager.cxx
    jsconsole.cxx
    jscontext2d.cxx
    jsmodule.cxx
    vegascene.cxx
    vtkVegaScene.cxx
  )

set(MocHeaders
    jscallback.h
    jscallbackmanager.h
    jsconsole.h
    data.h
    jscontext2d.h
  )

set(NonMocHeaders
    vtkVegaScene.h
    vegascene.h
    jsmodule.h
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

  qt5_wrap_cpp(LibMocSrcs ${MocHeaders})

  set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Concurrent_LIBRARIES} ${Qt5Qml_LIBRARIES} ${Qt5Gui_LIBRARIES})

  # When this module is loaded by an app, load Qt too.
  vtk_module_export_code_find_package(Qt5Core)
  vtk_module_export_code_find_package(Qt5Concurrent)
  vtk_module_export_code_find_package(Qt5Qml)
  vtk_module_export_code_find_package(Qt5Gui)
else()
    message("Qt 5 required")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  add_definitions(-DDEBUG)
  add_definitions(-DWITH_EXAMPLE_LIBS)
endif()

add_definitions(-DWITH_TOPOJSON)


set(${vtk-module}_NO_HeaderTest 1)
vtk_module_library(${vtk-module} ${LibSrcs} ${LibMocSrcs} ${NonMocHeaders} ${MocHeaders})

target_link_libraries(${vtk-module} LINK_PRIVATE ${QT_LIBRARIES})
