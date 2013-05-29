set( PROCESS_SRCS

	ProcessApp.cpp
	ProcessMain.cpp
	ProcessVisualizer.cpp

)

link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR} ${EPECUR_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLU_INCLUDE_PATH} ${GLUT_INCLUDE_DIR} )

add_executable (
	process
	${PROCESS_SRCS}
	)

target_link_libraries (
	process
	${Boost_LIBRARIES}
	${ROOT_LIBRARIES}
	${OPENGL_LIBRARIES}
	${GLU_LIBRARIES}
	${GLUT_LIBRARIES}
	epecur
	)
