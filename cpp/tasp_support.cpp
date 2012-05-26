#include "tasp_support.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

namespace tasp
{

// there should have been an easier way to pass this around as a global
// variable
double cubeLength;
double ballSpeed;

void set_cube_length( double cl ) {
	cubeLength = cl;
}

double get_cube_length() {
	return cubeLength;
}

void set_ball_speed( double bs ) {
	ballSpeed = bs;
}

double ranged_random_value(double radius) {
	double difference = 2 * (cubeLength - (radius+0.0001));
	return (( (double)rand()/RAND_MAX )*difference)-(difference/2.);
}

color3f random_color(){
	color3f color;
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

double random_radius() {
	double radius;
	do{
		radius = ((double)rand()/RAND_MAX);
	}while(radius > 0.7 || radius < 0.15);
	return radius;
}

point3f random_ranged_point(double radius) {
		return ( point3f) {ranged_random_value(radius), 
								 ranged_random_value(radius)+5.0,
								 ranged_random_value(radius)
		};
}

vector3f random_direction(double radius){
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

double random_velocity() {
	double velocity;
	velocity = (((double) rand() / RAND_MAX) * ballSpeed/2.) + (ballSpeed/2.);
	return velocity;
}

} // namespace tasp