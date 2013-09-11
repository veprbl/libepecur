#include <GL/glut.h>

#include <boost/foreach.hpp>

#include "ProcessVisualizer.hpp"

process_result_t vis_result;

bool	mouse_down = false;
int		mouse_x0;
int		mouse_y0;
float	xrot = 0;
float	yrot = 0;

void	display( void )
{
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f);

	glRotatef(-xrot, 0.0f, 0.0f, 1.0f);
	glRotatef(yrot, 1.0f, 0.0f, 0.0f);

	glPushMatrix();
	glScalef(1/500.0, 1/500.0, 1/500.0);

	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 1.0);
	BOOST_FOREACH(ublas::vector<double> &v, vis_result.i1)
	{
		glVertex3f(v(0), v(1), v(2));
	}
	glEnd();
	glPopMatrix();

	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glEnd();
	glBegin(GL_LINES);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glEnd();
	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();

	glutSwapBuffers();
}

void	reshape( GLint width, GLint height )
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (float)width / height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void	mouse( GLint button, GLint state, GLint x, GLint y )
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		mouse_down = true;

		mouse_x0 = x;
		mouse_y0 = y;
	}
	else
	{
		mouse_down = false;
	}
}

void	mouse_move( GLint x, GLint y )
{
	if (mouse_down)
	{
		xrot += mouse_x0 - x;
		yrot += mouse_y0 - y;
		mouse_x0 = x;
		mouse_y0 = y;
 
		glutPostRedisplay();
	}
}

void	ProcessVisualize( int argc, char *argv[] )
{
	glutInit(&argc, argv);
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("libepecur");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_move);

    glutMainLoop();
}
