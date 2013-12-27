set( LIB_EPECUR_MACROS_SRCS

	adjustment_shifts.C
	drift_calib.C
	drift_calib_curve.C
	drift_chisq.C
	drift_effectivity.C
	drift_effectivity_over_time.C
	target_info.C
	track_intersections.C
	track_plane_section.C
	)

include_directories ( ${ROOT_INCLUDE_DIR} ${EPECUR_INCLUDE_DIRS} ${ROOT_HELPER_DIR} )

add_library(
	libepecur-macros
	${LIB_EPECUR_MACROS_SRCS}
	)

target_link_libraries (
	libepecur-macros
	${ROOT_LIBRARIES}
	)

add_executable (
	make_plots
	make_plots.cpp
	)

target_link_libraries (
	make_plots
	${ROOT_LIBRARIES}
	libepecur-macros
	)

install(TARGETS make_plots
	RUNTIME DESTINATION bin
	)
