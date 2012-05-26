#include "tasp_sphere.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

namespace tasp
{

// constructor sets up new sphere
// NOTE : while condition was removed
// TODO : maybe include it again
Sphere::Sphere(double rad)
{
	// RADIUS MUST BE BEFORE RANDOM POINTS
	radius = rad;
	// gets 4 random points for the bezier curve
	p1 = random_ranged_point(radius);
	p2 = new_curve_point(p1);		
	p3 = new_curve_point(p2);
	p4 = new_curve_point(p3);
	
	// position starts at p1
	pos = p1;
	// 3D
	previous_pos.x = 0.0;
	previous_pos.y = 5.0;
	previous_pos.z = 0.0;
	// = {0.0, 5.0, 0.0};
		
	// gets a random direction, this might be a wasted step
	direction = random_direction(radius);

	active = 0;
	velocity = random_velocity();	

	// start on a curved path
	path = 1;
	color = random_color();		

	curve_length = get_curve_length();
	start_time = (double) clock();
	curve_time = curve_length / velocity;
	ghost = 0;
}

// desructor does nothing
Sphere::~Sphere(){}

// gets a new curve point
point3f Sphere::new_curve_point( point3f origin ){

	double p = ( (double)rand() /RAND_MAX);
	//printf("p: %f\n", p);
	point3f result;
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
 * CLASS : SPHERE
 * point2f get_position( sphere ball, double pos);
 *
 * returns the position of the ball on a curve
 */
point3f Sphere::get_position( double pos ) {
	double a, b, c;
	point3f result;
	// x
	c = 3 * (p2.x - p1.x);
	b = 3 * (p3.x - p2.x) - c;
	a = p4.x - p1.x - c - b;
	result.x = a * pow(pos,3) + b * pow(pos,2) + c * pos + p1.x;

	// y
	c = 3 * (p2.y - p1.y);
	b = 3 * (p3.y - p2.y) - c;
	a = p4.y - p1.y - c - b;
	result.y = a * pow(pos,3) + b * pow(pos,2) + c * pos + p1.y;
	
	// 3D
	// z
	c = 3 * (p2.z - p1.z);
	b = 3 * (p3.z - p2.z) - c;
	a = p4.z - p1.z - c - b;
	result.z = a * pow(pos,3) + b * pow(pos,2) + c * pos + p1.z;

	return result;
}

/*
 * CLASS : SPHERE
 * double curve_length( sphere ball );
 *
 * returns a double value representing the approximate length of the bezier
 * curve
 */
double Sphere::get_curve_length() {
	double i;
	double x1, y1, x2, y2;
	// 3D
	double z1, z2;
	double total = 0;
	point3f pp1, pp2;
	pp1 = get_position(0);
	for(i = 1./CURVE_LENGTH_APPROX ; i <= 1; i+= 1./CURVE_LENGTH_APPROX) {
		pp2 = get_position(i);
		total = total + sqrt( pow(pp2.x-pp1.x,2) + pow(pp2.y-pp1.y,2) + pow(pp2.z-pp1.z,2));
		pp1 = pp2;
	}
	return total;
}
/*
 * CLASS : SPHERE
 * void generate_curve( sphere ball);
 *
 * generates a new bezier curve based on a previous one
 */
void Sphere::generate_curve() {
	// store previous position
	previous_pos = pos;
	interval = 0.0;
	
	p1.x = p4.x;
	p2.x = ( p4.x - p3.x ) + p4.x;
	
	//p3.x = ranged_random_value();
	//p4.x = ranged_random_value();

	p2.y = ( p4.y - p3.y ) + p4.y;
	p1.y = p4.y;
	
	//p3.y = ranged_random_value();
	//p4.y = ranged_random_value();
	
	// 3D
	p2.z = ( p4.z - p3.z ) + p4.z;
	p1.z = p4.z;
	
	
	p3 = new_curve_point(p2);
	//printf("    x:%f y:%f\n",p3.x,p3.y); 
	p4 = new_curve_point(p3);
	//printf("    x:%f y:%f\n",p4.x,p4.y);
	
	curve_length = get_curve_length();
	start_time = (double) clock();
	curve_time = curve_length / velocity;
	//return ball;
}

/*
 * prints sphere info, used for debugging
 */
void Sphere::print_sphere() {
	printf("xpos: %f ypos: %f zpos: %f\n", pos.x, pos.y, pos.z);
	printf("direction: %f %f\n", direction.x, direction.y);
	printf("direction: %f %f %f\n", direction.x, direction.y, direction.z);
	printf("interval: %f\n", interval);
	printf("prev pos -- x: %f y: %f z:%f\n", previous_pos.x,
			previous_pos.y, previous_pos.z);
	printf("p1 -- x: %f y: %f z: %f\n", p1.x, p1.y, p1.z);
	printf("p2 -- x: %f y: %f z: %f\n", p2.x, p2.y, p2.z);
	printf("p3 -- x: %f y: %f z: %f\n", p3.x, p3.y, p3.z);
	printf("p4 -- x: %f y: %f z: %f\n", p4.x, p4.y, p4.z);
	printf("radius: %f", radius);
	printf("color: %f %f %f\n",
			color.red,
			color.green,
			color.blue);
	printf("velocity: %f\n", velocity);
	printf("curve length: %f\n", curve_length);
	printf("start time: %f\n", start_time);
	printf("curve time: %f\n", curve_time);
	printf("path: %d\n", path);
	printf("active:	%d\n", active);
}

/*
 * normalizes the delta_x and delta_y components of a sphere
 */
void Sphere::normalize_dir() {
	double mag = sqrt(pow(direction.x,2) + pow(direction.y,2) +
			pow(direction.z,2));
	direction.x /= mag;
	direction.y /= mag;
	direction.z /= mag;
}

/*
 * Generates a direction based on the previous direction. Used when a sphere
 * travelling on a curve path, become a straight path.
 */
void Sphere::update_direction() {
	direction.x = pos.x - previous_pos.x;
	direction.y = pos.y - previous_pos.y;
	direction.z = pos.z - previous_pos.z;
	normalize_dir();
}

/*
 * returns the mass of the ball based on the radius
 * Note: all balls have the same density
 * volume = (4*Pi * r^3) / 3
 */
double Sphere::get_mass() {
	double volume = (4 * PI * pow(radius,3))/3.;
	return volume * DENSITY;
}

/*
 * returns the position of the ball on a curve
 */
void Sphere::update_position() {
	double a, b, c;
	// point2f result;
	// x
	c = 3 * (p2.x - p1.x);
	b = 3 * (p3.x - p2.x) - c;
	a = p4.x - p1.x - c - b;
	pos.x = a * pow(interval,3) + b * pow(interval,2) + 
		c * interval + p1.x;

	// y
	c = 3 * (p2.y - p1.y);
	b = 3 * (p3.y - p2.y) - c;
	a = p4.y - p1.y - c - b;
	pos.y = a * pow(interval,3) + b * pow(interval,2) + 
		c * interval + p1.y;
	
	// 3D
	// z
	c = 3 * (p2.z - p1.z);
	b = 3 * (p3.z - p2.z) - c;
	a = p4.z - p1.z - c - b;
	pos.z = a * pow(interval,3) + b * pow(interval,2) +
		c * interval + p1.z;
}

/*
 * advances the sphere along the vector path
 */
void Sphere::move_on_vector(double current) {
	double timediff = ( current - start_time ) / ( CLOCKS_PER_SEC );

	//store previous positions
	previous_pos = pos;
	
	// update position
	pos.x += direction.x * velocity * timediff;
	pos.y += direction.y * velocity * timediff;
	pos.z += direction.z * velocity * timediff;
	start_time = (double) clock();
	//return ball;
}

/*
 * advances the sphere along the curved path
 */
void Sphere::move_on_curve(double current) {
	point3f temp;
	temp = pos;
	// store previous position
	previous_pos = temp;
	interval =( current - start_time )/
		( CLOCKS_PER_SEC * curve_time );
	update_position();
}

} // namespace tasp