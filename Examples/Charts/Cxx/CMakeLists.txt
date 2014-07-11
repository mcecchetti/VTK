cmake_minimum_required(VERSION 2.8.8)
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW) # CMake 3.0
endif()

PROJECT (Charts)

if(NOT VTK_SOURCE_DIR)
  message(ERROR " Cannot build Charts examples without VTK_SOURCE_DIR")
endif()


include(${VTK_USE_FILE})


ADD_EXECUTABLE(GraphItem GraphItem.cxx vtkGraphItem.cxx)
TARGET_LINK_LIBRARIES(GraphItem ${VTK_LIBRARIES})

ADD_EXECUTABLE(PiecewiseItem PiecewiseItem.cxx)
TARGET_LINK_LIBRARIES(PiecewiseItem ${VTK_LIBRARIES})



IF(NOT("${VTK_QT_VERSION}" STREQUAL ""))

  #Instruct CMake to run moc automatically when needed.
  SET(CMAKE_AUTOMOC ON)

  IF(VTK_QT_VERSION VERSION_GREATER "4")
    FIND_PACKAGE(Qt5Core REQUIRED QUIET)

    ADD_EXECUTABLE(QChartTable QChartTable.cxx)
    QT5_USE_MODULES(QChartTable Core Gui Widgets)
    TARGET_LINK_LIBRARIES(QChartTable ${VTK_LIBRARIES})

    ADD_EXECUTABLE(QScalarsToColors QScalarsToColors.cxx)
    QT5_USE_MODULES(QScalarsToColors Core Gui Widgets)
    TARGET_LINK_LIBRARIES(QScalarsToColors ${VTK_LIBRARIES})

    ADD_EXECUTABLE(QVegaScene QVegaScene.cxx)
    add_definitions(-DDEBUG)
    QT5_USE_MODULES(QVegaScene Core Gui Concurrent Qml)
    TARGET_LINK_LIBRARIES(QVegaScene ${VTK_LIBRARIES})

    ADD_EXECUTABLE(SceneString SceneString.cxx)
    QT5_USE_MODULES(SceneString Core Gui Concurrent Qml)
    TARGET_LINK_LIBRARIES(SceneString ${VTK_LIBRARIES})

    ADD_EXECUTABLE(VegaChart VegaChart.cxx)
    QT5_USE_MODULES(VegaChart Core Gui Concurrent Qml)
    TARGET_LINK_LIBRARIES(VegaChart ${VTK_LIBRARIES})

  ELSE()
      FIND_PACKAGE(Qt4 REQUIRED)
      INCLUDE(${QT_USE_FILE})

      SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
      SET(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
      SET(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")
      INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

      ADD_EXECUTABLE(QChartTable QChartTable.cxx)
      TARGET_LINK_LIBRARIES(QChartTable ${QT_LIBRARIES} ${VTK_LIBRARIES})

      ADD_EXECUTABLE(QScalarsToColors QScalarsToColors.cxx)
      TARGET_LINK_LIBRARIES(QScalarsToColors ${QT_LIBRARIES} ${VTK_LIBRARIES})
  ENDIF()
ENDIF()
