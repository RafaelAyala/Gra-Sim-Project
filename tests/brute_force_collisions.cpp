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
#define NUMBER_OF_BALLS 5
//#define INTERVAL 0.0049999
#define BALL_SIZE 0.2
#define BALL_SPEED 3.0 // ASU's per second
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
	double x1, x2, x3, x4, y1, y2, y3, y4;
	double size;
	double red;
	double green;
	double blue;
	double speed;
	double curve_length;
	double start_time;
	double curve_time;
	int path;  // 0 is linear path, 1 is a bezier curve
	double delta_x, delta_y;
	double mass;
} all_spheres[NUMBER_OF_BALLS];

double new_random_value() {
	return ((rand() % (101)/100.)*10)-5.0;	
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
	double i;
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

void normalize(struct sphere ball) {
	double mag;
	mag = sqrt(pow(ball.delta_x,2) + pow(ball.delta_y,2));
	ball.delta_x /= mag;
	ball.delta_y /= mag;
}

double distance( struct sphere ball1, struct sphere ball2 ) {
	return sqrt( pow(ball1.xPos - ball2.xPos,2) + pow(ball1.yPos - ball2.yPos,2));
}

void animate() {
	int j;
	double a, b, c;

	while((double) clock() == current){} // waits for next time step
	current = (double) clock();
	
	double tempX, tempY;

	for(j = 0; j < NUMBER_OF_BALLS; j++) {
		
		
		if(all_spheres[j].path == 0) { //linear paths
			// TODO
			all_spheres[j].xPos += all_spheres[j].delta_x * all_spheres[j].speed;
			all_spheres[j].yPos += all_spheres[j].delta_y * all_spheres[j].speed;



		} else if(all_spheres[j].path == 1) { // bezier curves for path	
			if( all_spheres[j].interval < 1.0) {
			
				tempX = all_spheres[j].xPos;
				tempY = all_spheres[j].yPos;
				all_spheres[j].interval = (current - all_spheres[j].start_time)/ 
											(CLOCKS_PER_SEC * all_spheres[j].curve_time);
				all_spheres[j].xPos = get_position(
											all_spheres[j].x1,
											all_spheres[j].x2,
											all_spheres[j].x3,
											all_spheres[j].x4,
											all_spheres[j].interval
						);
				all_spheres[j].yPos = get_position(
											all_spheres[j].y1,
											all_spheres[j].y2,
											all_spheres[j].y3,
											all_spheres[j].y4,
											all_spheres[j].interval
						);
			
				//printf("current speed: %f AMU/s\n", 
				//		sqrt( pow(all_spheres[j].xPos - tempX ,2) 
				//				+ pow(all_spheres[j].yPos - tempY ,2) )
				//);
				//all_spheres[j].interval = (current - all_spheres[j].start_time)/ 
				//							(CLOCKS_PER_SEC * all_spheres[j].curve_time);
			} else {
				//printf("Time spent on curve: %fs\n", (current-all_spheres[j].start_time) /
				//		CLOCKS_PER_SEC);
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
				all_spheres[j].curve_length = curve_length(
												all_spheres[j].x1,
												all_spheres[j].x2,
												all_spheres[j].x3,
												all_spheres[j].x4,
												all_spheres[j].y1,
												all_spheres[j].y2,
												all_spheres[j].y3,
												all_spheres[j].y4
						);
				all_spheres[j].start_time = (double) clock();
				all_spheres[j].curve_time = all_spheres[j].curve_length / 
											all_spheres[j].speed;
				//print_sphere_info(all_spheres[j]);
			}
			//print_sphere_info(all_spheres[j]);
		}	
	
	}
	
	// We must set the current window, since a window isn't
	// set before this function is called
	glutSetWindow(window);
	//  Ask Glut to redisplay the current window
	glutPostRedisplay();
}
     


void collision_check() {
	int i, j;
	double d;
	
	for(i = 0; i < NUMBER_OF_BALLS; i++) {
		
		// ball-wall collisions
		//TODO
		// speed are hard coded,
		// tangents need to be used
		// deltas need real values
		
		double dist = 5.0 - all_spheres[i].size;
		//right
		if(all_spheres[i].xPos >= dist ) {
			all_spheres[i].delta_x = -1;
			all_spheres[i].delta_y = 0;
			all_spheres[i].xPos = dist;
			all_spheres[i].path = 0;
			all_spheres[i].speed = 0.1;

		//left
		} else if(all_spheres[i].xPos <= -1*dist){
			all_spheres[i].delta_x = 1;
			all_spheres[i].delta_y = 0;
			all_spheres[i].xPos = -1*dist;
			all_spheres[i].path = 0;
			all_spheres[i].speed = 0.1;
		//top
		} else if(all_spheres[i].yPos >= dist ) {
			all_spheres[i].delta_x = 0;
			all_spheres[i].delta_y = -1;
			all_spheres[i].yPos = dist;
			all_spheres[i].path = 0;
			all_spheres[i].speed = 0.1;
			//all_spheres[i].delta_y *= -1;
			//all_spheres[i].yPos = dist;
		//bottom
		}else if(all_spheres[i].yPos <= -1*dist){
			all_spheres[i].delta_x = 0;
			all_spheres[i].delta_y = 1;
			all_spheres[i].yPos = -1*dist;
			all_spheres[i].path = 0;
			all_spheres[i].speed = 0.1;
			//all_spheres[i].delta_y *= -1;
			//all_spheres[i].yPos = -1*dist;
		}
		// end ball-wall collisions
		
		for( j=0; j < NUMBER_OF_BALLS; j++) {
			// check all but itself
			if( i == j) { j++; }
			
			// if collision distance in x
		//	if( fabs(all_spheres[i].xPos - all_spheres[j].xPos) <= 
		//			(all_spheres[i].size + all_spheres[j].size)  ) {
		//		// and collision distance in y
		//		if( fabs(all_spheres[i].yPos - all_spheres[j].yPos) <= 
		//				(all_spheres[i].size + all_spheres[j].size)  ) {
		
			d = distance(all_spheres[i], all_spheres[j]);

			if( d <= all_spheres[i].size + all_spheres[j].size) {
				//printf("collision\n");	
				//if(d > 0) {
					
				
				all_spheres[i].delta_x = all_spheres[i].xPos - all_spheres[j].xPos;
				all_spheres[i].delta_y = all_spheres[i].yPos - all_spheres[j].yPos;
				all_spheres[i].path = 0;
				normalize(all_spheres[i]);
     			all_spheres[i].speed = 0.1;

				all_spheres[j].delta_x = all_spheres[j].xPos - all_spheres[i].xPos;
                all_spheres[j].delta_y = all_spheres[j].yPos - all_spheres[i].yPos;
				all_spheres[i].path = 0;
                normalize(all_spheres[j]);
				all_spheres[j].speed = 0.1;
				
				//}else{
				
					//// collision occurs
					//current = (double) clock();
					//
					//all_spheres[i].interval = 0.0;
					//all_spheres[i].start_time = current;	
					//
					//all_spheres[i].x1 = all_spheres[i].xPos +
					//		(all_spheres[i].xPos - all_spheres[j].xPos)*0.000000001;
					//all_spheres[i].x2 = (all_spheres[i].xPos - all_spheres[j].xPos);
					//all_spheres[i].x3 = 2*all_spheres[i].x1;
					//all_spheres[i].x4 = 3*all_spheres[i].x1;
					//
					//all_spheres[i].y1 = all_spheres[i].yPos +
					//		(all_spheres[i].yPos - all_spheres[j].yPos)*0.1;
					//all_spheres[i].y2 = (all_spheres[i].yPos - all_spheres[j].yPos);
					//all_spheres[i].y3 = 2*all_spheres[i].y1;
					//all_spheres[i].y4 = 3*all_spheres[i].y1;
					//all_spheres[i].curve_length = curve_length(
					//								all_spheres[i].x1,
					//								all_spheres[i].x2,
					//								all_spheres[i].x3,
					//								all_spheres[i].x4,
					//								all_spheres[i].y1,
					//								all_spheres[i].y2,
					//								all_spheres[i].y3,
					//								all_spheres[i].y4
					//		);
					//
					//all_spheres[i].curve_time = all_spheres[i].curve_length / 
					//							all_spheres[i].speed;

					//all_spheres[j].interval = 0.0;
					//all_spheres[j].start_time = current;	
					//
					//all_spheres[j].x1 = all_spheres[j].xPos + 
					//		(all_spheres[j].xPos - all_spheres[i].xPos)*0.000000001;
					//all_spheres[j].x2 = (all_spheres[j].xPos - all_spheres[i].xPos);
					//all_spheres[j].x3 = 2*all_spheres[j].x1;
					//all_spheres[j].x4 = 3*all_spheres[j].x1;
					//
					//all_spheres[j].y1 = all_spheres[j].yPos + 
					//		(all_spheres[j].yPos - all_spheres[i].yPos)*0.000000001;
					//all_spheres[j].y2 = (all_spheres[j].yPos - all_spheres[i].yPos);
					//all_spheres[j].y3 = 2*all_spheres[j].y1;
					//all_spheres[j].y4 = 3*all_spheres[j].y1;
					//all_spheres[j].curve_length = curve_length(
					//								all_spheres[j].x1,
					//								all_spheres[j].x2,
					//								all_spheres[j].x3,
					//								all_spheres[j].x4,
					//								all_spheres[j].y1,
					//								all_spheres[j].y2,
					//								all_spheres[j].y3,
					//								all_spheres[j].y4
					//		);
					//
					//all_spheres[j].curve_time = all_spheres[j].curve_length / 
					//							all_spheres[j].speed;

				//}

			//	}
			}
		}
	}
}

/*
 * void gfxinit();
 *
 * initializes the system prior to animating
 */
void gfxinit() {
	start = (double) clock(); 
	current = 0.0;
	// LIGHTING 	
    GLfloat lightpos[4] = { 1.0, 0.0, 1.0, 1.0 };     // light position
    GLfloat lightamb[4] = { 0.0, 0.0, 0.0, 1.0 };     // ambient colour
    GLfloat lightdif[4] = { 1.0, 1.0, 1.0, 1.0 };     // diffuse colour
    GLfloat global_ambient[4] = {0.2, 0.2, 0.2, 1};

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
    glOrtho(-5.0,5.0,5.0,-5.0,1.0,20.0);
	glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glEnable ( GL_COLOR_MATERIAL ) ;
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glShadeModel(GL_SMOOTH);

	srand(time(NULL));	// seed for rand() calls

	int k;
	for( k = 0; k < NUMBER_OF_BALLS; k++ ) { //setup all ball settings
		
		all_spheres[k].x1 = new_random_value();
		all_spheres[k].x2 = new_random_value();
		all_spheres[k].x3 = new_random_value();
		all_spheres[k].x4 = new_random_value();

		all_spheres[k].y1 = new_random_value();
		all_spheres[k].y2 = new_random_value();
		all_spheres[k].y3 = new_random_value();
		all_spheres[k].y4 = new_random_value();
		
		all_spheres[k].size = BALL_SIZE;

		// TODO random ball speed
		all_spheres[k].speed = BALL_SPEED;	
		
		// TODO random ball mass for momentum

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
		
		// start on a curved path
		all_spheres[k].path = 1;	
		
		all_spheres[k].start_time = (double) clock();
		all_spheres[k].curve_time = all_spheres[k].curve_length / 
									all_spheres[k].speed;

		// set ball colors
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
	}
}

/*
 * void display();
 *
 * The display function displays the animation to the users screen.
 */
void display() {
	glClearColor(0.8,0.8,0.8,1.0);	// set the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	int i;
	for(i = 0; i < NUMBER_OF_BALLS; i++) {	// draw all spheres
		glPushMatrix();
		glColor3f(all_spheres[i].red,all_spheres[i].green,all_spheres[i].blue);
		glTranslatef(all_spheres[i].xPos,all_spheres[i].yPos,0);
		glutSolidSphere(all_spheres[i].size,25,25);
		glPopMatrix();
	}
	collision_check();	// check for collisions wall-ball and ball-ball
	glutSwapBuffers();
}


/*
 * void keystroke(unisgned char c, int x, int y);
 *
 * The keystroke function handles user input to modify how the animation
 * performs. Also allows the user to quit
 */
void keystroke(unsigned char c, int x, int y) {
	switch(c) {
		case 113:		// [q] is quit
			exit(0);
			break;
	}
}

/*
 * void main(int argc, char **argv);
 *
 * Simply sets up the OpenGL framework, calls functions to set up and begin
 * the animation.
 */
int main(int argc, char **argv) {
     glutInit(&argc,argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
     window = glutCreateWindow("Sphere Collisions"); //window title
     glutDisplayFunc(display);
     glutIdleFunc(animate);	// call animate() when idle
     glutKeyboardFunc(keystroke);	//handles user input
	 gfxinit();
     glutMainLoop(); // start the animation
}
