set( MOMENTUM_DISPERSION_SRCS

	EvShowHook.cpp
	ShowEventsApp.cpp

)

link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR} ${EPECUR_INCLUDE_DIRS} )

add_executable (
	show_events
	${MOMENTUM_DISPERSION_SRCS}
	)

target_link_libraries (
	show_events
	${Boost_LIBRARIES}
	${ROOT_LIBRARIES}
	epecur
	)

install(TARGETS show_events
	RUNTIME DESTINATION bin
	)
