set( EXPORT_TREE_SRCS

	TreeExportApp.cpp
	TreeExportHook.cpp
	DriftCalibHook.cpp

)

link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR} ${EPECUR_INCLUDE_DIRS} )

add_executable (
	export_tree
	${EXPORT_TREE_SRCS}
	)

target_link_libraries (
	export_tree
	${Boost_LIBRARIES}
	${ROOT_LIBRARIES}
	epecur
	)
