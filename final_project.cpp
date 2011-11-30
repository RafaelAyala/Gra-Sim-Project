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
#define DECAY_PROB 0.5
#define BALL_RADIUS 0.1
#define BALL_SPEED 2.0 // ASU's per second
#define CURVE_LENGTH_APPROX 16
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

struct vector2f {
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
	double velocity;
	double curve_length;
	double start_time;
	double curve_time;
	int path;  // flag: 0 is linear path, 1 is a bezier curve
	struct vector2f direction;
	double mass;
	int dead;
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
double normalize(struct vector2f dir) {
	return sqrt(pow(dir.x,2) + pow(dir.y,2));
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
		
		double p = (rand() % 101)/100.;

		if( all_spheres[j].radius>0.0) {
			if( p < DECAY_PROB) {
				all_spheres[j].radius -= 0.00045;
			}
		}else{
			continue;
		}

		if(all_spheres[j].path == 0) { //linear paths
			// TODO
			double timediff = (current - all_spheres[j].start_time)/(CLOCKS_PER_SEC);

			all_spheres[j].pos.x += all_spheres[j].direction.x * all_spheres[j].velocity * timediff;
			all_spheres[j].pos.y += all_spheres[j].direction.y * all_spheres[j].velocity * timediff;
			all_spheres[j].start_time = (double) clock();


		} else if(all_spheres[j].path == 1) { // bezier curves for path	
			if( all_spheres[j].interval < 1.0) {

				tempX = all_spheres[j].pos.x;
				tempY = all_spheres[j].pos.y;
				//printf("%f %f\n", tempX, tempY);
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
											all_spheres[j].velocity;
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
 * TODO velocitys are hard coded,
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
			all_spheres[i].direction.x *= -1;
			all_spheres[i].pos.x = dist;
			all_spheres[i].path = 0;
			//printf("right\n");
			//glutIdleFunc(NULL);
			all_spheres[i].start_time = (double) clock();

		//left
		} else if(all_spheres[i].pos.x <= -1*dist){
			all_spheres[i].direction.x *= -1;
			all_spheres[i].pos.x = -1*dist;
			all_spheres[i].path = 0;
			//printf("left\n");
			//glutIdleFunc(NULL);
			all_spheres[i].start_time = (double) clock();
		//bottom
		} else if(all_spheres[i].pos.y >= dist ) {
			all_spheres[i].direction.y *= -1;
			all_spheres[i].pos.y = dist;
			all_spheres[i].path = 0;
			//printf("bottom\n");
			//glutIdleFunc(NULL);
			all_spheres[i].start_time = (double) clock();
		//top
		}else if(all_spheres[i].pos.y <= -1*dist){
			all_spheres[i].direction.y *= -1;
			all_spheres[i].pos.y = -1*dist;
			all_spheres[i].path = 0;
			//printf("top\n");
			//glutIdleFunc(NULL);
			all_spheres[i].start_time = (double) clock();
		}
		// end ball-wall collisions	

		// ball-ball collisions
		for( j=0; j < NUMBER_OF_BALLS; j++) {
			// check all but itself
			if( i == j) { continue;}

			if(all_spheres[j].dead > 0) {
				all_spheres[j].dead--;
				continue;
			}



			d = distance(all_spheres[i], all_spheres[j]);

			if( d <= all_spheres[i].radius + all_spheres[j].radius) {
				//printf("ball-ball\n");
				//printf("ball i: %f %f\n", all_spheres[i].pos.x, all_spheres[i].pos.y);
				//printf("ball j: %f %f\n", all_spheres[j].pos.x, all_spheres[j].pos.y);
				//glutIdleFunc(NULL);

				// store before collision velocities
				double b1_v, b2_v;
				b1_v = all_spheres[i].velocity;
				b2_v = all_spheres[j].velocity;
				// have before collision velocities stored	
				printf("###COLLISION### %d vs %d\n", i, j);
				printf("b1 v before: %f\n", all_spheres[i].velocity);
				printf("b2 v before: %f\n", b2_v);
				printf("b1 direction: %f %f\n", all_spheres[i].direction.x,all_spheres[i].direction.y);
				printf("b2 direction: %f %f\n", all_spheres[j].direction.x,all_spheres[j].direction.y);
				// need x and y components of speed
				double b1_vx, b1_vy, b2_vx, b2_vy;
				b1_vx = (all_spheres[i].direction.x * b1_v);
				b1_vy = (all_spheres[i].direction.y * b1_v);

				b2_vx = (all_spheres[j].direction.x * b2_v);
				b2_vy = (all_spheres[j].direction.y * b2_v);
				// have x and y components of speed

				printf("b1 components: %f %f\n", b1_vx, b1_vy);
				printf("b2 components: %f %f\n", b2_vx, b2_vy);


				// need ball1 and ball2 masses
				all_spheres[i].mass = get_mass(all_spheres[i]);	
				all_spheres[j].mass = get_mass(all_spheres[j]);	
				double m1, m2;
				m1 = all_spheres[i].mass;
				m2 = all_spheres[j].mass;
				// have ball masses
				printf("mass1 %f\n", m1);
				printf("mass2 %f\n", m2);

				// need the new velocity components (after collision)
				double b1_vx_new, b1_vy_new, b2_vx_new, b2_vy_new;

				// ball 1 new components
				b1_vx_new = ( (m1-m2) * b1_vx + (2*m2) * b2_vx ) / (m1+m2);
				b1_vy_new = ( (m1-m2) * b1_vy + (2*m2) * b2_vy ) / (m1+m2);

				// ball 2 new components
				b2_vx_new = ( (m2-m1) * b2_vx + (2*m1) * b1_vx ) / (m1+m2);
				b2_vy_new = ( (m2-m1) * b2_vy + (2*m1) * b1_vy ) / (m1+m2);
				// have new velocity compoents
				printf("b1 newx: %f\n", b1_vx_new);
				printf("b1 newy: %f\n", b1_vy_new);
				printf("b2 newx: %f\n", b2_vx_new);
				printf("b2 newy: %f\n", b2_vy_new);

				// need to change direction to match new speeds
				all_spheres[i].direction.x = b1_vx_new;
				all_spheres[i].direction.y = b1_vy_new;
				double mag1 = normalize(all_spheres[i].direction);
				all_spheres[i].direction.x /= mag1;
				all_spheres[i].direction.y /= mag1;
				printf("ball1 dir: %f\n", all_spheres[i].direction.x);
				printf("ball1 dir: %f\n", all_spheres[i].direction.y);


				all_spheres[j].direction.x = b2_vx_new;
				all_spheres[j].direction.y = b2_vy_new;
				double mag2 = normalize(all_spheres[j].direction);
				all_spheres[j].direction.x /= mag2;
				all_spheres[j].direction.y /= mag2;
				printf("ball2 dir: %f\n", all_spheres[j].direction.x);
				printf("ball2 dir: %f\n", all_spheres[j].direction.y);
				// directions changed, normalized for unit vectors

				// need to update velocities of ball1 and ball2
				all_spheres[i].velocity = sqrt( pow(b1_vx_new,2) + pow(b1_vy_new,2));
				all_spheres[j].velocity = sqrt( pow(b2_vx_new,2) + pow(b2_vy_new,2));
				// speeds updated

				// TODO change direction here

				all_spheres[i].path = 0;
				all_spheres[j].path = 0;
				all_spheres[i].start_time = (double) clock();
				all_spheres[j].start_time = (double) clock();

				all_spheres[i].dead = 200;
				all_spheres[j].dead = 200;

				printf("after\n");
				printf("ball i: %f\n", all_spheres[i].velocity);
				printf("ball j: %f\n", all_spheres[j].velocity);
			}
		}
		// end ball-ball
	}
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
	all_spheres[k].p1.x = new_random_value();
	all_spheres[k].p2.x = new_random_value();
	all_spheres[k].p3.x = new_random_value();
	all_spheres[k].p4.x = new_random_value();

	all_spheres[k].p1.y = new_random_value();
	all_spheres[k].p2.y = new_random_value();
	all_spheres[k].p3.y = new_random_value();
	all_spheres[k].p4.y = new_random_value();

	all_spheres[k].radius = random_radius();

	// TODO random ball velocity
	all_spheres[k].velocity = BALL_SPEED;	

	// TODO random ball mass for momentum

	all_spheres[k].curve_length = curve_length( all_spheres[k] );

	// start on a curved path
	all_spheres[k].path = 1;	

	all_spheres[k].start_time = (double) clock();
	all_spheres[k].curve_time = all_spheres[k].curve_length / all_spheres[k].velocity;

	all_spheres[k].direction.x = new_random_value();
	all_spheres[k].direction.y = new_random_value();
	double mag = normalize( all_spheres[k].direction);
	all_spheres[k].direction.x /= mag;
	all_spheres[k].direction.y /= mag;

	all_spheres[k].color = random_color();		
	all_spheres[k].dead =0;
		//all_spheres[k] = make_sphere();	
	}
}

/*
 * void display();
 *
 * The display function displays the animation to the users screen.
 */
void display() {
	//printf("%f\n", (double) clock());
	glClearColor(0.0,0.0,0.0,1.0);	// set the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.5, 0.5, 0.5);
	// back box
	glBegin(GL_QUADS);
	  glVertex3f(-5, -5, -5);
	  glVertex3f( 5, -5, -5);
	  glVertex3f( 5,  5, -5);
	  glVertex3f(-5,  5, -5);
	glEnd();

	glColor3f(0.4, 0.4, 0.4);
	// left side
	glBegin(GL_QUADS);
	  glVertex3f(-5, -5, -5);
	  glVertex3f(-5, -5,  5);
	  glVertex3f(-5,  5,  5);
	  glVertex3f(-5,  5, -5);
	glEnd();

	// right side
	glBegin(GL_QUADS);
	  glVertex3f( 5, -5, -5);
	  glVertex3f( 5, -5,  5);
	  glVertex3f( 5,  5,  5);
	  glVertex3f( 5,  5, -5);
	glEnd();

	glColor3f(0.6, 0.6, 0.6);
	// bottom
	glBegin(GL_QUADS);
	  glVertex3f(-5,  5, -5);
	  glVertex3f(-5,  5,  5);
	  glVertex3f( 5,  5,  5);
	  glVertex3f( 5,  5, -5);
	glEnd();

	// top
	glBegin(GL_QUADS);
	  glVertex3f(-5, -5, -5);
	  glVertex3f(-5, -5,  5);
	  glVertex3f( 5, -5,  5);
	  glVertex3f( 5, -5, -5);
	glEnd();
	
	collision_check();	// check for collisions wall-ball and ball-ball
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

