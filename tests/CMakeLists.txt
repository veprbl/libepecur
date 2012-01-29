include_directories ( ${EPECUR_INCLUDE_DIRS} )

add_executable (
	unit_tests
	unit_tests_main.cpp
	loadfile_test.cpp
	TrackRecognitionHook_test.cpp
	track_test.cpp
        )

target_link_libraries (
        unit_tests
	${Boost_LIBRARIES}
        epecur
        )

add_test(
	NAME loadfile
	COMMAND $<TARGET_FILE:unit_tests> --run_test=loadfile_test
)

add_test(
	NAME track
	COMMAND $<TARGET_FILE:unit_tests> --run_test=track_test
)

add_test(
	NAME TrackRecognitionHook
	COMMAND $<TARGET_FILE:unit_tests> --run_test=TrackRecognitionHook_test
)
