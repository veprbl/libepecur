set( PROCESS_SRCS

	Process2ndPass.cpp
	ProcessApp.cpp
	ProcessMain.cpp

)

link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR} ${EPECUR_INCLUDE_DIRS} ${EPECUR_MACROS_INCLUDE_DIRS} )

add_executable (
	process
	${PROCESS_SRCS}
	)

target_link_libraries (
	process
	${Boost_LIBRARIES}
	${ROOT_LIBRARIES}
	epecur
	epecur-macros
	)

install(TARGETS process
	RUNTIME DESTINATION bin
	)
