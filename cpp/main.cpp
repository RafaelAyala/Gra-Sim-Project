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

// Our Libraries
//#include "tasp_sphere.h"
#include "tasp_collision_dar.h"
#include "tasp_draw_functions.h"
using namespace tasp;

// user defined values
// TODO : CLI parameters
#define DECAY_PROB 0.5
#define BALL_SPEED 2.0 // ASU's per second
#define PI 3.14159
#define DENSITY 1.0

#define CUBE_LENGTH 5.0

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// global variables

const int balls(1);		// initial number of balls in the system

int window; 			// id of the window
double current(0.0); 		// stores time
double mass_of_system(0.0);
int response(0);

// camera controls
double angleX = 0.0;
double angleY = 0.0;
double lx = -10.0, ly = -10.0, lz = -10.0; // for keyboard movement
double x = 10.0, y = 10.0, z = 10.0; // for keyboard movement
double deltaAngleX = 0.0;
double deltaAngleY = 0.0;
double deltaMove = 0.0;
int xOrigin = -1;
int yOrigin = -1;
// end camera controls

// sphere next_ball;
double next_ball_mass;
double next_ball_radius;

int showText = 0; 

int dust_shown = 1;
int reflection = 1;

std::vector<Sphere*> all_spheres;
std::vector<dust> tails;

/*
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


		if( all_spheres[j]->radius>0.0) {
			if( decay <= DECAY_PROB && all_spheres[j]->active) {
				mass_before = all_spheres[j]->get_mass();
				all_spheres[j]->radius -= 0.00045;
				mass_of_system += ( mass_before - all_spheres[j]->get_mass());
				
				if(dust_shown) {
					 dust tail;
					double rad = all_spheres[j]->radius;
					tail.pos.x = all_spheres[j]->pos.x + 
						(((double) rand() / RAND_MAX) * (2*rad) - rad);
					tail.pos.y = all_spheres[j]->pos.y + 
						(((double) rand() / RAND_MAX) * (2*rad) - rad);
					tail.pos.z = all_spheres[j]->pos.z + 
						(((double) rand() / RAND_MAX) * (2*rad) - rad);
					tail.color = all_spheres[j]->color;
					tail.life = 75;
							
					tails.resize(tails.size()+1);
					tails.push_back(tail);
				}
			}
		}else{
			all_spheres.erase(all_spheres.begin()+j);
			break;
		}
		// END DECAY

		if(all_spheres[j]->path == 0) { //linear paths
			// advance position on vector
			all_spheres[j]->move_on_vector(current);
		
		}else if(all_spheres[j]->path == 1) { // bezier curves for path	
			
			if( all_spheres[j]->interval < 1.0) {
				// advance position on curve
				all_spheres[j]->move_on_curve(current);
			} else { 
				// generate a new curve
				all_spheres[j]->generate_curve();		
			}
		}	

	
	}
	// set window and call display to refresh screen
	glutSetWindow(window);
	glutPostRedisplay();
}

/*
 * checks if the system has enough mass to spawn the new ball
 */
void spawn_next_ball() {
	if( mass_of_system >= next_ball_mass ) {
		all_spheres.push_back(  new Sphere(next_ball_radius) );
		mass_of_system -= next_ball_mass; 
		next_ball_radius = random_radius();
		next_ball_mass = (4 * PI * pow(next_ball_radius,3))/3.;
	}
}

/*
 * 
 */ 
void keyMove (double deltaMove ) {
	x += deltaMove * lx * 0.1;
	y += deltaMove * ly * 0.1;
	z += deltaMove * lz * 0.1;
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
	collision_check(all_spheres);
	
	
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
		draw_spheres(all_spheres, 0);
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
		draw_spheres(all_spheres, 1);
		draw_dust(tails);
		glDisable(GL_LIGHTING);
		draw_box();
	// END DRAW SCENE

	glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

	if(showText){
		glColor3f( 1.0f, 1.0f, 1.0f);
		glRasterPos3f(x+lx,y+ly,z+lz);
		char blah[20];
		sprintf(blah,"%d - %2.0f",all_spheres.size(), mass_of_system/next_ball_mass*100);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)blah);
	}
	glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
	
	spawn_next_ball();
	
	glutSwapBuffers();
}

/*
 * initializes the system prior to animating
 */
void gfxinit() {
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

	glEnable ( GL_COLOR_MATERIAL ) ;
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glShadeModel(GL_SMOOTH);

	srand(time(NULL));	// seed for rand() calls

	// create all spheres for the initial system state
	int k;
	for( k = 0; k < balls; k++ ) { //setup all ball settings
		all_spheres.push_back( new Sphere(random_radius()) );
	}
	next_ball_radius = random_radius();
	next_ball_mass = (4 * PI * pow(next_ball_radius,3))/3.;
}

/*
 * The keystroke function handles user input to modify how the animation
 * performs. Also allows the user to quit
 */
void keystroke(unsigned char c, int x, int y) {
	switch(c) {
		case 97:	// [a] for add ball
		{
			all_spheres.push_back( new Sphere(random_radius()) );
		}
			break;
		case 99:	// [c] collisions response mode
			response = (response == 0) ? 1 : 0;
			set_response(response);
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
 * Simply sets up the OpenGL framework, calls functions to set up and begin
 * the animation.
 */
int main(int argc, char **argv) {
	// TODO : get rid of these setters
	set_cube_length(CUBE_LENGTH);
	set_ball_speed(BALL_SPEED);
	set_response(response);
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