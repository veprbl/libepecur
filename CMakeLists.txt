cmake_minimum_required (VERSION 2.6)
project (EPECUR)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

find_package( Boost 1.41 REQUIRED iostreams filesystem system program_options regex test_exec_monitor unit_test_framework )
find_package( BoostEndian )
find_package( ROOT )
find_package( OpenGL )
find_package( GLU )
find_package( GLUT )

set( EPECUR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/ )

option(USE_CLANG "build application with clang" OFF)
if (USE_CLANG)
	set(CMAKE_C_COMPILER "/usr/bin/clang")
	set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
	add_definitions("-stdlib=libstdc++")
endif()

add_definitions("-std=c++0x")
add_definitions("-Wall -pedantic")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}) 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}) 

add_subdirectory(src)
add_subdirectory(tests)

install(FILES contrib/geom_apr10.c DESTINATION share)
