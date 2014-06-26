
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
    vtkVegaSpecReader.cxx
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
    vtkVegaSpecReader.h
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


# Copy JavaScript files if necessary.
if(NOT "${VTK_BINARY_DIR}" MATCHES "^${VTK_SOURCE_DIR}$")
# Make the necessary directories.
  file(MAKE_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/jslib
    ${CMAKE_CURRENT_BINARY_DIR}/jsmodules)

  unset(VEGA_JS_FILES)

# JavaScript source files.
  list(APPEND VEGA_JS_FILES
    jslib/console
    jslib/d3.geo.projection.min
    jslib/setTimeout
    jslib/vega.data.load
    jslib/vega.postload.config
    jslib/vega.preload.config
    jslib/vegascene.canvas
    jslib/vegascene.render
    jslib/vegascene.run
    )

# JavaScript modules.
  list(APPEND VEGA_JS_FILES
    jsmodules/d3
    jsmodules/topojson
    jsmodules/vega
    )

# Now copy these files.
  unset(VEGA_JS_SOURCE_FILES)
  foreach(file ${VEGA_JS_FILES})
    set(src "${CMAKE_CURRENT_BINARY_DIR}/${file}.js")
    set(VEGA_JS_SOURCE_FILES ${VEGA_JS_SOURCE_FILES} ${src})
  endforeach()

  foreach(file ${VEGA_JS_FILES})
    set(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}.js")
    set(tgt "${CMAKE_CURRENT_BINARY_DIR}/${file}.js")
    add_custom_command(
      DEPENDS ${src}
      OUTPUT ${tgt}
      COMMAND ${CMAKE_COMMAND} -E copy ${src} ${tgt}
      COMMENT "source copy")
  endforeach()

  add_custom_target(vtkvega_js_source_copy ALL echo "..."
      DEPENDS ${VEGA_JS_SOURCE_FILES})

  add_dependencies(${vtk-module} vtkvega_js_source_copy)
endif()
