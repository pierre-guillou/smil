file(GLOB TEST_PY *.py)

foreach(_PY ${TEST_PY})
  get_filename_component(TEST_NAME ${_PY} NAME_WE)
  add_test(NAME ${TEST_NAME}_py COMMAND ${Python3_EXECUTABLE} ${_PY})
endforeach()
