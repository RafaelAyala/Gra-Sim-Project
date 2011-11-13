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

#define NUMBER_OF_BALLS 150
#define INTERVAL 0.0049999
#define BALL_SIZE 0.25
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int window; 	//id of the window
static double s = 0.5; // tightness of the paths (0.0 - tight, 0.5 - loose)

struct sphere{
	double xPos, yPos, interval;
	double x1, x2, x3, x4, y1, y2, y3, y4;
	double size;
	double red;
	double green;
	double blue;
} all_spheres[NUMBER_OF_BALLS];

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
	glColor3f(1.0, 1.0, 1.0);
	
	glBegin(GL_QUADS);
		glVertex3f(-20.0, -20.0, -1.0);
		glVertex3f(20.0, -20.0, -1.0);
		glVertex3f(20.0, 20.0, -1.0);
		glVertex3f(-20.0, 20.0, -1.0);
	glEnd();
	int i;
	for(i = 0; i < NUMBER_OF_BALLS; i++) {
		// red sphere
		glPushMatrix();
		glColor3f(all_spheres[i].red,all_spheres[i].green,all_spheres[i].blue);
		glPushMatrix();
		glTranslatef(all_spheres[i].xPos,all_spheres[i].yPos,0);
		glutSolidSphere(all_spheres[i].size,25,25);
		glPopMatrix();
	}
	glutSwapBuffers();
}

void animate() {
	int j;
	float a, b, c;
	for(j = 0; j < NUMBER_OF_BALLS; j++) {
		if( all_spheres[j].interval < 1.0) {
			c = 3 * (all_spheres[j].x2 - all_spheres[j].x1);
			b = 3 * (all_spheres[j].x3 - all_spheres[j].x2) - c;
			a = all_spheres[j].x4 - all_spheres[j].x1 - c - b;
			all_spheres[j].xPos = a * pow(all_spheres[j].interval,3) + b * pow(all_spheres[j].interval,2) + c * all_spheres[j].interval + all_spheres[j].x1;
			
			c = 3 * (all_spheres[j].y2 - all_spheres[j].y1);
			b = 3 * (all_spheres[j].y3 - all_spheres[j].y2) - c;
			a = all_spheres[j].y4 - all_spheres[j].y1 - c - b;
			all_spheres[j].yPos = a * pow(all_spheres[j].interval,3) + b * pow(all_spheres[j].interval,2) + c * all_spheres[j].interval + all_spheres[j].y1;
			
			all_spheres[j].interval+=INTERVAL;

		} else {
			all_spheres[j].interval = 0.0;
			all_spheres[j].x2 = (all_spheres[j].x4 - all_spheres[j].x3) + 
				all_spheres[j].x4;
			all_spheres[j].x1 = all_spheres[j].x4;
			all_spheres[j].x3 = new_random_value();
			all_spheres[j].x4 = new_random_value();
			
			all_spheres[j].y2 = (all_spheres[j].y4 - all_spheres[j].y3) + 
				all_spheres[j].y4;
			all_spheres[j].y1 = all_spheres[j].y4;
			all_spheres[j].y3 = new_random_value();
			all_spheres[j].y4 = new_random_value();
		}
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
	// global ambient
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	// turn on lighting
    glEnable(GL_LIGHTING);
    // enable light 0, all the other lights are off
    glEnable(GL_LIGHT0);


    // enable the depth buffer
    glEnable(GL_DEPTH_TEST);
   
    glMatrixMode(GL_PROJECTION);
    gluPerspective(60.0, 16/9., 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glEnable ( GL_COLOR_MATERIAL ) ;
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glShadeModel(GL_SMOOTH);

	srand(time(NULL));
	int k;
	for( k = 0; k < NUMBER_OF_BALLS; k++ ) {
		
		all_spheres[k].x1 = 0.0;
		all_spheres[k].x2 = new_random_value();
		all_spheres[k].x3 = new_random_value();
		all_spheres[k].x4 = new_random_value();

		all_spheres[k].y1 = 0.0;
		all_spheres[k].y2 = new_random_value();
		all_spheres[k].y3 = new_random_value();
		all_spheres[k].y4 = new_random_value();
		
		all_spheres[k].size = BALL_SIZE;
		
		//red spheres
		//all_spheres[k].red = 1.0;
		//all_spheres[k].green = 0.0;
		//all_spheres[k].blue	= 0.0;
		
		//colored spheres
		all_spheres[k].red = fmod(new_random_value(),1.0);
		all_spheres[k].green = fmod(new_random_value(),1.0);
		all_spheres[k].blue	= fmod(new_random_value(),1.0);
	}
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
