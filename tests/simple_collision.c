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

// configuration
#define NUMBER_OF_BALLS 3
//#define INTERVAL 0.0049999
#define BALL_SIZE 0.5
#define BALL_SPEED 2.0 // ASU's per second
#define CURVE_LENGTH_APPROX 8
#define RAINBOW 1 // 1 = multi colored spheres, 0 = red

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// global variables
int window; 	//id of the window
static double s = 0.5; // tightness of the paths (0.0 - tight, 0.5 - loose)
static double start;
double current;
double temp;

struct sphere{
	double xPos, yPos, interval;
	//double x1, x2, x3, x4, y1, y2, y3, y4;
	double size;
	double red;
	double green;
	double blue;
	double speed;
	//double curve_length;
	//double start_time;
	//double curve_time;
	double delta_x;
	double delta_y;
	double mass;
} all_spheres[NUMBER_OF_BALLS];

// prototypes
double new_random_value();
void set_curve_length( struct sphere ball);
double get_position( double p1, double p2, double p3, double p4, double pos);
double curve_length( double p1x, double p2x, double p3x, double p4x, 
					 double p1y, double p2y, double p3y, double p4y);
void keystroke(unsigned char c, int x, int y);
void display();
void animate();
void gfxinit();
//void print_sphere_info(struct sphere ball);

void animate() {
	int j;
	double a, b, c;

	while((double) clock() == current){} // waits for next time step
	while((double) clock() == current){} // waits for next time step
	//current = (double) clock();
	//while((double) clock() == current){} // waits for next time step
	//current = (double) clock();
	//while((double) clock() == current){} // waits for next time step
	//current = (double) clock();
	
	double tempX, tempY;

	for(j = 0; j < NUMBER_OF_BALLS; j++) {	
		//all_spheres[j].xPos += all_spheres[j].delta_x * 1./CLOCKS_PER_SEC;
		//all_spheres[j].yPos += all_spheres[j].delta_y * 1./CLOCKS_PER_SEC;
		all_spheres[j].xPos += all_spheres[j].delta_x * all_spheres[j].speed;
		all_spheres[j].yPos += all_spheres[j].delta_y * all_spheres[j].speed;
	}
	
	// We must set the current window, since a window isn't
	// set before this function is called
	glutSetWindow(window);
	//  Ask Glut to redisplay the current window
	glutPostRedisplay();
}
     
void gfxinit() {
	start = (double) clock(); 
	current = 0.0;
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
    //gluPerspective(60.0, 16/9., 1.0, 20.0);
    glOrtho(-5.0, 5.0,5.0,-5.0 ,   1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glEnable ( GL_COLOR_MATERIAL ) ;
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glShadeModel(GL_SMOOTH);

	srand(time(NULL));
	int k;
	

	// ball 1
	// starts at (1.0,0.0)
	// moves in x direction
	all_spheres[0].size = BALL_SIZE;
	all_spheres[0].xPos = -4.0;
	all_spheres[0].yPos = 1.0;
	all_spheres[0].speed = 0.1; // units per second
	all_spheres[0].delta_x = 1.0;
	all_spheres[0].delta_y = 0.0;
	all_spheres[0].red = 0.7;
	all_spheres[0].green = 0.0;
	all_spheres[0].blue = 0.0;
	all_spheres[0].mass = 1.0;

	// ball 2
	// starts at (-1.0,0.0)
	// moves in y direction
	all_spheres[1].size = BALL_SIZE;
	all_spheres[1].xPos = 0.0;
	all_spheres[1].yPos = 0.0;
	all_spheres[1].speed = 0.2 ; // units per second
	all_spheres[1].delta_x = 0.0;
	all_spheres[1].delta_y = 1.0;
	all_spheres[1].red = 0.0;
	all_spheres[1].green = 0.7;
	all_spheres[1].blue = 0.0;
	
	// ball 2
	// starts at (-1.0,0.0)
	// moves in y direction
	all_spheres[2].size = BALL_SIZE;
	all_spheres[2].xPos = -2.0;
	all_spheres[2].yPos = -2.0;
	all_spheres[2].speed = 0.3; // units per second
	all_spheres[2].delta_x = 0.2;
	all_spheres[2].delta_y = 0.2;
	all_spheres[2].red = 0.0;
	all_spheres[2].green = 0.0;
	all_spheres[2].blue = 0.7;
}


// [q] is quit
void keystroke(unsigned char c, int x, int y) {
	int q;
	switch(c) {
		case 113:
			exit(0);
			break;
		case 117:	//Increase the speed of ball 1 
			all_spheres[0].speed+=0.01;
			break;
		case 105:	//increase the speed of ball 2	
			all_spheres[1].speed+=0.01;
			break;
		case 106:	//Increase the speed of ball 1 
			all_spheres[0].speed = (all_spheres[0].speed <= 0.01) ? 0.0 : 
				all_spheres[0].speed-0.01;
			printf("new speed of ball 0: %f\n", all_spheres[0].speed);
			break;
		case 107:	//increase the speed of ball 2	
			all_spheres[1].speed = (all_spheres[1].speed <= 0.01) ? 0.0 : 
				all_spheres[1].speed-0.01;
			printf("new speed of ball 1: %f\n", all_spheres[1].speed);
			break;
		case 100:	
			for( q = 0; q < 10000; q++) {
			glutIdleFunc(animate);
			}
			break;

	}
}

void normalize(struct sphere ball) {
		double mag;
		mag = sqrt(pow(ball.delta_x,2) + pow(ball.delta_y,2));
		ball.delta_x /= mag;
		ball.delta_y /= mag;
}

void collision_check() {
	int i, j;
	double mag;
	for(i = 0; i < NUMBER_OF_BALLS; i++) {
		double dist = 5.0 - all_spheres[i].size;
		if(all_spheres[i].xPos >= dist || all_spheres[i].xPos <= -1*dist) {
			all_spheres[i].delta_x *= -1;
		}
		
		if(all_spheres[i].yPos >= dist || all_spheres[i].yPos <= -1*dist) {
			all_spheres[i].delta_y *= -1;
		}
		for( j=i+1; j < NUMBER_OF_BALLS; j++) {
			// check all but itself
			//if( i == j) { j++; }
			
			// if collision distance in x
			if( fabs(all_spheres[i].xPos - all_spheres[j].xPos) < 
					0.9 ) {
					//(all_spheres[i].size + all_spheres[j].size)  ) {
				// and collision distance in y
				if( fabs(all_spheres[i].yPos - all_spheres[j].yPos) < 
						0.9 ){
						//(all_spheres[i].size + all_spheres[j].size)  ) {
					// collision occurs
					printf("collision\n");	
					printf("ball1 -- xPos: %f yPos: %f\n", 
							all_spheres[0].xPos, all_spheres[0].yPos); 	
					printf("ball2 -- xPos: %f yPos: %f\n", 
							all_spheres[1].xPos, all_spheres[1].yPos); 	
		
					int k;
					//glPushMatrix();
					//glColor3f(0.0,0.0,0.7);
					//glTranslatef( fabs(all_spheres[i].xPos - all_spheres[j].xPos)/2. + 
					//		((all_spheres[i].xPos < all_spheres[j].xPos) ? 
					//		 all_spheres[i].xPos : all_spheres[j].xPos),
					//		fabs(all_spheres[i].yPos - all_spheres[j].yPos)/2. +
					//		((all_spheres[i].yPos < all_spheres[j].yPos) ?
					//		 all_spheres[i].yPos : all_spheres[j].yPos),
					//		0);
					//glutSolidSphere(0.2,25,25);
					//glPopMatrix();
					
					//glutIdleFunc(NULL);	
					
					//for(j = 0; k < NUMBER_OF_BALLS; k++) {
					//	// red sphere
					//	glPushMatrix();
					//	glColor3f(all_spheres[k].red,all_spheres[k].green,all_spheres[k].blue);
					//	glTranslatef(all_spheres[k].xPos,all_spheres[k].yPos,0);
					//	glutSolidSphere(all_spheres[k].size,25,25);
					//	glPopMatrix();
					//}
					



					// ball 1 pos adjustment
					//all_spheres[i].xPos -= all_spheres[i].delta_x;
					//all_spheres[i].yPos -= all_spheres[i].delta_y;
					// ball 1 velocity adjustment
					all_spheres[i].delta_x = all_spheres[i].xPos - all_spheres[j].xPos;
					all_spheres[i].delta_y = all_spheres[i].yPos - all_spheres[j].yPos;
					// normalize velocity
					//mag = sqrt(pow(all_spheres[i].delta_x,2) + pow(all_spheres[i].delta_y,2));
					//all_spheres[i].delta_x /= mag;
					//all_spheres[i].delta_y /= mag;
					normalize(all_spheres[i]);

					// ball 2 pos adjustment
					//all_spheres[j].xPos -= all_spheres[j].delta_x;
					//all_spheres[j].yPos -= all_spheres[j].delta_y;
					// ball 1 velocity adjustment
					all_spheres[j].delta_x = all_spheres[j].xPos - all_spheres[i].xPos;
					all_spheres[j].delta_y = all_spheres[j].yPos - all_spheres[i].yPos;
					// normalize velocity
					//mag = sqrt(pow(all_spheres[j].delta_x,2) + pow(all_spheres[j].delta_y,2));
					//all_spheres[j].delta_x /= mag;
					//all_spheres[j].delta_y /= mag;
					normalize(all_spheres[i]);
				}
			}
		}
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_QUADS);
		glVertex3f(20.0, -20.0, -1.0);
		glVertex3f(-20.0, -20.0, -1.0);
		glVertex3f(-20.0, 20.0, -1.0);
		glVertex3f(20.0, 20.0, -1.0);
	glEnd();
	int i;

	for(i = 0; i < NUMBER_OF_BALLS; i++) {
		// red sphere
		glPushMatrix();
		glColor3f(all_spheres[i].red,all_spheres[i].green,all_spheres[i].blue);
		glTranslatef(all_spheres[i].xPos,all_spheres[i].yPos,0);
		glutSolidSphere(all_spheres[i].size,25,25);
		glPopMatrix();
	}
	collision_check();

	
	//	collision_check();
	
	//printf("%f\n", clock());
	glutSwapBuffers();
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
