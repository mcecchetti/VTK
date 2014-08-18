vtk_add_test_cxx(${vtk-module}CxxTests tests
  TestSVGPath.cxx,NO_DATA,NO_VALID
  )

list(APPEND tests
  TestVegaRenderer.cxx
  )

set(ExampleList
  arc
  area
  bar
  )

foreach(example IN LISTS ExampleList)
  # message(WARNING ${scene})

  ExternalData_add_test(VTKData
    NAME ${vtk-module}Cxx-TestExample_${example}
    COMMAND ${vtk-module}CxxTests TestVegaRenderer
      ${${vtk-module}_SOURCE_DIR}/Testing/Data/Examples/scenes/${example}-sg.json
      -b file://${${vtk-module}_SOURCE_DIR}/Testing/Data/Examples
      -D ${VTK_TEST_DATA_DIR}
      -T ${VTK_TEST_OUTPUT_DIR}
      -V ${${vtk-module}_SOURCE_DIR}/Testing/Data/Baseline/${example}.png
      #-V DATA{../Data/Baseline/TestExample_${example}.png,:}
  )
endforeach()

vtk_test_cxx_executable(${vtk-module}CxxTests tests
  RENDERING_FACTORY
  )
