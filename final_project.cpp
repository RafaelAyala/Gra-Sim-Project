/*
 * Rafael Ayala
 *
 * Jordan Stadler
 *
 * Final Project
 *
 * TODO decay on collide
 * remove dead variable
 */

#ifdef __APPLE__  	// Mac OpenGL Libraries
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else				// Linux OpenGL Libraries
#include <GL/freeglut.h>
//#include <GL/freeglut_std.h>
//#include <GL/freeglut_ext.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
//#include <GL/glui.h>
#endif
// Common Libraries
#include <stdio.h> 
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
// configuration
// TODO clean defines
#define NUMBER_OF_BALLS 2
#define DECAY_PROB 0.5
//#define BALL_RADIUS 0.1
#define BALL_SPEED 2.0 // ASU's per second
#define CURVE_LENGTH_APPROX 16
//#define RAINBOW 1 // 1 = multi colored spheres, 0 = red
#define DENSITY 1.0
#define PI 3.14159

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// global variables
int window; 	//id of the window
static double s = 0.5; // tightness of the paths (0.0 - tight, 0.5 - loose)
static double start;
double current;
double temp;
int balls;

// holds 2 floating point number representing a point in 2 space
struct point2f {
	double x;
	double y;
};

// hold two floating point number representing a direction
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
	struct point2f previous_pos;
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
	//int dead;
	int active;
};

std::vector<sphere> all_spheres;

/*
 * double ranged_random_value();
 *
 * returns a random value based on the the bounds of the system
 */ 
double ranged_random_value() {
	return ((rand() % (101)/100.)*9)-4.5;	
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
struct point2f get_position( struct sphere *ball, double pos) {
	double a, b, c;
	struct point2f result;
	// x
	c = 3 * (ball->p2.x - ball->p1.x);
	b = 3 * (ball->p3.x - ball->p2.x) - c;
	a = ball->p4.x - ball->p1.x - c - b;
	result.x = a * pow(pos,3) + b * pow(pos,2) + c * pos + ball->p1.x;

	// y
	c = 3 * (ball->p2.y - ball->p1.y);
	b = 3 * (ball->p3.y - ball->p2.y) - c;
	a = ball->p4.y - ball->p1.y - c - b;
	result.y = a * pow(pos,3) + b * pow(pos,2) + c * pos + ball->p1.y;

	return result;
}

/*
 * double curve_length(struct sphere ball);
 *
 * returns a double value representing the approximate length of the bezier
 * curve
 */
double curve_length( struct sphere *ball ) {
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
double distance( struct sphere b1, struct sphere b2 ) {
	return sqrt( pow(b1.pos.x - b2.pos.x,2) + pow(b1.pos.y - b2.pos.y,2));
}

/*
 * struct sphere move_on_vector( struct sphere ball);
 *
 * advances the sphere along the vector path
 */
struct sphere move_on_vector( struct sphere ball ) {
	double timediff = ( current - ball.start_time ) / ( CLOCKS_PER_SEC );

	//store previous positions
	ball.previous_pos = ball.pos;
	
	// update position
	ball.pos.x += ball.direction.x * ball.velocity * timediff;
	ball.pos.y += ball.direction.y * ball.velocity * timediff;
	ball.start_time = (double) clock();
	return ball;
}

/*
 * struct sphere move_on_curve( struct sphere ball);
 *
 * advances the sphere along the curved path
 */
//struct sphere move_on_curve( struct sphere *ball ) {
void move_on_curve( struct sphere *ball ) {
	// store previous position
	ball->previous_pos = ball->pos;
	ball->interval =( current - ball->start_time )/
		( CLOCKS_PER_SEC * ball->curve_time );
	ball->pos = get_position( ball, ball->interval );
	
	//return ball;
}

/*
 * void generate_curve( struct sphere ball);
 *
 * generates a new bezier curve based on a previous one
 */
void generate_curve( struct sphere *ball) {
	// store previous position
	ball->previous_pos = ball->pos;
	
	ball->interval = 0.0;
	ball->p2.x = ( ball->p4.x - ball->p3.x ) + ball->p4.x;
	ball->p1.x = ball->p4.x;
	ball->p3.x = ranged_random_value();
	ball->p4.x = ranged_random_value();

	ball->p2.y = ( ball->p4.y - ball->p3.y ) + ball->p4.y;
	ball->p1.y = ball->p4.y;
	ball->p3.y = ranged_random_value();
	ball->p4.y = ranged_random_value();
	ball->curve_length = curve_length( ball );
	ball->start_time = (double) clock();
	ball->curve_time = ball->curve_length / ball->velocity;
	//return ball;
}

/*
 * void animate();
 *
 * This acts as the idle function, whenever the system is idle, animte() is
 * called.  The end of animate() forces display() to refresh
 */
void animate() {
	int j;
	
	while((double) clock() == current){} // waits for next time step
	current = (double) clock();

	for(j = 0; j < all_spheres.size(); j++) {
		
		// DECAY
		double decay = (rand() % 101)/100.;

		if( all_spheres[j].radius>0.0) {
			if( decay <= DECAY_PROB && all_spheres[j].active) {
				all_spheres[j].radius -= 0.00045;
			}
		}else{
			all_spheres.erase(all_spheres.begin()+j);
			//printf("spheres left: %d\n", all_spheres.size());
			balls--;
			break;
		}
		// END DECAY

		if(all_spheres[j].path == 0) { //linear paths
			// advance position on vector
			all_spheres[j] = move_on_vector(all_spheres[j]);
		
		}else if(all_spheres[j].path == 1) { // bezier curves for path	
			
			if( all_spheres[j].interval < 1.0) {
				// advance position on curve
				//all_spheres[j] = move_on_curve(all_spheres[j]);
				move_on_curve(&all_spheres[j]);
			} else { 
				// generate a new curve
				generate_curve(&all_spheres[j]);		
			}
		}	
	}
	// set window and call display to refresh screen
	glutSetWindow(window);
	glutPostRedisplay();
}

/*
 * int collision_detection(struct sphere ball);
 *
 * returns 1 when a collision is detected with ball
 * returns 0 when no collisions detected with ball
 */
int collision_detection( struct sphere ball ) {
	int i, count;
	double d;
	count = 0;
	for( i = 0; i < all_spheres.size(); i++ ) {
		d = distance(ball, all_spheres[i]);
		
		if( d <= ball.radius + all_spheres[i].radius + 0.0001) {
			count++;
		}
	}
	return (count>0) ? 1 : 0;
}

/*
 * void wall_check( struct sphere ball );
 *
 * checks for a ball to wall collision
 */ 
//struct sphere wall_check( struct sphere ball ) {
void wall_check( struct sphere *ball ) {
		
		//return ball;
		double dist = 5.0 - ball->radius;
		if(ball->path == 1) {
			ball->direction.x = ball->pos.x - ball->previous_pos.x;
			ball->direction.y = ball->pos.y - ball->previous_pos.y;
			double mag1 = normalize(ball->direction);
			ball->direction.x /= mag1;
			ball->direction.y /= mag1;
		}
		if( ball->pos.x >= dist || ball->pos.x <= -1*dist) {
			ball->direction.x *= -1;
			ball->pos.x = ( ball->pos.x < 0.0  ) ? -1*dist : dist;
			ball->path = 0;
			ball->start_time = (double) clock();
			ball->active = 1;
		} else if(ball->pos.y >= dist || ball->pos.y <= -1*dist) {
			ball->direction.y *= -1;
			ball->pos.y = ( ball->pos.y < 0.0 ) ? -1*dist : dist;
			ball->path = 0;
			ball->start_time = (double) clock();
			ball->active = 1;
		}
}

/*
 * void nudge_spheres( struct sphere *b1, struct sphere b2, double d);
 *
 * nudges sphere so they are at the point of collision, uses d to determine
 * the amount of peneatation and corrects it.
 */
void nudge_spheres( struct sphere *b1, struct sphere *b2, double d) {
	double penetration = (b1->radius + b2->radius) - d;
	double ipen, jpen, ij;
	ij = b1->velocity + b2->velocity;
	ipen = b1->velocity / ij; // 0-1
	jpen = b2->velocity / ij; // 0-1
	ipen *= penetration;
	jpen *= penetration;
	
	double i_dir_mag = normalize(b1->direction);
	
	b1->pos.x -= (ipen * b1->direction.x ) / i_dir_mag;
	b1->pos.y -= (ipen * b1->direction.y ) /i_dir_mag;
	
	double j_dir_mag = normalize(b2->direction);
	b2->pos.x -= (jpen * b2->direction.x ) / j_dir_mag;
	b2->pos.y -= (jpen * b2->direction.y ) /j_dir_mag;	

}

/*
 * void update_direction( struct sphere *ball);
 *
 * Generates a direction based on the previous direction. Used when a sphere
 * travelling on a curve path, become a straight path.
 */
void update_direction( struct sphere *ball ) {
	ball->direction.x = ball->pos.x - ball->previous_pos.x;
	ball->direction.y = ball->pos.y - ball->previous_pos.y;
	double mag1 = normalize(ball->direction);
	ball->direction.x /= mag1;
	ball->direction.y /= mag1;
}

/*
 * void collision_response( struct sphere *b1, struct sphere *b2);
 *
 * performs collision response between two balls
 */
void collision_response(struct sphere *b1, struct sphere *b2) {

	b1->active = 1;
	b2->active = 1;
	

	// store before collision velocities
	double b1_v, b2_v;
	b1_v = b1->velocity;
	b2_v = b2->velocity;
	double b1_vx, b1_vy, b2_vx, b2_vy;
	b1_vx = (b1->direction.x * b1_v);
	b1_vy = (b1->direction.y * b1_v);

	b2_vx = (b2->direction.x * b2_v);
	b2_vy = (b2->direction.y * b2_v);
	// have x and y components of speed

	// need ball1 and ball2 masses
	b1->mass = get_mass(*b1);	
	b2->mass = get_mass(*b2);	
	double m1, m2;
	m1 = b1->mass;
	m2 = b2->mass;
	// have ball masses

	// need the new velocity components (after collision)
	double b1_vx_new, b1_vy_new, b2_vx_new, b2_vy_new;

	// ball 1 new components
	b1_vx_new = ((m1-m2) * b1_vx + (2*m2) * b2_vx)/(m1+m2);
	b1_vy_new = ((m1-m2) * b1_vy + (2*m2) * b2_vy)/(m1+m2);

	// ball 2 new components
	b2_vx_new = ((m2-m1) * b2_vx + (2*m1) * b1_vx)/(m1+m2);
	b2_vy_new = ((m2-m1) * b2_vy + (2*m1) * b1_vy)/(m1+m2);

	// need to change direction to match new speeds
	b1->direction.x = b1_vx_new;
	b1->direction.y = b1_vy_new;
	double mag1 = normalize(b1->direction);
	b1->direction.x /= mag1;
	b1->direction.y /= mag1;


	b2->direction.x = b2_vx_new;
	b2->direction.y = b2_vy_new;
	double mag2 = normalize(b2->direction);
	b2->direction.x /= mag2;
	b2->direction.y /= mag2;
	b1->velocity = 
		sqrt( pow(b1_vx_new,2) + pow(b1_vy_new,2));
	b2->velocity = 
		sqrt( pow(b2_vx_new,2) + pow(b2_vy_new,2));
	// speeds updated

	b1->path = 0;
	b2->path = 0;
	b1->start_time = (double) clock();
	b2->start_time = (double) clock();

}

/*
 * void collision_check();
 *
 * determines if any ball-wall or ball-ball collisions occur
 */
void collision_check() {
	int i, j;
	double d;

	for(i = 0; i < all_spheres.size(); i++) {

		wall_check(&all_spheres[i]);
		
		// ball-ball collisions
		if( i < all_spheres.size()-1 ){
			for( j = i+1; j < all_spheres.size(); j++) {

				d = distance(all_spheres[i], all_spheres[j]);
				
				// if a collision
				if( d <= all_spheres[i].radius + all_spheres[j].radius) {
					
					// COLLISION RESPONSE STARTS
					
					// two curves colliding, need to generate direction vectors
					if(all_spheres[i].path + all_spheres[j].path == 2){
						{
							update_direction(&all_spheres[i]);						
							update_direction(&all_spheres[j]);						
						}	
					// one curve and 1 ball collide, need to generate vector
					}else if(all_spheres[i].path + all_spheres[j].path == 1){
						if(all_spheres[i].path == 1) { //[i] is the curved ball
							update_direction(&all_spheres[i]);							
						}else{ //[j] is the curved ball
							update_direction(&all_spheres[j]);							
						}
						// END BALL-CURVE
					}
						nudge_spheres(&all_spheres[i], &all_spheres[j], d);
						collision_response(&all_spheres[i], &all_spheres[j]);
				}
			}
		}
		// end ball-ball
	}
}

/*
 * struct point2f random_ranged_point();
 *
 * Returns a randomized 2D point within a bounded 2D area.
 */
struct point2f random_ranged_point() {
		return (struct point2f) {ranged_random_value(), ranged_random_value()};
}

/*
 * struct vector2f random_direction();
 *
 * Returns a random direction vector that has been normalized.
 */
struct vector2f random_direction(){
		struct vector2f direction;
		direction.x = ranged_random_value();
		direction.y = ranged_random_value();
		double mag = normalize(direction);
		direction.x /= mag;
		direction.y /= mag;
		return direction;
}

/*
 * double random_velocity();
 *
 * Returns a random speed between 0.0 and BALL_SPEED
 */
double random_velocity() {
	double velocity;
	velocity = ((rand() % 101)/100. * BALL_SPEED/2.) + (BALL_SPEED/2.);
	//printf("velocity %f\n", velocity);
	return velocity;
}

/*
 * struct sphere generate_sphere();
 * 
 * return a ball 
 */
struct sphere generate_sphere() {
		struct sphere ball;
		
		do{
			// gets 4 random points for the bezier curve
			ball.p1 = random_ranged_point();
			ball.p2 = random_ranged_point();
			ball.p3 = random_ranged_point();
			ball.p4 = random_ranged_point();
			
			// position starts at p1
			ball.pos = ball.p1;
			ball.previous_pos = {0.0, 0.0};
			
			// gets a random direction, this might be a wasted step
			ball.direction = random_direction();

			ball.radius = random_radius();
			ball.active = 0;

			ball.velocity = random_velocity();	

			// start on a curved path
			ball.path = 1;
			// dead variable is set to 0, used to prevent collisions
			//ball.dead = 0;
			ball.color = random_color();		

			ball.curve_length = curve_length( &ball );
			ball.start_time = (double) clock();
			ball.curve_time = ball.curve_length / ball.velocity;
		}while(collision_detection(ball) == 1 );	
		
		return ball;
}

/*
 * void print_sphere( struct sphere *ball);
 *
 * prints sphere info, used for debugging
 */
void print_sphere( struct sphere *ball) {
	printf("xpos: %f ypos: %f\n", ball->pos.x, ball->pos.y);
	printf("direction: %f %f\n", ball->direction.x, ball->direction.y);
	printf("prev:");
	//double interval;
	//struct point2f pos;
	//struct point2f previous_pos;
	//struct point2f p1,p2,p3,p4;
	//double radius;
	//struct color3f color;
	//double velocity;
	//double curve_length;
	//double start_time;
	//double curve_time;
	//int path;  // flag: 0 is linear path, 1 is a bezier curve
	//struct vector2f direction;
	//double mass;
	////int dead;
	//int active;
}

/*
 * void display();
 *
 * The display function displays the animation to the users screen.
 */
void display() {
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
	
	// MUST BE BEFORE SPHERES ARE DRAWN
	collision_check();
	
	int i;
	for(i = 0; i < all_spheres.size(); i++) {	// draw all spheres
		//printf("%f %f\n", all_spheres[i].pos.x, all_spheres[i].pos.y);
		glPushMatrix();
		glColor3f(all_spheres[i].color.red,
				all_spheres[i].color.green,
				all_spheres[i].color.blue);
		glTranslatef(all_spheres[i].pos.x,all_spheres[i].pos.y,0);
		if( all_spheres[i].active == 0) {
			glColor3f( 1.0, 1.0, 1.0);
			glutWireSphere(all_spheres[i].radius, 10, 10);
		}else{
			glutSolidSphere(all_spheres[i].radius, 25, 25);
		}
		glPopMatrix();
		if(isnan(all_spheres[i].pos.x) != 0 || isnan(all_spheres[i].pos.y) != 0) {
			print_sphere(&all_spheres[i]);
		}
	}
	//printf("spheres: %d\n", all_spheres.size());
	glutSwapBuffers();
	//glutIdleFunc(NULL);
}

/*
 * void gfxinit();
 *
 * initializes the system prior to animating
 */
void gfxinit() {
	balls = NUMBER_OF_BALLS;
	all_spheres.resize(balls);
	
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

	// create all spheres for the initial system state
	int k;
	for( k = 0; k < all_spheres.size(); k++ ) { //setup all ball settings
		//printf("%d\n",k);
		all_spheres[k] = generate_sphere();
	}
}



/*
 * void keystroke(unisgned char c, int x, int y);
 *
 * The keystroke function handles user input to modify how the animation
 * performs. Also allows the user to quit
 */
void keystroke(unsigned char c, int x, int y) {
	switch(c) {
		case 97:	// [a] for add ball
		{
		balls++;
		all_spheres.resize(balls);
		struct sphere ball;
		ball = generate_sphere();
		all_spheres.push_back(ball);
		}
			break;
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
