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
#define NUMBER_OF_BALLS 250
//#define INTERVAL 0.0049999
#define BALL_SIZE 0.02
#define BALL_SPEED 2.00 // ASU's per second
#define CURVE_LENGTH_APPROX 8
#define RAINBOW 1 // 1 = multi colored spheres, 0 = red

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// global variables
int window; 	//id of the window
static double s = 0.5; // tightness of the paths (0.0 - tight, 0.5 - loose)
static double start;
double current;
double progress;
double x,y;
double x2,y2;
double x3,y3;
double x4,y4;
double x5,y5;
double x6,y6;
double x7,y7;
double x8,y8;
double x9,y9;
struct sphere{
	double xPos, yPos, interval;
	double x1, x2, x3, x4, y1, y2, y3, y4;
	double size;
	double red;
	double green;
	double blue;
	double speed;
	double curve_length;
	double start_time;
	double curve_time;
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
void print_sphere_info(struct sphere ball);

void animate() {
	progress = (progress >= 1.0) ? 0.0 : progress+0.001;	
	// We must set the current window, since a window isn't
	// set before this function is called
	glutSetWindow(window);
	//  Ask Glut to redisplay the current window
	glutPostRedisplay();
}
     
void gfxinit() {
	x = 5.0; y = 3.0;
	x2 = 2.0; y2 = 8.0;
	x3 = 3.0; y3 = 7.0;
	x4 = 4.0; y4 = 6.0;
	x5 = 5.0; y5 = 5.0;
	x6 = 6.0; y6 = 4.0;
	x7 = 7.0; y7 = 3.0;
	x8 = 8.0; y8 = -2.0;
	x9 = 9.0; y9 = -1.0;
	start = (double) clock(); 
	progress = 0.0;
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
		all_spheres[k].speed = BALL_SPEED;	
		//all_spheres[k].speed = new_random_value();
		//all_spheres[k].speed = (all_spheres[k].speed < 0) ? 
		//						all_spheres[k].speed * -1 :
		//						all_spheres[k].speed;
		all_spheres[k].curve_length = curve_length(
										all_spheres[k].x1,
										all_spheres[k].x2,
										all_spheres[k].x3,
										all_spheres[k].x4,
										all_spheres[k].y1,
										all_spheres[k].y2,
										all_spheres[k].y3,
										all_spheres[k].y4
				);
		
		all_spheres[k].start_time = (double) clock();
		all_spheres[k].curve_time = all_spheres[k].curve_length / 
									all_spheres[k].speed;
		
		if(RAINBOW) {
			//multi-colored spheres
			all_spheres[k].red = fmod(new_random_value(),1.0);
			all_spheres[k].green = fmod(new_random_value(),1.0);
			all_spheres[k].blue	= fmod(new_random_value(),1.0);
		} else {
			//red spheres
			all_spheres[k].red = 1.0;
			all_spheres[k].green = 0.0;
			all_spheres[k].blue	= 0.0;
		}
		//print_sphere_info(all_spheres[k]);
		
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

double new_random_value() {
	return ((rand() % (99)/100.)*10)-5.0;
	
}

void set_curve_length( struct sphere ball) {
	ball.curve_length = 1.0;
}

double get_position( double p1, double p2, double p3, double p4, double pos) {
	double result;
	double a, b, c;
	c = 3 * (p2 - p1);
	b = 3 * (p3 - p2) - c;
	a = p4 - p1 - c - b;
	result = a * pow(pos,3) + b * pow(pos,2) + c * pos + p1;
	
	return result;
}

double curve_length( double p1x, double p2x, double p3x, double p4x, 
					 double p1y, double p2y, double p3y, double p4y) {
	float i;
	double x1, y1, x2, y2;
	double total = 0;
	x1 = get_position( p1x, p2x, p3x, p4x, 0);
	y1 = get_position( p1y, p2y, p3y, p4y, 0);
	for(i = 1./CURVE_LENGTH_APPROX ; i <= 1; i+= 1./CURVE_LENGTH_APPROX) {
		//printf("%f\n", i);
		x2 = get_position( p1x, p2x, p3x, p4x, i);
		y2 = get_position( p1y, p2y, p3y, p4y, i);
		total = total + sqrt( pow(x2-x1,2) + pow(y2-y1,2) );
		x1 = x2;
		y1 = y2;
	}
	return total;
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	// red sphere
	glPushMatrix();
		glColor3f(1.0, 0.0, 0.0);
		glTranslatef(0,0,0);
		glutSolidSphere(0.5,25,25);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(x*progress,y*progress,0);
		glutSolidSphere(0.04,25,25);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(x2*progress,y2*progress,0);
		glutSolidSphere(0.04,25,25);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(x3*progress,y3*progress,0);
		glutSolidSphere(0.04,25,25);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(x4*progress,y4*progress,0);
		glutSolidSphere(0.04,25,25);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(x5*progress,y5*progress,0);
		glutSolidSphere(0.04,25,25);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(x6*progress,y6*progress,0);
		glutSolidSphere(0.04,25,25);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(x7*progress,y7*progress,0);
		glutSolidSphere(0.04,25,25);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(x8*progress,y8*progress,0);
		glutSolidSphere(0.04,25,25);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(x9*progress,y9*progress,0);
		glutSolidSphere(0.04,25,25);
	glPopMatrix();
	
	glutSwapBuffers();
}
void print_sphere_info(struct sphere ball) {
	printf("x: %f %f %f %f\n", 
			ball.x1,
			ball.x2,
			ball.x3,
			ball.x4
		  );
	printf("y: %f %f %f %f\n", 
			ball.y1,
			ball.y2,
			ball.y3,
			ball.y4
		  );
	printf("speed %f\n", ball.speed);
	printf("start time %f\n", ball.start_time);
	printf("curve length %f\n",ball.curve_length);
	printf("curve time %f\n", ball.curve_time);
	printf("interval: %f\n", ball.interval);
}
