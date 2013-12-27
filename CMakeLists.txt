cmake_minimum_required (VERSION 2.6)
project (EPECUR)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

find_package( Boost 1.41 REQUIRED iostreams filesystem system program_options regex unit_test_framework )
find_package( BoostEndian )
find_package( ROOT )

set( EPECUR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/ )
set( EPECUR_MACROS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR} )

add_definitions("-std=c++0x")
add_definitions("-Wall -pedantic")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}) 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}) 

add_subdirectory(macros)
add_subdirectory(src)
add_subdirectory(tests)

install(FILES contrib/geom_apr10.c DESTINATION share)
