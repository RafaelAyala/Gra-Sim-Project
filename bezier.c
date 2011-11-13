/*
 * Rafael Ayala
 * 100XXXXXX
 *
 * Jordan Stadler
 * 100366066
 *
 * Final Project
 */

#ifdef __APPLE__  	// Mac OpenGL Libraries
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else				// Linux OpenGL Libraries
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
// Common Libraries
#include <stdio.h> 
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define NUMBER_OF_BALLS 100
#define INTERVAL 0.0001
#define BALL_SIZE 0.1
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int window; 	//id of the window
static double s = 0.5; // tightness of the paths (0.0 - tight, 0.5 - loose)

double new_random_value() {
	return ((rand() % (99)/100.)*20)-10.0;
	
}

// [q] is quit
void keystroke(unsigned char c, int x, int y) {
	switch(c) {
		case 113:
			exit(0);
			break;
	}
}

void display() {
	//printf("hello\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	float p0x = -4;
	float p0y = -9;
	float p1x = -4.4;
	float p1y = -25.8;
	float p2x = -4.4;
	float p2y = 8.2;
	float p3x = -0.6;
	float p3y = 3.4;
	float a, b, c, x, y;

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	float u = 0;
	while(u<=1.0) {
		c = 3 * (p1x - p0x);
		b = 3 * (p2x - p1x) - c;
		a = p3x - p0x - c - b;
		x = a * pow(u,3) + b * pow(u,2) + c * u + p0x;
		c = 3 * (p1y - p0y);
		b = 3 * (p2y - p1y) - c;
		a = p3y - p0y - c - b;
		y = a * pow(u,3) + b * pow(u,2) + c * u + p0y;
		
		glVertex3f(x,y,0);
		u = u + 0.01;
	}
	glEnd();
	glColor3f(0.0, 1.0, 0.0);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
		glVertex3f(p0x, p0y, 0);
		glVertex3f(p3x, p3y, 0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(p1x, p1y, 0);
		glVertex3f(p2x, p2y, 0);
	glEnd();
	printf("%f\n",(double) CLOCKS_PER_SEC);
	//p0x = 2;
	//p0y = 0;
	//p1x = 2;
	//p1y = -1;
	//p2x = 4;
	//p2y = 0;
	//p3x = 4;
	//p3y = 1;
	//
	//glColor3f(1.0, 0.0, 0.0);
	//glBegin(GL_LINE_STRIP);
	//u = 0;
	//while(u<=1.0) {
	//	c = 3 * (p1x - p0x);
	//	b = 3 * (p2x - p1x) - c;
	//	a = p3x - p0x - c - b;
	//	x = a * pow(u,3) + b * pow(u,2) + c * u + p0x;
	//	c = 3 * (p1y - p0y);
	//	b = 3 * (p2y - p1y) - c;
	//	a = p3y - p0y - c - b;
	//	y = a * pow(u,3) + b * pow(u,2) + c * u + p0y;
	//	
	//	glVertex3f(x,y,0);
	//	u = u + 0.01;
	//}
	//glEnd();

	glColor3f(0.0, 1.0, 0.0);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
		glVertex3f(p0x, p0y, 0);
		glVertex3f(p3x, p3y, 0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(p1x, p1y, 0);
		glVertex3f(p2x, p2y, 0);
	glEnd();
	
	
	glutSwapBuffers();
}

void animate() {
	
	
	// We must set the current window, since a window isn't
	// set before this function is called
	glutSetWindow(window);
	//  Ask Glut to redisplay the current window
	glutPostRedisplay();
}
     

void gfxinit() {
 
   
    glMatrixMode(GL_PROJECTION);
    gluPerspective(120.0, 16/9., 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glEnable ( GL_COLOR_MATERIAL ) ;

	srand(time(NULL));
}


int main(int argc, char **argv) {
     // Initialize GLUT and process its command line arguments
     glutInit(&argc,argv);
     // Set the display mode, double buffered, RGB colour and depth buffer
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
     // Create the display window
     window = glutCreateWindow("first");
     // Set the display function
     glutDisplayFunc(display);
     // Set the Idle function, called when GLUT isn't busy
     glutIdleFunc(animate);
     glutKeyboardFunc(keystroke);
	 // Initialize OpenGL
	 gfxinit();
     // Pass control to GLUT to run the application
     glutMainLoop();
   
	//return 0;
}
