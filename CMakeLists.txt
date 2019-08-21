cmake_minimum_required(VERSION 3.1)

project(snoot)

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

option(BUILD_TESTING "Builds tests" OFF)

# Configure the compiler.
set(CMAKE_CXX_STANDARD 17)

file(GLOB SNOOT_SOURCES "src/*.cpp")
add_compile_options(-Wall -Wextra -Werror -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast -std=c++17)

# Outputs
# Library
add_library(snoot_lib SHARED ${SNOOT_SOURCES})
# This probably will need to be changed for Windows
target_link_libraries(snoot_lib pthread)
install(TARGETS snoot_lib
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Executable
add_executable(${PROJECT_NAME} "main.cpp")
target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} snoot_lib)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

# Tests
if(BUILD_TESTING)
  include(CTest)
  add_subdirectory(test)
endif()
