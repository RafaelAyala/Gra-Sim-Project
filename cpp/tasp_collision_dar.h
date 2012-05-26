#ifndef TASP_COLLISION_DAR_H
#define TASP_COLLISION_DAR_H

#include "tasp_sphere.h"
#include <vector>

namespace tasp
{

void set_response(int x);

/*
 * returns the distance between two parameter spheres
 */
double distance( Sphere* b1, Sphere* b2 );

/*
 * returns 1 when a collision is detected with ball
 * returns 0 when no collisions detected with ball
 */
int collision_detection(  std::vector<Sphere*>* all_spheres, Sphere* ball );

/*
 * checks for a ball to wall collision
 */ 
void wall_check(  Sphere *ball );

/*
 * nudges sphere so they are at the point of collision, uses d to determine
 * the amount of peneatation and corrects it.
 */
void nudge_spheres(  Sphere* b1,  Sphere* b2, double d);

/*
 * performs collision response between two balls
 */
void collision_response( Sphere* b1, Sphere* b2 );

/*
 * determines if any ball-wall or ball-ball collisions occur
 */
void collision_check(std::vector<Sphere*> all_spheres);

} // namespace tasp

#endif