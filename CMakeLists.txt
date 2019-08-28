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

if (MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(MSVC)

if (UNIX)
  add_compile_options(-Wall -Wextra -Werror -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast -std=c++17)
endif(UNIX)

# Outputs
# Library
add_library(snoot_lib SHARED ${SNOOT_SOURCES})

if (UNIX)
  target_link_libraries(snoot_lib pthread)
endif(UNIX)

if (APPLE)
  # These paths might need to be corrected
  include_directories(/usr/local/opt/llvm/include)
  link_directories(/usr/local/opt/llvm/lib)
  target_link_libraries(snoot_lib -lc++fs)
endif(APPLE)

target_include_directories(snoot_lib PUBLIC 3rdparty/date/include/date/)


install(TARGETS snoot_lib
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Executable
add_executable(${PROJECT_NAME} "main.cpp")
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    src
    3rdparty/args/
)

target_link_libraries(${PROJECT_NAME} snoot_lib)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

# Tests
if(BUILD_TESTING)
  include(CTest)
  add_subdirectory(test)
endif()
