#ifndef TASP_SPHERE_H
#define TASP_SPHERE_H

#include "tasp_support.h"

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
#define CURVE_LENGTH_APPROX 16
#define PI 3.14159
#define DENSITY 1.0

namespace tasp
{

// holds all information pertaining to a single sphere
class Sphere{
  public:
	point3f pos;
	double velocity;
	vector3f direction;
	double radius;
	
	int path;
	int active;
	color3f color;
	double start_time;
	int ghost;

	point3f p1,p2,p3,p4;
	point3f previous_pos;
	double interval;
	double curve_length;
	double curve_time;
	Sphere(double rad);
	~Sphere();
	/*
	 * Creates a new point for the interpolation of a bezier curve affected by the
	 * defined probabilities
	 */
	point3f new_curve_point( point3f origin );

	/*
	 * returns the position of the ball on a curve
	 */
	point3f get_position( double pos );

	/*
	 * returns a double value representing the approximate length of the bezier
	 * curve
	 */
	double get_curve_length();

	/*
	 * generates a new bezier curve based on a previous one
	 */
	void generate_curve();

	/*
	 * prints sphere info, used for debugging
	 */
	void print_sphere();

	/*
	 * normalizes the delta_x and delta_y components of a sphere
	 */
	void normalize_dir();
	
	/*
	 * Generates a direction based on the previous direction. Used when a sphere
	 * travelling on a curve path, become a straight path.
	 */
	void update_direction();

	/*
	 * returns the mass of the ball based on the radius
	 * Note: all balls have the same density
	 * volume = (4*Pi * r^3) / 3
	 */
	double get_mass();

	/*
	 * returns the position of the ball on a curve
	 */
	void update_position();

	/*
	 * advances the sphere along the vector path
	 */
	void move_on_vector(double current);

	/*
	 * advances the sphere along the curved path
	 */
	void move_on_curve(double current);
	
};

} // namespace tasp

#endif