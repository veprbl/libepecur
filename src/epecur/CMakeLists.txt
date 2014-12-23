set( EPECUR_SRCS

	geometry.cpp
	loadfile.cpp
	track.cpp
	StdHits.cpp
	TrackRecognitionHook.cpp
	git-rev-export.cpp

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

add_dependencies(epecur git_rev)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(git_rev
	${PROJECT_SOURCE_DIR}/get_rev_id.sh ${CMAKE_CURRENT_BINARY_DIR}/git-rev.hpp
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	VERBATIM
	)
