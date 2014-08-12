vtk_add_test_cxx(${vtk-module}CxxTests tests
  TestSVGPath.cxx,NO_DATA,NO_VALID
  )

  vtk_test_cxx_executable(${vtk-module}CxxTests tests)
