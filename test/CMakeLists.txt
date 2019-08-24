add_library(catch STATIC test_main.cpp)
target_include_directories(catch PUBLIC ../3rdparty/catch2)

macro(_snoot_test TEST_NAME)
  add_executable(${TEST_NAME} ${ARGN})
  target_link_libraries(${TEST_NAME} snoot_lib catch)
  add_test(${TEST_NAME} ${TEST_NAME})
endmacro()

_snoot_test(log_item_test log_item_test.cpp)
_snoot_test(monitor_test monitor_test.cpp)
