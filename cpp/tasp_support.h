#ifndef TASP_SUPPORT_H
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