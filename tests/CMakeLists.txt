include_directories ( ${EPECUR_INCLUDE_DIRS} )

add_executable (
	loadfile_test
	loadfile_test.cpp
        )

target_link_libraries (
        loadfile_test
	${Boost_LIBRARIES}
        epecur
        )

add_test(
	NAME loadfile
	COMMAND $<TARGET_FILE:loadfile_test>
)
