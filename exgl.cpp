#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

static void redraw(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0,0,-100);
	glBegin(GL_TRIANGLES);
	glColor3f(1,0,0);
	glVertex3f(-30,-30,0);
	glColor3f(0,1,0);
	glVertex3f(30,-30,0);
	glColor3f(0,0,1);
	glVertex3f(-30,30,0);
	glEnd();
	glBegin(GL_QUADS);
	glColor3f(1,1,1);
	glVertex3f(-30,30,60);
	glVertex3f(30,30,0);
	glVertex3f(30,0,0);
	glVertex3f(-30,0,0);
	glEnd();
	glPopMatrix();
	glutSwapBuffers();
}
int main(int argc, char **argv)
{
  glutInit(&argc,argv);                                   	//initializes the GLUT framework
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);   //sets up the display mode
  glutCreateWindow("My first GLUT program");                  //creates a window
  glutDisplayFunc(redraw);

glMatrixMode(GL_PROJECTION);   //changes the current matrix to the projection matrix
  gluPerspective(45, 	//view angle
             	1.0,    //aspect ratio
             	10.0,   //near clip
             	200.0); //far clip

glMatrixMode(GL_MODELVIEW);   //changes the current matrix to the modelview matrix

  glutMainLoop();           	//the main loop of the GLUT framework

  return 0;
}
