/*
* CSCI 3090U
* Assignment 2 - Part 2
* Feb. 20, 2010
*
* Jordan Stadler
* 100366066
*/

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h> 
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
int window; 	//id of the window
double x,y,a,b,c,d,u,v,w;
double x1,x2,x3,x4,l1,y2,y3,y4;
double a1,a2,a3,a4,b1,b2,b3,b4;
double c1,c2,c3,c4,d1,d2,d3,d4;
static double s = 0.5;
 
void keystroke(unsigned char c, int x, int y) {
	switch(c) {
		case 113:
			exit(0);
			break;
		case 110:
			x1 = x2;
			l1 = y2;
			x2 = x3; y2 = y3;
			x3 = x4; y3 = y4;
			x4 = ((rand() % (99)/100.)*5)-2.5;
			y4 = ((rand() % (99)/100.)*5)-2.5;
			break;
		case 111:
			a1 = a2;
			b1 = b2;
			a2 = a3; b2 = b3;
			a3 = a4; b3 = b4;
			a4 = ((rand() % (99)/100.)*5)-2.5;
			b4 = ((rand() % (99)/100.)*5)-2.5;
			break;
		case 112:
			c1 = c2;
			d1 = d2;
			c2 = c3; d2 = d3;
			c3 = c4; d3 = d4;
			c4 = ((rand() % (99)/100.)*5)-2.5;
			d4 = ((rand() % (99)/100.)*5)-2.5;
			break;
	}
}
void display() {
	//printf("hello\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// red sphere
	glPushMatrix();
	glColor3f(1.0,0.0,0.0);
	glPushMatrix();
	glTranslatef(x,y,0);
	glutSolidSphere(0.25,25,25);
	glPopMatrix();
	
	// green sphere
	glPushMatrix();
	glColor3f(0.0,1.0,0.0);
	glPushMatrix();
	glTranslatef(a,b,0);
	glutSolidSphere(0.25,25,25);
	glPopMatrix();
	
	// blue sphere
	glPushMatrix();
	glColor3f(0.0,0.0,1.0);
	glPushMatrix();
	glTranslatef(c,d,0);
	glutSolidSphere(0.25,25,25);
	glPopMatrix();
	glutSwapBuffers();
}

void animate() {
	if( u < 1.0) {
		x = (s * ((-1*u) + (2*pow(u,2)) + (-1*pow(u,3))) * x1) +
			(s * (pow(u,2) + (-1 * pow(u,3))) * x2) +
			((1 - (3*pow(u,2)) + (2*pow(u,3))) * x2) +
			(s * (u - (2*pow(u,2)) + pow(u,3)) * x3) +
			(((3*pow(u,2)) - (2*pow(u,3))) * x3) +
			((s * (pow(u,3) - pow(u,2))) * x4);
		y = (s * ((-1*u) + (2*pow(u,2)) + (-1*pow(u,3))) * l1) +
			(s * (pow(u,2) + (-1 * pow(u,3))) * y2) +
			((1 - (3*pow(u,2)) + (2*pow(u,3))) * y2) +
			(s * (u - (2*pow(u,2)) + pow(u,3)) * y3) +
			(((3*pow(u,2)) - (2*pow(u,3))) * y3) +
			((s * (pow(u,3) - pow(u,2))) * y4);
		u+=0.001;
	}else{
		u = 0.0;
		keystroke('n', 0, 0);
	}
	
	if( v < 1.0) {
		a = (s * ((-1*v) + (2*pow(v,2)) + (-1*pow(v,3))) * a1) +
			(s * (pow(v,2) + (-1 * pow(v,3))) * a2) +
			((1 - (3*pow(v,2)) + (2*pow(v,3))) * a2) +
			(s * (v - (2*pow(v,2)) + pow(v,3)) * a3) +
			(((3*pow(v,2)) - (2*pow(v,3))) * a3) +
			((s * (pow(v,3) - pow(v,2))) * a4);
		b = (s * ((-1*v) + (2*pow(v,2)) + (-1*pow(v,3))) * b1) +
			(s * (pow(v,2) + (-1 * pow(v,3))) * b2) +
			((1 - (3*pow(v,2)) + (2*pow(v,3))) * b2) +
			(s * (v - (2*pow(v,2)) + pow(v,3)) * b3) +
			(((3*pow(v,2)) - (2*pow(v,3))) * b3) +
			((s * (pow(v,3) - pow(v,2))) * b4);
		v+=0.001;
	}else{
		v = 0.0;
		keystroke(111, 0, 0);
	}
	
	if( w < 1.0) {
		c = (s * ((-1*w) + (2*pow(w,2)) + (-1*pow(w,3))) * c1) +
			(s * (pow(w,2) + (-1 * pow(w,3))) * c2) +
			((1 - (3*pow(w,2)) + (2*pow(w,3))) * c2) +
			(s * (w - (2*pow(w,2)) + pow(w,3)) * c3) +
			(((3*pow(w,2)) - (2*pow(w,3))) * c3) +
			((s * (pow(w,3) - pow(w,2))) * c4);
		d = (s * ((-1*w) + (2*pow(w,2)) + (-1*pow(w,3))) * d1) +
			(s * (pow(w,2) + (-1 * pow(w,3))) * d2) +
			((1 - (3*pow(w,2)) + (2*pow(w,3))) * d2) +
			(s * (w - (2*pow(w,2)) + pow(w,3)) * d3) +
			(((3*pow(w,2)) - (2*pow(w,3))) * d3) +
			((s * (pow(w,3) - pow(w,2))) * d4);
		w+=0.001;
	}else{
		w = 0.0;
		keystroke(112, 0, 0);
	}
	// We must set the current window, since a window isn't
	// set before this function is called
	glutSetWindow(window);
	//  Ask Glut to redisplay the current window
	glutPostRedisplay();
}
     

void gfxinit() {
 
	// LIGHTING 	
    GLfloat lightpos[4] = { 1.0, 0.0, 1.0, 1.0 };     // light position
    GLfloat lightamb[4] = { 0.0, 0.0, 0.0, 1.0 };     // ambient colour
    GLfloat lightdif[4] = { 1.0, 1.0, 1.0, 1.0 };     // diffuse colour
    GLfloat global_ambient[4] = {0.2, 0.2, 0.2, 1};

    // set the light position, 5 units along the y axis
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    // set the ambient light colour
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
    // set the diffuse light colour
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
    // set the specular light colour
    //glLightfv(GL_LIGHT0, GL_SPECULAR,lightspec);
	// global ambient
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	// turn on lighting
    glEnable(GL_LIGHTING);
    // enable light 0, all the other lights are off
    glEnable(GL_LIGHT0);


    // enable the depth buffer
    glEnable(GL_DEPTH_TEST);
   
    glMatrixMode(GL_PROJECTION);
    gluPerspective(60.0, 1.0, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glEnable ( GL_COLOR_MATERIAL ) ;
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glShadeModel(GL_SMOOTH);

	srand(time(NULL));
	x1 = 0;
	l1 = 0;
	x2 = ((rand() % (99)/100.)*10)-5.0;
	y2 = ((rand() % (99)/100.)*10)-5.0;
	x3 = ((rand() % (99)/100.)*10)-5.0;
	y3 = ((rand() % (99)/100.)*10)-5.0;
	x4 = ((rand() % (99)/100.)*10)-5.0;
	y4 = ((rand() % (99)/100.)*10)-5.0;
	a1 = 0;
	b1 = 0;
	a2 = ((rand() % (99)/100.)*10)-5.0;
	b2 = ((rand() % (99)/100.)*10)-5.0;
	a3 = ((rand() % (99)/100.)*10)-5.0;
	b3 = ((rand() % (99)/100.)*10)-5.0;
	a4 = ((rand() % (99)/100.)*10)-5.0;
	b4 = ((rand() % (99)/100.)*10)-5.0;
	c1 = 0;
	d1 = 0;
	c2 = ((rand() % (99)/100.)*10)-5.0;
	d2 = ((rand() % (99)/100.)*10)-5.0;
	c3 = ((rand() % (99)/100.)*10)-5.0;
	d3 = ((rand() % (99)/100.)*10)-5.0;
	c4 = ((rand() % (99)/100.)*10)-5.0;
	d4 = ((rand() % (99)/100.)*10)-5.0;
     
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
