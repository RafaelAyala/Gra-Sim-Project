/*
 * Rafael Ayala
 * 100XXXXXX
 *
 * Jordan Stadler
 * 100366066
 *
 * Final Project
 *
 * TODO decay
 * TODO mass for momentum
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
#define NUMBER_OF_BALLS 20
//#define INTERVAL 0.0049999
#define BALL_RADIUS 0.1
#define BALL_SPEED 12.0 // ASU's per second
#define CURVE_LENGTH_APPROX 8
#define RAINBOW 1 // 1 = multi colored spheres, 0 = red
#define DENSITY 1.0
#define PI 3.14159

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// global variables
int window; 	//id of the window
static double s = 0.5; // tightness of the paths (0.0 - tight, 0.5 - loose)
static double start;
double current;
double temp;

// holds 2 floating point number representing a point in 2 space
struct point2f {
	double x;
	double y;
};

// holds 3 floating point numbers representing a color (RGB)
struct color3f {
	double red;
	double green;
	double blue;
};

// TODO clean sphere struct
// holds all information pertaining to a single sphere
struct sphere{
	double interval;
	struct point2f pos;
	struct point2f p1,p2,p3,p4;
	double radius;
	struct color3f color;
	double speed;
	double curve_length;
	double start_time;
	double curve_time;
	int path;  // flag: 0 is linear path, 1 is a bezier curve
	double delta_x, delta_y;
	double mass;
} all_spheres[NUMBER_OF_BALLS];

// TODO create all random functions
double new_random_value() {
	return ((rand() % (101)/100.)*10)-5.0;	
}

/*
 * struct color3f random_color();
 *
 * returns a randomized RGB color w/o dark colors
 */
struct color3f random_color(){
	struct color3f color;
	double red, green, blue;
	do{
		red = (rand()%(10001))/10000.;
		if(red < 0.5) red = 0.0;
		green = (rand()%(10001))/10000.;
		if(green < 0.5) green = 0.0;
		blue = (rand()%(10001))/10000.;
		if(blue < 0.5) blue = 0.0;
	}while( red == 0.0 && green == 0.0 && blue == 0.0); // no black balls
	color.red = red;  color.green = green;  color.blue = blue;
	return color;
}

/*
 * double random_radius();
 *
 * returns a random radius between 0.15 and 0.7
 */
double random_radius() {
	double radius;
	do{
		radius = (rand() % 10001)/10000.;
	}while(radius > 0.7 || radius < 0.15);
	return radius;
}

/*
 * double get_mass(struct sphere ball);
 *
 * returns the mass of the ball based on the radius
 * Note: all balls have the same density
 * volume = (4*Pi * r^3) / 3
 */
double get_mass(struct sphere ball) {
	double volume = (4 * PI * pow(ball.radius,3))/3.;
	return volume * DENSITY;
}

/*
 * struct point2f get_position(struct sphere ball, double pos);
 *
 * returns the position of the ball on a curve
 */
struct point2f get_position( struct sphere ball, double pos) {
	double a, b, c;
	struct point2f result;
	// x
	c = 3 * (ball.p2.x - ball.p1.x);
	b = 3 * (ball.p3.x - ball.p2.x) - c;
	a = ball.p4.x - ball.p1.x - c - b;
	result.x = a * pow(pos,3) + b * pow(pos,2) + c * pos + ball.p1.x;
	
	// y
	c = 3 * (ball.p2.y - ball.p1.y);
	b = 3 * (ball.p3.y - ball.p2.y) - c;
	a = ball.p4.y - ball.p1.y - c - b;
	result.y = a * pow(pos,3) + b * pow(pos,2) + c * pos + ball.p1.y;
	
	return result;
}

/*
 * double curve_length(struct sphere ball);
 *
 * returns a double value representing the approximate length of the bezier
 * curve
 */
double curve_length( struct sphere ball ) {
	double i;
	double x1, y1, x2, y2;
	double total = 0;
	struct point2f p1, p2;
	p1 = get_position( ball, 0);
	for(i = 1./CURVE_LENGTH_APPROX ; i <= 1; i+= 1./CURVE_LENGTH_APPROX) {
		p2 = get_position( ball, i);
		total = total + sqrt( pow(p2.x-p1.x,2) + pow(p2.y-p1.y,2) );
		p1 = p2;
	}
	return total;
}

/*
 * void normalize(struct sphere ball);
 *
 * normalizes the delta_x and delta_y components of a sphere
 */
void normalize(struct sphere ball) {
	double mag;
	mag = sqrt(pow(ball.delta_x,2) + pow(ball.delta_y,2));
	ball.delta_x /= mag;
	ball.delta_y /= mag;
}

/*
 * double distance(struct sphere ball1, struct sphere ball2);
 *
 * returns the distance between two parameter spheres
 */
double distance( struct sphere ball1, struct sphere ball2 ) {
	return sqrt( pow(ball1.pos.x - ball2.pos.x,2) + pow(ball1.pos.y - ball2.pos.y,2));
}

/*
 * void animate();
 *
 * This acts as the idle function, whenever the system is idle, animte() is
 * called.  The end of animate() forces display() to refresh
 * TODO clean animate
 */
void animate() {
	int j;
	double a, b, c;

	while((double) clock() == current){} // waits for next time step
	current = (double) clock();
	
	double tempX, tempY;

	for(j = 0; j < NUMBER_OF_BALLS; j++) {
		
		
		if(all_spheres[j].path == 0) { //linear paths
			// TODO
			all_spheres[j].pos.x += all_spheres[j].delta_x / BALL_SPEED;
			all_spheres[j].pos.y += all_spheres[j].delta_y / BALL_SPEED;



		} else if(all_spheres[j].path == 1) { // bezier curves for path	
			if( all_spheres[j].interval < 1.0) {
			
				tempX = all_spheres[j].pos.x;
				tempY = all_spheres[j].pos.y;
				all_spheres[j].interval = (current - all_spheres[j].start_time)/ 
											(CLOCKS_PER_SEC * all_spheres[j].curve_time);
				all_spheres[j].pos = get_position( all_spheres[j], all_spheres[j].interval );
			} else {
				all_spheres[j].interval = 0.0;
				all_spheres[j].p2.x = (all_spheres[j].p4.x - all_spheres[j].p3.x) + 
					all_spheres[j].p4.x;
				all_spheres[j].p1.x = all_spheres[j].p4.x;
				all_spheres[j].p3.x = new_random_value();
				all_spheres[j].p4.x = new_random_value();
				
				all_spheres[j].p2.y = (all_spheres[j].p4.y - all_spheres[j].p3.y) + 
					all_spheres[j].p4.y;
				all_spheres[j].p1.y = all_spheres[j].p4.y;
				all_spheres[j].p3.y = new_random_value();
				all_spheres[j].p4.y = new_random_value();
				all_spheres[j].curve_length = curve_length( all_spheres[j] );
				all_spheres[j].start_time = (double) clock();
				all_spheres[j].curve_time = all_spheres[j].curve_length / 
											all_spheres[j].speed;
			}
		}	
	
	}

	// set window and call display to refresh screen
	glutSetWindow(window);
	glutPostRedisplay();
}

/*
 * void collision_check();
 *
 * determines if any ball-wall or ball-ball collisions occur
 * TODO speeds are hard coded,
 * TODO tangents need to be used
 * TODO deltas need real values
 */
void collision_check() {
	int i, j;
	double d;
	
	for(i = 0; i < NUMBER_OF_BALLS; i++) {
		
		// ball-wall collisions
		double dist = 5.0 - all_spheres[i].radius;
		//right
		if(all_spheres[i].pos.x >= dist ) {
			all_spheres[i].delta_x *= -1;
			all_spheres[i].pos.x = dist;
			all_spheres[i].path = 0;
			//printf("right\n");
			//glutIdleFunc(NULL);
		//left
		} else if(all_spheres[i].pos.x <= -1*dist){
			all_spheres[i].delta_x *= -1;
			all_spheres[i].pos.x = -1*dist;
			all_spheres[i].path = 0;
			//printf("left\n");
			//glutIdleFunc(NULL);
		//bottom
		} else if(all_spheres[i].pos.y >= dist ) {
			all_spheres[i].delta_y *= -1;
			all_spheres[i].pos.y = dist;
			all_spheres[i].path = 0;
			//printf("bottom\n");
			//glutIdleFunc(NULL);
		//top
		}else if(all_spheres[i].pos.y <= -1*dist){
			all_spheres[i].delta_y *= -1;
			all_spheres[i].pos.y = -1*dist;
			all_spheres[i].path = 0;
			//printf("top\n");
			//glutIdleFunc(NULL);
		}
		// end ball-wall collisions	

		// ball-ball collisions
		for( j=0; j < NUMBER_OF_BALLS; j++) {
			// check all but itself
			if( i == j) { continue; }
					
			d = distance(all_spheres[i], all_spheres[j]);

			if( d <= all_spheres[i].radius + all_spheres[j].radius) {
				//printf("ball-ball\n");
				//printf("ball i: %f %f\n", all_spheres[i].pos.x, all_spheres[i].pos.y);
				//printf("ball j: %f %f\n", all_spheres[j].pos.x, all_spheres[j].pos.y);
				//glutIdleFunc(NULL);
					
				
				all_spheres[i].delta_x = all_spheres[i].pos.x - all_spheres[j].pos.x;
				all_spheres[i].delta_y = all_spheres[i].pos.y - all_spheres[j].pos.y;
				all_spheres[i].path = 0;
				normalize(all_spheres[i]);
     			all_spheres[i].speed = 0.1;

				all_spheres[j].delta_x = all_spheres[j].pos.x - all_spheres[i].pos.x;
                all_spheres[j].delta_y = all_spheres[j].pos.y - all_spheres[i].pos.y;
				all_spheres[i].path = 0;
                normalize(all_spheres[j]);
				all_spheres[j].speed = 0.1;	
			}
		}
		// end ball-ball
	}
}

struct sphere make_sphere(){
	struct sphere ball;
	ball.p1.x = new_random_value();
	ball.p2.x = new_random_value();
	ball.p3.x = new_random_value();
	ball.p4.x = new_random_value();

	ball.p1.y = new_random_value();
	ball.p2.y = new_random_value();
	ball.p3.y = new_random_value();
	ball.p4.y = new_random_value();
	
	ball.radius = random_radius();

	// TODO random ball speed
	ball.speed = BALL_SPEED;	
	
	// TODO random ball mass for momentum

	ball.curve_length = curve_length( ball );
	
	// start on a curved path
	ball.path = 1;	
	
	ball.start_time = (double) clock();
	ball.curve_time = ball.curve_length / ball.speed;

	ball.delta_x = 0.3;
	ball.delta_y = 0.3;
	
	ball.color = random_color();		
	return ball;
}

/*
 * void gfxinit();
 *
 * initializes the system prior to animating
 * TODO refactor(make a ball generation function)
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
		all_spheres[k] = make_sphere();	
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
		glColor3f(all_spheres[i].color.red,
				all_spheres[i].color.green,
				all_spheres[i].color.blue);
		//glColor3f(all_spheres[i].red,all_spheres[i].green,all_spheres[i].blue);
		glTranslatef(all_spheres[i].pos.x,all_spheres[i].pos.y,0);
		glutSolidSphere(all_spheres[i].radius,25,25);
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
		case 110:
			glutIdleFunc(animate);
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
