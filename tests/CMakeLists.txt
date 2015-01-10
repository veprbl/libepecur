include_directories ( ${EPECUR_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} )

add_executable (
	unit_tests
	unit_tests_main.cpp
	loadfile_test.cpp
	StdHits_test.cpp
	TrackReconstructionHook_test.cpp
	track_test.cpp
        )

target_link_libraries (
        unit_tests
	${Boost_LIBRARIES}
        epecur
        )

add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

add_test(
	NAME loadfile
	COMMAND $<TARGET_FILE:unit_tests> --run_test=loadfile_test
)

add_test(
	NAME track
	COMMAND $<TARGET_FILE:unit_tests> --run_test=track_test
)

add_test(
	NAME StdHits
	COMMAND $<TARGET_FILE:unit_tests> --run_test=StdHits_test
)

add_test(
	NAME TrackReconstructionHook
	COMMAND $<TARGET_FILE:unit_tests> --run_test=TrackReconstructionHook_test
)
