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
 #include <GL/glut.h>
 #include <GL/glu.h>
 #include <GL/gl.h>
 #include <GL/freeglut_ext.h>
#endif

// Common Libraries
#include <stdio.h> 
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
// user defined values
#define NUMBER_OF_BALLS 1
#define DECAY_PROB 0.5
#define BALL_SPEED 2.0 // ASU's per second
#define CURVE_LENGTH_APPROX 16
#define DENSITY 1.0
#define PI 3.14159
#define CUBE_LENGTH 5.0
#define RESPONSE 0
///Probabilities for the directions p3 and p4 will go in bezier curves
#define BACK 0.038461
#define BACK_N 0.038461
#define BACK_S 0.038461
#define BACK_E 0.038461
#define BACK_W 0.038461
#define BACK_NW 0.038461
#define BACK_SE 0.038461
#define BACK_SW 0.038461
#define BACK_NE 0.038461
#define MID_N 0.038461
#define MID_S 0.038461
#define MID_E 0.038461
#define MID_W 0.038461
#define MID_NW 	0.038461
#define MID_SE 	0.038461
#define MID_SW 	0.038461
#define MID_NE 	0.038461
#define FRONT 0.038461
#define FRONT_N 0.038461
#define FRONT_S 0.038461
#define FRONT_E 0.038461
#define FRONT_W 0.038461
#define FRONT_NW 0.038461
#define FRONT_SE 0.038461
#define FRONT_SW 0.038461
#define FRONT_NE 0.038461
#define STEP 1.0

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// global variables
int window; 	//id of the window
double s = 0.5; // tightness of the paths (0.0 - tight, 0.5 - loose)
double current;
double temp;
int balls;
double mass_of_system;
int response = RESPONSE;

double angleX = 0.0;
double angleY = 0.0;

double lx = -10.0, ly = -10.0, lz = -10.0;

double x = 10.0, y = 10.0, z = 10.0;

double deltaAngleX = 0.0;
double deltaAngleY = 0.0;
double deltaMove = 0.0;
int xOrigin = -1;
int yOrigin = -1;

int showText = 0; 

int dust_shown = 1;
int reflection = 1;

// holds 3 floating point number representing a point in 2 space
struct point3f {
	double x;
	double y;
	double z;
};

// hold two floating point number representing a direction
struct vector2f {
	double x;
	double y;
};

// holds three floating point number representing a direction
struct vector3f {
	double x;
	double y;
	double z;
};

// holds 3 floating point numbers representing a color (RGB)
struct color3f {
	double red;
	double green;
	double blue;
};

// holds all information pertaining to a single sphere
struct sphere{
	//struct point2f pos;
	struct point3f pos;
	double velocity;
	//struct vector2f direction;
	struct vector3f direction;
	//double mass;
	double radius;
	
	int path;  // flag: 0 is linear path, 1 is a bezier curve
	int active;
	struct color3f color;
	double start_time;
	int ghost;

	//struct point2f p1,p2,p3,p4;
	struct point3f p1,p2,p3,p4;
	//struct point2f previous_pos;
	struct point3f previous_pos;
	double interval;
	double curve_length;
	double curve_time;
};

// holds all information pertaining to a spec of dust 
struct dust{
	struct point3f pos;
	struct color3f color;
	int life;
};

std::vector<sphere> all_spheres;
std::vector<dust> tails;

//struct sphere next_ball;
double next_ball_mass;
double next_ball_radius;

/*
 * double ranged_random_value();
 *
 * returns a random value based on the the bounds of the system
 */ 
double ranged_random_value(double radius) {
	//printf("#### radius: %f\n", radius);
	double difference = 2 * (CUBE_LENGTH - (radius+0.0001));
	return (( (double)rand()/RAND_MAX )*difference)-(difference/2.);	
	//return (( (double)rand()/RAND_MAX )*difference);	
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
		red = ((double)rand()/RAND_MAX);
		if(red < 0.5) red = 0.0;
		green = ((double)rand()/RAND_MAX);
		if(green < 0.5) green = 0.0;
		blue = ((double)rand()/RAND_MAX);
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
		radius = ((double)rand()/RAND_MAX);
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
double get_mass( double radius) {
	double volume = (4 * PI * pow(radius,3))/3.;
	return volume * DENSITY;
}

/*
 * void update_position(struct sphere ball, double pos);
 *
 * returns the position of the ball on a curve
 */
void update_position( struct sphere *ball) {
	double a, b, c;
	//struct point2f result;
	// x
	c = 3 * (ball->p2.x - ball->p1.x);
	b = 3 * (ball->p3.x - ball->p2.x) - c;
	a = ball->p4.x - ball->p1.x - c - b;
	ball->pos.x = a * pow(ball->interval,3) + b * pow(ball->interval,2) + 
		c * ball->interval + ball->p1.x;

	// y
	c = 3 * (ball->p2.y - ball->p1.y);
	b = 3 * (ball->p3.y - ball->p2.y) - c;
	a = ball->p4.y - ball->p1.y - c - b;
	ball->pos.y = a * pow(ball->interval,3) + b * pow(ball->interval,2) + 
		c * ball->interval + ball->p1.y;
	
	// 3D
	// z
	c = 3 * (ball->p2.z - ball->p1.z);
	b = 3 * (ball->p3.z - ball->p2.z) - c;
	a = ball->p4.z - ball->p1.z - c - b;
	ball->pos.z = a * pow(ball->interval,3) + b * pow(ball->interval,2) +
		c * ball->interval + ball->p1.z;
}

/*
 * struct point2f get_position(struct sphere ball, double pos);
 *
 * returns the position of the ball on a curve
 */
struct point3f get_position( struct sphere *ball, double pos) {
	double a, b, c;
	struct point3f result;
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
	
	// 3D
	// z
	c = 3 * (ball->p2.z - ball->p1.z);
	b = 3 * (ball->p3.z - ball->p2.z) - c;
	a = ball->p4.z - ball->p1.z - c - b;
	result.z = a * pow(pos,3) + b * pow(pos,2) + c * pos + ball->p1.z;

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
	// 3D
	double z1, z2;
	double total = 0;
	struct point3f p1, p2;
	p1 = get_position( ball, 0);
	for(i = 1./CURVE_LENGTH_APPROX ; i <= 1; i+= 1./CURVE_LENGTH_APPROX) {
		p2 = get_position( ball, i);
		total = total + sqrt( pow(p2.x-p1.x,2) + pow(p2.y-p1.y,2) + pow(p2.z-p1.z,2));
		p1 = p2;
	}
	return total;
}

/*
 * void normalize(struct sphere ball);
 *
 * normalizes the delta_x and delta_y components of a sphere
 */
void normalize_dir(struct sphere *ball) {
	double mag = sqrt(pow(ball->direction.x,2) + pow(ball->direction.y,2) +
			pow(ball->direction.z,2));
	ball->direction.x /= mag;
	ball->direction.y /= mag;
	ball->direction.z /= mag;
}

/*
 * double distance(struct sphere ball1, struct sphere ball2);
 *
 * returns the distance between two parameter spheres
 */
double distance( struct sphere b1, struct sphere b2 ) {
	return sqrt( pow(b1.pos.x - b2.pos.x,2) + pow(b1.pos.y - b2.pos.y,2) + 
			pow(b1.pos.z - b2.pos.z,2));
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
	ball.pos.z += ball.direction.z * ball.velocity * timediff;
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
	point3f temp;
	temp = ball->pos;
	// store previous position
	ball->previous_pos = temp;
	ball->interval =( current - ball->start_time )/
		( CLOCKS_PER_SEC * ball->curve_time );
	update_position( ball );
}

/*
 * struct point2f new_curve_point(struct point2f origin);
 *
 * Creates a new point for the interpolation of a bezier curve affected by the
 * defined probabilities
 */
struct point3f new_curve_point(struct point3f origin){

	double p = ( (double)rand() /RAND_MAX);
	//printf("p: %f\n", p);
	struct point3f result;
	result = origin;
	double back, backN, backS, backE, backW, backNW, backSE, backSW, backNE;
	double midN, midS, midE, midW, midNW, midSE, midSW, midNE;
	double front, frontN, frontS, frontE, frontW, frontNW, frontSE, frontSW, frontNE;

	back = BACK;
	backN = BACK_N + back;
	backS = BACK_S + backN;
	backE = BACK_E + backS;
	backW = BACK_W + backE;
	backNW = BACK_NW + backW;
	backSE = BACK_SE + backNW;
	backSW = BACK_SW + backSE;
	backNE = BACK_NE + backSW;
	
	midN = MID_N + backNE;
	midS = MID_S + midN;
	midE = MID_E + midS;
	midW = MID_W + midE;
	midNW = MID_NW + midW;
	midSE = MID_SE + midNW;
	midSW = MID_SW + midSE;
	midNE = MID_NE + midSW;
	
	front = FRONT + midNE;
	frontN = FRONT_N + front;
	frontS = FRONT_S + frontN;
	frontE = FRONT_E + frontS;
	frontW = FRONT_W + frontE;
	frontNW = FRONT_NW + frontW;
	frontSE = FRONT_SE + frontNW;
	frontSW = FRONT_SW + frontSE;
	frontNE = FRONT_NE + frontSW;

	// back movement
	if( p < backNE ) {
		//printf("back\n");
		if(p < back) {
		// Move Back
			result.z = origin.z - STEP;
		} else if(p < backN){
		// Move Up - Back
			result.y = origin.y + STEP;
			result.z = origin.z - STEP;
		} else if (p < backS){
		// Move Down - Back
			result.y = origin.y - STEP;
			result.z = origin.z - STEP;
		} else if (p < backE){
		// Move Right - Back
			result.x = origin.x + STEP;
			result.z = origin.z - STEP;
		} else if(p < backW){
		// Move Left - Back
			result.x = origin.x - STEP;
			result.z = origin.z - STEP;
		} else if (p < backNW){
		// move north west - Back
			result.x = origin.x - STEP;
			result.y = origin.y + STEP;
			result.z = origin.z - STEP;
		} else if (p < backSE){
		// move south east - Back
			result.x = origin.x + STEP;
			result.y = origin.y - STEP;
			result.z = origin.z - STEP;
		} else if (p < backSW) {
		//move south west - Back
			result.x = origin.x - STEP;
			result.y = origin.y - STEP;
			result.z = origin.z - STEP;
		} else {
		//Move north east - Back
			result.x = origin.x + STEP;
			result.y = origin.y + STEP;
			result.z = origin.z - STEP;
		}
		return result;
	
	// mid movement
	}else if( p < midNE ) {
		//printf("mid\n");
		
		if(p < midN){
		// Move Up
			result.y = origin.y + STEP;
		} else if (p < midS){
		// Move down
			result.y = origin.y - STEP;
		} else if (p < midE){
		// Move Right
			result.x = origin.x + STEP;
		} else if(p < midW){
		// Move left
			result.x = origin.x - STEP;
		} else if (p < midNW){
		// move north west
			result.x = origin.x - STEP;
			result.y = origin.y + STEP;
		} else if (p < midSE){
		// move south east
			result.x = origin.x + STEP;
			result.y = origin.y - STEP;
		} else if (p < midSW) {
		//move south west
			result.x = origin.x - STEP;
			result.y = origin.y - STEP;
		} else {
		//Move north east
			result.x = origin.x + STEP;
			result.y = origin.y + STEP;
		}
		return result;

	// front movement
	} else {
		//printf("front\n");
		if(p < front) {
		// Move Front
			result.z = origin.z + STEP;
		} else if(p < frontN){
		// Move Up - Front
			result.y = origin.y + STEP;
			result.z = origin.z + STEP;
		} else if (p < frontS){
		// Move Down - Front
			result.y = origin.y - STEP;
			result.z = origin.z + STEP;
		} else if (p < frontE){
		// Move Right - Front
			result.x = origin.x + STEP;
			result.z = origin.z + STEP;
		} else if(p < frontW){
		// Move Left - Front
			result.x = origin.x - STEP;
			result.z = origin.z + STEP;
		} else if (p < frontNW){
		// move north west - Front
			result.x = origin.x - STEP;
			result.y = origin.y + STEP;
			result.z = origin.z + STEP;
		} else if (p < frontSE){
		// move south east - Front
			result.x = origin.x + STEP;
			result.y = origin.y - STEP;
			result.z = origin.z + STEP;
		} else if (p < frontSW) {
		//move south west - Front
			result.x = origin.x - STEP;
			result.y = origin.y - STEP;
			result.z = origin.z + STEP;
		} else {
		//Move north east - Front
			result.x = origin.x + STEP;
			result.y = origin.y + STEP;
			result.z = origin.z + STEP;
		}
		return result;

	}
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
	
	ball->p1.x = ball->p4.x;
	ball->p2.x = ( ball->p4.x - ball->p3.x ) + ball->p4.x;
	
	//ball->p3.x = ranged_random_value();
	//ball->p4.x = ranged_random_value();

	ball->p2.y = ( ball->p4.y - ball->p3.y ) + ball->p4.y;
	ball->p1.y = ball->p4.y;
	
	//ball->p3.y = ranged_random_value();
	//ball->p4.y = ranged_random_value();
	
	// 3D
	ball->p2.z = ( ball->p4.z - ball->p3.z ) + ball->p4.z;
	ball->p1.z = ball->p4.z;
	
	
	ball->p3 = new_curve_point(ball->p2);
	//printf("    x:%f y:%f\n",ball->p3.x,ball->p3.y); 
	ball->p4 = new_curve_point(ball->p3);
	//printf("    x:%f y:%f\n",ball->p4.x,ball->p4.y);
	
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
	double mass_before;
	while((double) clock() == current){} // waits for next time step
	current = (double) clock();

	// remove tails
	int k;
	for( k = 0; k < tails.size(); k++) {
		tails[k].life--;
		if(tails[k].life <= 0) {
			tails.erase(tails.begin()+k);
		}
	}

	for(j = 0; j < all_spheres.size(); j++) {
		
		// DECAY
		double decay = ((double) rand() / RAND_MAX);


		if( all_spheres[j].radius>0.0) {
			if( decay <= DECAY_PROB && all_spheres[j].active) {
				mass_before = get_mass(all_spheres[j].radius);
				all_spheres[j].radius -= 0.00045;
				mass_of_system += ( mass_before - get_mass(all_spheres[j].radius));
				
				if(dust_shown) {
					struct dust tail;
					double rad = all_spheres[j].radius;
					tail.pos.x = all_spheres[j].pos.x + 
						(((double) rand() / RAND_MAX) * (2*rad) - rad);
					tail.pos.y = all_spheres[j].pos.y + 
						(((double) rand() / RAND_MAX) * (2*rad) - rad);
					tail.pos.z = all_spheres[j].pos.z + 
						(((double) rand() / RAND_MAX) * (2*rad) - rad);
					tail.color = all_spheres[j].color;
					tail.life = 75;
							
					tails.resize(tails.size()+1);
					tails.push_back(tail);
				}
			}
		}else{
			all_spheres.erase(all_spheres.begin()+j);
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
		double dist_top = 10.0 - ball->radius;
		double dist_bottom = 0.0 + ball->radius;
		if(ball->path == 1) {
			ball->direction.x = ball->pos.x - ball->previous_pos.x;
			ball->direction.y = ball->pos.y - ball->previous_pos.y;
			// 3D
			ball->direction.z = ball->pos.z - ball->previous_pos.z;
			normalize_dir(ball);
		}
		if( ball->pos.x >= dist || ball->pos.x <= -1*dist) {
			ball->direction.x *= -1;
			ball->pos.x = ( ball->pos.x < 0.0  ) ? -1*dist : dist;
			ball->path = 0;
			ball->start_time = (double) clock();
			ball->active = 1;
		} else if(ball->pos.y >= dist_top || ball->pos.y <= dist_bottom) {
			ball->direction.y *= -1;
			ball->pos.y = ( ball->pos.y < 5.0 ) ? dist_bottom : dist_top;
			ball->path = 0;
			ball->start_time = (double) clock();
			ball->active = 1;
		// 3D
		} else if(ball->pos.z >= dist || ball->pos.z <= -1*dist) {
			ball->direction.z *= -1;
			ball->pos.z = ( ball->pos.z < 0.0 ) ? -1*dist : dist;
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
	
	double i_dir_mag = sqrt(pow(b1->direction.x,2) + pow(b1->direction.y,2) +
			pow(b1->direction.z,2));
	
	b1->pos.x -= (ipen * b1->direction.x ) / i_dir_mag;
	b1->pos.y -= (ipen * b1->direction.y ) /i_dir_mag;
	b1->pos.z -= (ipen * b1->direction.z ) /i_dir_mag;
	
	double j_dir_mag = sqrt(pow(b2->direction.x,2) + pow(b2->direction.y,2) +
			pow(b2->direction.z,2));
	b2->pos.x -= (jpen * b2->direction.x ) / j_dir_mag;
	b2->pos.y -= (jpen * b2->direction.y ) /j_dir_mag;	
	b2->pos.z -= (jpen * b2->direction.z ) /j_dir_mag;	
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
	ball->direction.z = ball->pos.z - ball->previous_pos.z;
	normalize_dir(ball);
}

/*
 * void collision_response( struct sphere *b1, struct sphere *b2);
 *
 * performs collision response between two balls
 */
void collision_response(struct sphere *b1, struct sphere *b2) {

	b1->active = 1;
	b2->active = 1;
	

	if(response == 1) {
		// proper 3D collisions using equation provided on 
		// http://www.plasmaphysics.org.uk/programs/coll3d_cpp.htm
		double R = 1.0;
		double m1 = get_mass(b1->radius); 
		double m2 = get_mass(b2->radius);
		double r1 = b1->radius;
		double r2 = b2->radius;

		double x1 = b1->pos.x;
		double y1 = b1->pos.y;
		double z1 = b1->pos.z;
		
		double x2 = b2->pos.x;
		double y2 = b2->pos.y;
		double z2 = b2->pos.z;
		double vx1 = (b1->direction.x * b1->velocity);
		double vy1 = (b1->direction.y * b1->velocity);
		double vz1 = (b1->direction.z * b1->velocity);
		
		double vx2 = (b2->direction.x * b2->velocity);
		double vy2 = (b2->direction.y * b2->velocity);
		double vz2 = (b2->direction.z * b2->velocity);

		double  pi,r12,m21,d,v,theta2,phi2,st,ct,sp,cp,vx1r,vy1r,vz1r,fvz1r,
				   thetav,phiv,dr,alpha,beta,sbeta,cbeta,dc,sqs,t,a,dvz2,
				   vx2r,vy2r,vz2r,x21,y21,z21,vx21,vy21,vz21,vx_cm,vy_cm,vz_cm;

	//     **** initialize some variables ****
		   pi=acos(-1.0E0);
		   //error=0;
		   r12=r1+r2;
		   m21=m2/m1;
		   x21=x2-x1;
		   y21=y2-y1;
		   z21=z2-z1;
		   vx21=vx2-vx1;
		   vy21=vy2-vy1;
		   vz21=vz2-vz1;
		   
		   vx_cm = (m1*vx1+m2*vx2)/(m1+m2) ;
		   vy_cm = (m1*vy1+m2*vy2)/(m1+m2) ;
		   vz_cm = (m1*vz1+m2*vz2)/(m1+m2) ;  

		   
	//     **** calculate relative distance and relative speed ***
		   d=sqrt(x21*x21 +y21*y21 +z21*z21);
		   v=sqrt(vx21*vx21 +vy21*vy21 +vz21*vz21);
		   
	//     **** shift coordinate system so that ball 1 is at the origin ***
		   x2=x21;
		   y2=y21;
		   z2=z21;
		   
	//     **** boost coordinate system so that ball 2 is resting ***
		   vx1=-vx21;
		   vy1=-vy21;
		   vz1=-vz21;

	//     **** find the polar coordinates of the location of ball 2 ***
		   theta2=acos(z2/d);
		   if (x2==0 && y2==0) {phi2=0;} else {phi2=atan2(y2,x2);}
		   st=sin(theta2);
		   ct=cos(theta2);
		   sp=sin(phi2);
		   cp=cos(phi2);


	//     **** express the velocity vector of ball 1 in a rotated coordinate
	//          system where ball 2 lies on the z-axis ******
		   vx1r=ct*cp*vx1+ct*sp*vy1-st*vz1;
		   vy1r=cp*vy1-sp*vx1;
		   vz1r=st*cp*vx1+st*sp*vy1+ct*vz1;
		   fvz1r = vz1r/v ;
		   if (fvz1r>1) {fvz1r=1;}   // fix for possible rounding errors
			  else if (fvz1r<-1) {fvz1r=-1;} 
		   thetav=acos(fvz1r);
		   if (vx1r==0 && vy1r==0) {phiv=0;} else {phiv=atan2(vy1r,vx1r);}

									
	//     **** calculate the normalized impact parameter ***
		   dr=d*sin(thetav)/r12;
 
	//     **** calculate impact angles if balls do collide ***
		   alpha=asin(-dr);
		   beta=phiv;
		   sbeta=sin(beta);
		   cbeta=cos(beta);			
		   
	//     **** calculate time to collision ***
		   t=(d*cos(thetav) -r12*sqrt(1-dr*dr))/v;
		 
	//     **** update positions and reverse the coordinate shift ***
		   x2=x2+vx2*t +x1;
		   y2=y2+vy2*t +y1;
		   z2=z2+vz2*t +z1;
		   x1=(vx1+vx2)*t +x1;
		   y1=(vy1+vy2)*t +y1;
		   z1=(vz1+vz2)*t +z1;
		   
	//  ***  update velocities ***

		   a=tan(thetav+alpha);
		   dvz2=2*(vz1r+a*(cbeta*vx1r+sbeta*vy1r))/((1+a*a)*(1+m21));
		   vz2r=dvz2;
		   vx2r=a*cbeta*dvz2;
		   vy2r=a*sbeta*dvz2;
		   vz1r=vz1r-m21*vz2r;
		   vx1r=vx1r-m21*vx2r;
		   vy1r=vy1r-m21*vy2r;

		   
	//     **** rotate the velocity vectors back and add the initial velocity
	//           vector of ball 2 to retrieve the original coordinate system ****
						 
		   vx1=ct*cp*vx1r-sp*vy1r+st*cp*vz1r +vx2;
		   vy1=ct*sp*vx1r+cp*vy1r+st*sp*vz1r +vy2;
		   vz1=ct*vz1r-st*vx1r               +vz2;
		   vx2=ct*cp*vx2r-sp*vy2r+st*cp*vz2r +vx2;
		   vy2=ct*sp*vx2r+cp*vy2r+st*sp*vz2r +vy2;
		   vz2=ct*vz2r-st*vx2r               +vz2;
			
		b1->direction.x = vx1;
		b1->direction.y = vy1;
		// 3D
		b1->direction.z = vz1;
		normalize_dir(b1);

		b2->direction.x = vx2;
		b2->direction.y = vy2;
		b2->direction.z = vz2;
		normalize_dir(b2);

		b1->velocity = 
			sqrt( pow(vx1,2) + pow(vy1,2) + pow(vz1,2));
		b2->velocity = 
			sqrt( pow(vx2,2) + pow(vy2,2) + pow(vz2,2));
	
	}else{
	
		// store before collision velocities
		double b1_v, b2_v;
		b1_v = b1->velocity;
		b2_v = b2->velocity;
		double b1_vx, b1_vy, b2_vx, b2_vy;
		// 3D
		double b1_vz, b2_vz;
		b1_vx = (b1->direction.x * b1_v);
		b1_vy = (b1->direction.y * b1_v);
		
		// 3D
		b1_vz = (b1->direction.z * b1_v);

		b2_vx = (b2->direction.x * b2_v);
		b2_vy = (b2->direction.y * b2_v);
		
		// 3D
		b2_vz = (b2->direction.z * b2_v);

		// have x and y components of speed

		// get ball masses
		double m1 = get_mass(b1->radius), m2 = get_mass(b2->radius);

		// need the new velocity components (after collision)
		double b1_vx_new, b1_vy_new, b2_vx_new, b2_vy_new;

		// 3D
		double b1_vz_new, b2_vz_new;

		// ball 1 new components
		b1_vx_new = ((m1-m2) * b1_vx + (2*m2) * b2_vx)/(m1+m2);
		b1_vy_new = ((m1-m2) * b1_vy + (2*m2) * b2_vy)/(m1+m2);
		// 3D
		b1_vz_new = ((m1-m2) * b1_vz + (2*m2) * b2_vz)/(m1+m2);

		// ball 2 new components
		b2_vx_new = ((m2-m1) * b2_vx + (2*m1) * b1_vx)/(m1+m2);
		b2_vy_new = ((m2-m1) * b2_vy + (2*m1) * b1_vy)/(m1+m2);
		// 3D
		b2_vz_new = ((m2-m1) * b2_vz + (2*m1) * b1_vz)/(m1+m2);

		// need to change direction to match new speeds
		b1->direction.x = b1_vx_new;
		b1->direction.y = b1_vy_new;
		// 3D
		b1->direction.z = b1_vz_new;
		normalize_dir(b1);

		b2->direction.x = b2_vx_new;
		b2->direction.y = b2_vy_new;
		b2->direction.z = b2_vz_new;
		normalize_dir(b2);
		
		b1->velocity = 
			sqrt( pow(b1_vx_new,2) + pow(b1_vy_new,2) + pow(b1_vz_new,2));
		b2->velocity = 
			sqrt( pow(b2_vx_new,2) + pow(b2_vy_new,2) + pow(b2_vz_new,2));
		// speeds updated
	}
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
				if(all_spheres[i].ghost + all_spheres[j].ghost >= 1) {
					continue;
				}
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
// 3D
struct point3f random_ranged_point(double radius) {
		return (struct point3f) {ranged_random_value(radius), 
								 ranged_random_value(radius)+5.0,
								 ranged_random_value(radius)
		};
}

/*
 * struct vector2f random_direction();
 *
 * Returns a random direction vector that has been normalized.
 */
// 3D
struct vector3f random_direction(double radius){
		vector3f direction;
		direction.x = ranged_random_value(radius);
		direction.y = ranged_random_value(radius)+5.0;
		direction.z = ranged_random_value(radius);
		double mag = sqrt(pow(direction.x,2) + pow(direction.y,2) + pow(direction.z,2));
		direction.x /= mag;
		direction.y /= mag;
		direction.z /= mag;
		return direction;
}

/*
 * double random_velocity();
 *
 * Returns a random speed between 0.0 and BALL_SPEED
 */
double random_velocity() {
	double velocity;
	velocity = (((double) rand() / RAND_MAX) * BALL_SPEED/2.) + (BALL_SPEED/2.);
	//printf("velocity %f\n", velocity);
	return velocity;
}

/*
 * struct sphere generate_sphere();
 * 
 * return a ball 
 */
struct sphere generate_sphere(int rad) {
		struct sphere ball;
		
		do{
			// RADIUS MUST BE BEFORE RANDOM POINTS
			ball.radius = (rad) ? next_ball_radius : random_radius();
			// gets 4 random points for the bezier curve
			ball.p1 = random_ranged_point(ball.radius);
			ball.p2 = new_curve_point(ball.p1);		
			ball.p3 = new_curve_point(ball.p2);
			ball.p4 = new_curve_point(ball.p3);
			
			// position starts at p1
			ball.pos = ball.p1;
			// 3D
			ball.previous_pos = {0.0, 5.0, 0.0};
			
			// gets a random direction, this might be a wasted step
			ball.direction = random_direction(ball.radius);

			ball.active = 0;

			ball.velocity = random_velocity();	

			// start on a curved path
			ball.path = 1;
			ball.color = random_color();		

			ball.curve_length = curve_length( &ball );
			ball.start_time = (double) clock();
			ball.curve_time = ball.curve_length / ball.velocity;
			ball.ghost = 0;
			// 3D
		}while(collision_detection(ball) == 1 || ball.p1.x == 0.0 || ball.p1.y == 0.0 ||
				ball.p1.z == 0.0);	
		
		return ball;
}

/*
 * void print_sphere( struct sphere *ball);
 *
 * prints sphere info, used for debugging
 */
void print_sphere( struct sphere *ball) {
	printf("xpos: %f ypos: %f zpos: %f\n", ball->pos.x, ball->pos.y, ball->pos.z);
	printf("direction: %f %f\n", ball->direction.x, ball->direction.y);
	printf("direction: %f %f %f\n", ball->direction.x, ball->direction.y, ball->direction.z);
	printf("interval: %f\n", ball->interval);
	printf("prev pos -- x: %f y: %f z:%f\n", ball->previous_pos.x,
			ball->previous_pos.y, ball->previous_pos.z);
	printf("p1 -- x: %f y: %f z: %f\n", ball->p1.x, ball->p1.y, ball->p1.z);
	printf("p2 -- x: %f y: %f z: %f\n", ball->p2.x, ball->p2.y, ball->p2.z);
	printf("p3 -- x: %f y: %f z: %f\n", ball->p3.x, ball->p3.y, ball->p3.z);
	printf("p4 -- x: %f y: %f z: %f\n", ball->p4.x, ball->p4.y, ball->p4.z);
	printf("radius: %f", ball->radius);
	printf("color: %f %f %f\n",
			ball->color.red,
			ball->color.green,
			ball->color.blue);
	printf("velocity: %f\n", ball->velocity);
	printf("curve length: %f\n", ball->curve_length);
	printf("start time: %f\n", ball->start_time);
	printf("curve time: %f\n", ball->curve_time);
	printf("path: %d\n", ball->path);
	printf("active:	%d\n", ball->active);
}

/*
 * void spawn_next_ball();
 *
 * checks if the system has enough mass to spawn the new ball
 */
void spawn_next_ball() {
	//printf("%f/%f till next spawn\n", mass_of_system, next_ball_mass);
	if( mass_of_system >= next_ball_mass ) {
		all_spheres.resize(all_spheres.size()+1);
		all_spheres.push_back( generate_sphere(1) );
		mass_of_system -= next_ball_mass; 
		next_ball_radius = random_radius();
		next_ball_mass = get_mass( next_ball_radius );
	}
}

/*
 * void keyMove (double deltaMove );
 *
 * 
 */ 
void keyMove (double deltaMove ) {
	x += deltaMove * lx * 0.1;
	y += deltaMove * ly * 0.1;
	z += deltaMove * lz * 0.1;
}

void draw_spheres(int a) {
	int i;
	for(i = 0; i < all_spheres.size(); i++) {	// draw all spheres
		//printf("%f %f\n", all_spheres[i].pos.x, all_spheres[i].pos.y);
		glPushMatrix();
		glColor3f(all_spheres[i].color.red,
				all_spheres[i].color.green,
				all_spheres[i].color.blue);
		glTranslatef(all_spheres[i].pos.x,all_spheres[i].pos.y, all_spheres[i].pos.z);
		if( all_spheres[i].active == 0) {
			if( a == 1 ){
				glColor3f( 1.0, 1.0, 1.0);
				glutWireSphere(all_spheres[i].radius, 10, 10);
			}
		}else{
			glutSolidSphere(all_spheres[i].radius, 25, 25);
		}
		glPopMatrix();
	}

}

void draw_dust() {
	glPushMatrix();
	int j;
	glBegin(GL_POINTS);
	for( j = 0; j < tails.size(); j++) {
		
		double perc = tails[j].life/10.;
	  	  glColor4f( tails[j].color.red*perc,
				  	 tails[j].color.green*perc,
					 tails[j].color.blue*perc,
					 0.3);
	  	  glVertex3f(tails[j].pos.x, tails[j].pos.y, tails[j].pos.z);
	  	  

	}
	glEnd();
	glPopMatrix();

}

void draw_box() {
	glDisable(GL_LIGHTING);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glColor4f(0.5f, 0.5f, 0.5f, 0.1f);
	// front box
	glBegin(GL_QUADS);
	  glVertex3f(-5,  0, 5);
	  glVertex3f( 5,  0, 5);
	  glVertex3f( 5, 10, 5);
	  glVertex3f(-5, 10, 5);
	glEnd();
	
	// back box
	glBegin(GL_QUADS);
	  glVertex3f(-5, 0, -5);
	  glVertex3f( 5, 0, -5);
	  glVertex3f( 5, 10, -5);
	  glVertex3f(-5, 10, -5);
	glEnd();

	glColor4f(0.4f, 0.4f, 0.4f, 0.1f);
	// left side
	glBegin(GL_QUADS);
	  glVertex3f(-5, 0, -5);
	  glVertex3f(-5, 0,  5);
	  glVertex3f(-5, 10,  5);
	  glVertex3f(-5, 10, -5);
	glEnd();

	// right side
	glBegin(GL_QUADS);
	  glVertex3f( 5, 0, -5);
	  glVertex3f( 5, 0,  5);
	  glVertex3f( 5, 10,  5);
	  glVertex3f( 5, 10, -5);
	glEnd();
	glColor4f(0.6f, 0.6f, 0.6f, 0.1f);
	// top
	glBegin(GL_QUADS);
	  glVertex3f(-5, 10, -5);
	  glVertex3f(-5, 10,  5);
	  glVertex3f( 5, 10,  5);
	  glVertex3f( 5, 10, -5);
	glEnd();
	glPopMatrix();

	glPopMatrix();
	
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
}

/*
 * void display();
 *
 * The display function displays the animation to the users screen.
 */
void display() {

	if(deltaMove) {
		keyMove(deltaMove);
	}
	glClearStencil(0);
	glClearDepth(1.0f);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();
    
    gluLookAt(x, y, z, x+lx, y+ly, z+lz, 0.0, 1.0, 0.0);
	// MUST BE BEFORE SPHERES ARE DRAWN
	collision_check();
	
	
	if(reflection) {	
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	// DRAW FLOOR 
	glPushMatrix();
	// bottom
	glBegin(GL_QUADS);
	  glVertex3f(-5, 0, -5);
	  glVertex3f(-5, 0,  5);
	  glVertex3f( 5, 0,  5);
	  glVertex3f( 5, 0, -5);
	glEnd();
	glPopMatrix();

	// END DRAW FLOOR

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glScalef(1.0f, -1.0f, 1.0f);

	// DRAW INVERTED SCENE
		glEnable(GL_LIGHTING);
		draw_spheres(0);
		//draw_dust();
		glDisable(GL_LIGHTING);
		draw_box();
	// END DRAW SCENE

	glPopMatrix();
	}
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// DRAW FLOOR AGAIN
	glPushMatrix();
	glColor4f(0.6f, 0.6f, 0.6f, 0.1f);
	// bottom
	glBegin(GL_QUADS);
	  glVertex3f(-5, 0, -5);
	  glVertex3f(-5, 0,  5);
	  glVertex3f( 5, 0,  5);
	  glVertex3f( 5, 0, -5);
	glEnd();

	// END DRAW FLOOR

	glDisable(GL_BLEND);

	// DRAW SCENE
		glEnable(GL_LIGHTING);
		draw_spheres(1);
		draw_dust();
		glDisable(GL_LIGHTING);
		draw_box();
	// END DRAW SCENE


	//draw_dust();
	//glDisable(GL_LIGHTING);
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//draw_box();
	//
	//glColor4f(0.6f, 0.6f, 0.6f, 0.1f);
	//// bottom
	//glBegin(GL_QUADS);
	//  glVertex3f(-5,  5, -5);
	//  glVertex3f(-5,  5,  5);
	//  glVertex3f( 5,  5,  5);
	//  glVertex3f( 5,  5, -5);
	//glEnd();

	//
	//glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

	if(showText){
		glColor3f( 1.0f, 1.0f, 1.0f);
		glRasterPos3f(x+lx,y+ly,z+lz);
		char blah[20];
		sprintf(blah,"%d - %2.0f %",all_spheres.size(), mass_of_system/next_ball_mass*100);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)blah);
	}
	glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
	
	spawn_next_ball();
	
	glutSwapBuffers();
}

/*
 * void gfxinit();
 *
 * initializes the system prior to animating
 */
void gfxinit() {
	mass_of_system = 0.0;
	balls = NUMBER_OF_BALLS;
	all_spheres.resize(balls);
	
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
    gluPerspective(60.0, 16/9., 0.1, 500.0);
    //glOrtho(-5.0,5.0,-5.0,5.0,1.0,20.0);
	glMatrixMode(GL_MODELVIEW);
    //gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	
	//glRasterPos2i(10, 10);
	//glColor3f(1.0f, 1.0f, 1.0f);
	//glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"dog");

	glEnable ( GL_COLOR_MATERIAL ) ;
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glShadeModel(GL_SMOOTH);

	srand(time(NULL));	// seed for rand() calls

	// create all spheres for the initial system state
	int k;
	for( k = 0; k < all_spheres.size(); k++ ) { //setup all ball settings
		//printf("%d\n",k);
		all_spheres[k] = generate_sphere(0);
	}
	next_ball_radius = random_radius();
	next_ball_mass = get_mass( next_ball_radius ); 
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
			//balls++;
			all_spheres.resize(all_spheres.size()+1);
			//sphere ball;
			//ball = generate_sphere(0);
			//printf("\nKEY PRESS SPAWN\n\n");
			//print_sphere(&ball);
			//all_spheres.push_back(ball);
			all_spheres.push_back( generate_sphere(0) );
		}
			break;
		case 99:	// [c] collisions response mode
			response = (response == 0) ? 1 : 0;
			break;
		case 100:
			dust_shown = ( dust_shown ) ? 0 : 1;
			break;
		case 119: // [w] forward
			deltaMove = 0.5;
			break;
		case 114: // [r] relfection on/off
			reflection = (reflection) ? 0 : 1;
			break;
		case 115: // [s] back
			deltaMove = -0.5;
			break;
		case 116: // [t] text
			showText = (showText) ? 0 : 1; 
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
 * void key_release( unisgned char key, int x, int y);
 *
 * when the key is released, stop moving in that direction 
 */
void key_release( unsigned char key, int x, int y) {
	switch(key) {
		case 119: // [w] forward
			deltaMove = 0.0;
			break;
		case 115: // [s] back
			deltaMove = 0.0;
			break;
	}	
}

/*
 * void mouse_move( int x, int y);
 *
 * Detects the change is mouse direction
 */
void mouse_move( int x, int y) {
	if( xOrigin >= 0) {
		deltaAngleX = ( x - xOrigin) * 0.02;
		deltaAngleY = ( y - yOrigin) * 0.02;

		lx = sin(angleX + deltaAngleX);
		ly = -1*sin(angleY + deltaAngleY);
		lz = -1*cos(angleX + deltaAngleX);
	}
}

/*
 * void mouse_button( int button, int state, int x, int y);
 *
 * Handles mouse button presses
 */
void mouse_button( int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON) {
		if( state == GLUT_UP) {
			angleX += deltaAngleX;
			angleY += deltaAngleY;
			xOrigin = -1;
			yOrigin = -1;
		}else{
			xOrigin = x;
			yOrigin = y;
		}
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
     glutInitWindowSize(1600,900);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
     
	 window = glutCreateWindow("Sphere Collisions"); //window title
     glutDisplayFunc(display);
     glutIdleFunc(animate);	// call animate() when idle
 	glutIgnoreKeyRepeat(1);
	 glutKeyboardFunc(keystroke);	//handles user input
	 glutKeyboardUpFunc(key_release);
	 glutMouseFunc(mouse_button);
	 glutMotionFunc(mouse_move);
	 gfxinit();
     glutMainLoop(); // start the animation
}
