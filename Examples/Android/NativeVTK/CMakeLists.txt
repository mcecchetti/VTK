cmake_minimum_required(VERSION 2.8)
project(NativeVTK)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  message(FATAL_ERROR "VTK Android does not support in-source builds :) .")
endif ()

find_package(VTK COMPONENTS
  vtkInteractionStyle
  vtkRenderingOpenGL2
)
include(${VTK_USE_FILE})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_NDK_ABI_NAME})
message(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
add_subdirectory(jni)

# find android
find_program(ANDROID_EXECUTABLE
  NAMES android
  DOC   "The android command-line tool")
if(NOT ANDROID_EXECUTABLE)
  message(FATAL_ERROR "Can not find android command line tool: android")
endif()

set(ANDROID_TARGET "android-19"
  CACHE STRING "The Android API to target")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/AndroidManifest.xml"
  "${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml"
  COPYONLY)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/ant.properties.in"
  "${CMAKE_CURRENT_BINARY_DIR}/ant.properties"
  @ONLY)

add_custom_target(NativeVTK-ant-configure ALL
  COMMAND "${ANDROID_EXECUTABLE}"
          update project
          --name NativeVTK
          --path "${CMAKE_CURRENT_SOURCE_DIR}"
          --target "${ANDROID_TARGET}"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/build.xml"
          "${CMAKE_CURRENT_BINARY_DIR}/build.xml"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/local.properties"
          "${CMAKE_CURRENT_BINARY_DIR}/local.properties"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/project.properties"
          "${CMAKE_CURRENT_BINARY_DIR}/project.properties"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/proguard-project.txt"
          "${CMAKE_CURRENT_BINARY_DIR}/proguard-project.txt"
  COMMAND "${CMAKE_COMMAND}" -E remove
          "${CMAKE_CURRENT_SOURCE_DIR}/build.xml"
          "${CMAKE_CURRENT_SOURCE_DIR}/local.properties"
          "${CMAKE_CURRENT_SOURCE_DIR}/project.properties"
          "${CMAKE_CURRENT_SOURCE_DIR}/proguard-project.txt"
  WORKING_DIRECTORY
          "${CMAKE_CURRENT_BINARY_DIR}")

add_dependencies(NativeVTK-ant-configure NativeVTK)

if (FALSE)
#find ant
find_program(ANT_EXECUTABLE
  NAMES ant
  DOC   "The ant build tool")
if(NOT ANT_EXECUTABLE)
  message(FATAL_ERROR "Can not find ant build tool: ant")
endif()

add_custom_target(NativeVTK-apk-release ALL
  COMMAND ${ANT_EXECUTABLE}
          -file "${CMAKE_CURRENT_BINARY_DIR}/build.xml"
          release)
add_dependencies(NativeVTK-apk-release
  NativeVTK-ant-configure
  NativeVTK
  NativevTK-apk-resource)

add_custom_target(NativeVTK-apk-debug ALL
  COMMAND ${ANT_EXECUTABLE}
          -file "${CMAKE_CURRENT_BINARY_DIR}/build.xml"
          debug)
add_dependencies(NativeVTK-apk-debug
  NativeVTK-apk-release
  NativeVTK-ant-configure
  NativeVTK
  NativevTK-apk-resource)
endif()

# Remove existing resource directory if any in the binary dir
add_custom_target(NativeVTK-apk-rm-resource ALL
  COMMAND ${CMAKE_COMMAND} -E remove_directory
          "${CMAKE_CURRENT_BINARY_DIR}/res")
add_dependencies(NativeVTK-apk-rm-resource
  NativeVTK-ant-configure
  NativeVTK)

# Copy res from the source dir to binary dir
add_custom_target(NativeVTK-apk-resource ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/res"
          "${CMAKE_CURRENT_BINARY_DIR}/res")
add_dependencies(NativeVTK-apk-resource
  NativeVTK-apk-rm-resource
  NativeVTK-ant-configure
  NativeVTK)
