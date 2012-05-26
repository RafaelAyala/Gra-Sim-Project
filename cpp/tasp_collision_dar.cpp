// Collision Detection and Response

#include "tasp_collision_dar.h"
#include <vector>
#include <math.h>
#include <time.h>

namespace tasp
{

// this is a silly way to do it
int response;
void set_response(int x) {
	response = x;
}

/*
 * returns the distance between two parameter spheres
 */
double distance( Sphere* b1, Sphere* b2 ) {
	return sqrt( pow(b1->pos.x - b2->pos.x,2) + pow(b1->pos.y - b2->pos.y,2) + 
			pow(b1->pos.z - b2->pos.z,2));
}

/*
 * returns 1 when a collision is detected with ball
 * returns 0 when no collisions detected with ball
 */
int collision_detection(  std::vector<Sphere*> all_spheres, Sphere* ball ) {
	int i, count;
	double d;
	count = 0;
	for( i = 0; i < all_spheres.size(); i++ ) {
		d = distance(ball, all_spheres[i]);
		
		if( d <= ball->radius + all_spheres[i]->radius + 0.0001) {
			count++;
		}
	}
	return (count>0) ? 1 : 0;
}

/*
 * checks for a ball to wall collision
 */ 
void wall_check(  Sphere *ball ) {	
		//return ball;
		double dist = 5.0 - ball->radius;
		double dist_top = 10.0 - ball->radius;
		double dist_bottom = 0.0 + ball->radius;
		if(ball->path == 1) {
			ball->direction.x = ball->pos.x - ball->previous_pos.x;
			ball->direction.y = ball->pos.y - ball->previous_pos.y;
			// 3D
			ball->direction.z = ball->pos.z - ball->previous_pos.z;
			ball->normalize_dir();
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
 * nudges sphere so they are at the point of collision, uses d to determine
 * the amount of peneatation and corrects it.
 */
void nudge_spheres(  Sphere* b1,  Sphere* b2, double d) {
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
 * performs collision response between two balls
 */
void collision_response( Sphere* b1, Sphere* b2 ) {

	b1->active = 1;
	b2->active = 1;
	

	if(response == 1) {
		// proper 3D collisions using equation provided on 
		// http://www.plasmaphysics.org.uk/programs/coll3d_cpp.htm
		double R = 1.0;
		double m1 = b1->get_mass(); 
		double m2 = b2->get_mass();
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
		b1->normalize_dir();

		b2->direction.x = vx2;
		b2->direction.y = vy2;
		b2->direction.z = vz2;
		b2->normalize_dir();

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
		double m1 = b1->get_mass(), m2 = b2->get_mass();

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
		b1->normalize_dir();

		b2->direction.x = b2_vx_new;
		b2->direction.y = b2_vy_new;
		b2->direction.z = b2_vz_new;
		b2->normalize_dir();
		
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
 * determines if any ball-wall or ball-ball collisions occur
 */
void collision_check(std::vector<Sphere*> all_spheres) {
	int i, j;
	double d;

	for(i = 0; i < all_spheres.size(); i++) {

		wall_check(all_spheres[i]);
		
		// ball-ball collisions
		if( i < all_spheres.size()-1 ){
			for( j = i+1; j < all_spheres.size(); j++) {
				if(all_spheres[i]->ghost + all_spheres[j]->ghost >= 1) {
					continue;
				}
				d = distance(all_spheres[i], all_spheres[j]);
				
				// if a collision
				if( d <= all_spheres[i]->radius + all_spheres[j]->radius) {
					
					// COLLISION RESPONSE STARTS
					
					// two curves colliding, need to generate direction vectors
					if(all_spheres[i]->path + all_spheres[j]->path == 2){
						{
							all_spheres[i]->update_direction();
							all_spheres[j]->update_direction();						
						}	
					// one curve and 1 ball collide, need to generate vector
					}else if(all_spheres[i]->path + all_spheres[j]->path == 1){
						if(all_spheres[i]->path == 1) { //[i] is the curved ball
							all_spheres[i]->update_direction();						
						}else{ //[j] is the curved ball
							all_spheres[j]->update_direction();						
						}
						// END BALL-CURVE
					}
						nudge_spheres(all_spheres[i], all_spheres[j], d);
						collision_response(all_spheres[i], all_spheres[j]);
				}
			}
		}
		// end ball-ball
	}
}


} // namespace tasp