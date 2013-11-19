#ifndef __PROCESS_VISUALIZER_HPP
#define __PROCESS_VISUALIZER_HPP

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

#include "ProcessMain.hpp"

extern process_result_t vis_result;

void	ProcessVisualize();

#endif
