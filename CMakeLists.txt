cmake_minimum_required (VERSION 2.8)
project (EPECUR)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

# workaround for
# http://stackoverflow.com/questions/9948375/cmake-find-package-succeeds-but-returns-wrong-path
execute_process(
	COMMAND lsb_release -is
	OUTPUT_VARIABLE LSB_DISTRIBUTOR
	)
execute_process(
	COMMAND lsb_release -rs
	OUTPUT_VARIABLE LSB_RELEASE
	)
if((${LSB_DISTRIBUTOR} MATCHES "Scientific.+") OR # Scientific, ScientificSL
   (${LSB_DISTRIBUTOR} STREQUAL "CentOS\n") OR
   (${LSB_DISTRIBUTOR} MATCHES "RedHat.+")) # RedHatEnterpriseServer
	if (${LSB_RELEASE} VERSION_LESS "7.0")
		set(Boost_NO_BOOST_CMAKE ON)
	endif()
endif()

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
