#include "tasp_draw_functions.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/freeglut_ext.h>
#include <stdio.h>


namespace tasp 
{

void draw_spheres(std::vector<Sphere*>* all_spheres, int a) {
	int i;
	for(i = 0; i < all_spheres->size(); i++) {	// draw all spheres
		//printf("%f %f\n", (*all_spheres)[i]->pos.x, (*all_spheres)[i]->pos.y);
		glPushMatrix();
		glColor3f((*all_spheres)[i]->color.red,
				(*all_spheres)[i]->color.green,
				(*all_spheres)[i]->color.blue);
		glTranslatef((*all_spheres)[i]->pos.x,(*all_spheres)[i]->pos.y, (*all_spheres)[i]->pos.z);
		if( (*all_spheres)[i]->active == 0) {
			if( a == 1 ){
				glColor3f( 1.0, 1.0, 1.0);
				glutWireSphere((*all_spheres)[i]->radius, 10, 10);
			}
		}else{
			glutSolidSphere((*all_spheres)[i]->radius, 25, 25);
		}
		glPopMatrix();
	}

}

void draw_dust(std::vector<dust*>* tails) {
	glPushMatrix();
	int j;
	glBegin(GL_POINTS);
	printf("looping through to draw dust: %d\n", tails->size());
	for( j = 0; j < tails->size(); j++) {
		
		double perc = (*tails)[j]->life/10.;
	  	  glColor4f( (*tails)[j]->color.red*perc,
				  	 (*tails)[j]->color.green*perc,
					 (*tails)[j]->color.blue*perc,
					 0.3);
	  	  glVertex3f((*tails)[j]->pos.x, (*tails)[j]->pos.y, (*tails)[j]->pos.z);
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

} // namespace tasp