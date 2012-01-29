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

add_executable (
	track_test
	track_test.cpp
        )

target_link_libraries (
        track_test
	${Boost_LIBRARIES}
        epecur
        )

add_test(
	NAME track
	COMMAND $<TARGET_FILE:track_test>
)

add_executable (
	TrackRecognitionHook_test
	TrackRecognitionHook_test.cpp
        )

target_link_libraries (
        TrackRecognitionHook_test
	${Boost_LIBRARIES}
        epecur
        )

add_test(
	NAME TrackRecognitionHook
	COMMAND $<TARGET_FILE:TrackRecognitionHook_test>
)
