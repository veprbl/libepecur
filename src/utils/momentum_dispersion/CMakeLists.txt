set( MOMENTUM_DISPERSION_SRCS

	EvSumHook.cpp
	MomentumDispersionApp.cpp

)

link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR} ${EPECUR_INCLUDE_DIRS} )

add_executable (
	momentum_dispersion
	${MOMENTUM_DISPERSION_SRCS}
	)

target_link_libraries (
	momentum_dispersion
	${Boost_LIBRARIES}
	${ROOT_LIBRARIES}
	epecur
	)
