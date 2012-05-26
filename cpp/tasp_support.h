#ifndef TASP_SUPPPORT_H
#define TASP_SUPPORT_H

namespace tasp
{

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
	// point2f pos;
	 point3f pos;
	double velocity;
	// vector2f direction;
	 vector3f direction;
	//double mass;
	double radius;
	
	int path;  // flag: 0 is linear path, 1 is a bezier curve
	int active;
	 color3f color;
	double start_time;
	int ghost;

	// point2f p1,p2,p3,p4;
	 point3f p1,p2,p3,p4;
	// point2f previous_pos;
	 point3f previous_pos;
	double interval;
	double curve_length;
	double curve_time;
};

// holds all information pertaining to a spec of dust 
struct dust{
	 point3f pos;
	 color3f color;
	int life;
};

void set_cube_length( double cl );
void set_ball_speed( double bs );

/*
 * returns a random value based on the the bounds of the system
 */ 
double ranged_random_value(double radius);

/*
 * returns a randomized RGB color w/o dark colors
 */
color3f random_color();

/*
 * returns a random radius between 0.15 and 0.7
 */
double random_radius();

/*
 * Returns a randomized 2D point within a bounded 2D area.
 */
point3f random_ranged_point(double radius);

/*
 * Returns a random direction vector that has been normalized.
 */
vector3f random_direction(double radius);

/*
 * Returns a random speed between 0.0 and BALL_SPEED
 */
double random_velocity();

} // namespace tasp

#endif