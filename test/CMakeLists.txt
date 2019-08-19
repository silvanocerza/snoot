macro(_snoot_test TEST_NAME)
  add_executable(${TEST_NAME} ${ARGN})
  target_link_libraries(${TEST_NAME} snoot_lib)
  add_test(${TEST_NAME} ${TEST_NAME})
endmacro()

# Example
# _snoot_test(foo_test foo_test.cpp)
