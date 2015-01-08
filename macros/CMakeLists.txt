set( LIB_EPECUR_MACROS_SRCS

	adjustment_shifts.C
	drift_calib.C
	drift_calib_curve.C
	drift_chisq.C
	drift_efficiency.C
	drift_efficiency_over_time.C
	drift_sum_rule.C
	target_info.C
	track_intersections.C
	)

include_directories ( ${ROOT_INCLUDE_DIR} ${EPECUR_INCLUDE_DIRS} ${ROOT_HELPER_DIR} )

add_library(
	epecur-macros
	${LIB_EPECUR_MACROS_SRCS}
	)

target_link_libraries (
	epecur-macros
	${ROOT_LIBRARIES}
	)

add_executable (
	make_plots
	make_plots.cpp
	)

target_link_libraries (
	make_plots
	${ROOT_LIBRARIES}
	epecur-macros
	)

install(TARGETS make_plots
	RUNTIME DESTINATION bin
	)
