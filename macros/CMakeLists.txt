set( MACROS_SRCS

	make_plots.cpp
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

add_executable (
	make_plots
	${MACROS_SRCS}
	)

target_link_libraries (
	make_plots
	${ROOT_LIBRARIES}
	)

install(TARGETS make_plots
	RUNTIME DESTINATION bin
	)
