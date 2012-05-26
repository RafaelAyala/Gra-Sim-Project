#ifndef TASP_DRAW_FUNCTIONS_H
#define TASP_DRAW_FUNCTIONS_H

#include <vector>
#include "tasp_sphere.h"
#include "tasp_support.h"

namespace tasp 
{

// holds all information pertaining to a spec of dust 
struct dust{
	 point3f pos;
	 color3f color;
	int life;
};

/*
 * draws the spheres in the systems
 */
void draw_spheres(std::vector<Sphere*>* all_spheres, int a);

/*
 * draws the dust trails behind the box
 */
void draw_dust(std::vector<dust*>* tails);

/*
 * draws the cube that all the spheres are bound in
 */
void draw_box();

} // namespace tasp

#endif