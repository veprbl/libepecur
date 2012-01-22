set( EPECUR_SRCS

	geometry.cpp
	loadfile.cpp

)

link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} ${Boost_Endian_INCLUDE_DIRS} )

add_library(
	epecur
	${EPECUR_SRCS}
	)

target_link_libraries (
	epecur
	${Boost_LIBRARIES}
	)
