set( AXIAL_SHIFT_SRCS

	AxialShiftApp.cpp
	AxialShiftHook.cpp

)

link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR} ${EPECUR_INCLUDE_DIRS} )

add_executable (
	axial_shift
	${AXIAL_SHIFT_SRCS}
	)

target_link_libraries (
	axial_shift
	${Boost_LIBRARIES}
	${ROOT_LIBRARIES}
	epecur
	)
