/*
 * Rafael Ayala
 * Press a to animate, esc to exit
 */
//#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#define PI 3.14159

float x1,y,z1,x2;
float s;
float radius = 8;
int switch1 = 0; //boolean to make the animation oscillate
double ini_time;
struct timeval tiempo;
struct timeval t_current;

float left,right,top,bottom;
static void init(void)
{
    x1 = -10.0;
    x2 = 50.0;
    y = 0.0;
    z1 = 0.0;
    glEnable(GL_DEPTH_TEST);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);
    glMatrixMode (GL_PROJECTION);
    //gluPerspective(60.0, 1.0, 1.0,70);
    glOrtho(-60.0,60.0,-60.0,60.0,0.0,80.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt (20.0, 0.0, 60.0, 20.0, 0.0,0.0, 0.0, 1.0, 0.0);
    //srand(time(NULL));
}

void display(void)
{
    
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0,0.0,0.0);
    
    glPushMatrix();
    glTranslatef(x1,y,z1);
    //glScalef(scalex,scaley,scalez);
    glutSolidSphere(radius,25,25);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(x2,y,z1);
    //glScalef(scalex,scaley,scalez);
    glutSolidSphere(radius,25,25);
    glPopMatrix();
    
    glColor3f(0.0,1.0,0.0);
    glBegin(GL_LINES);
    glVertex3f(20.0,-15.0,0.0);
    glVertex3f(20.0,15.0,0.0);
    glEnd();
    
    glutSwapBuffers();
}

void animate(void)
{
    float p = (rand() % 101) /100;
    if (switch1){ //collision
        //printf("%f\n",x1);
        if(radius > 0){
            if (p < 0.6){
                radius -= 0.0001;
            }
        }
        x1 -=0.01;
        x2 +=0.01;
        //printf("HUUURRR\n");
        //Start easing in 
        if(x1 <= -10.0){
            switch1= 0;
            printf("SWITCH %f\n",x1);
        }
        //glutIdleFunc(NULL);
    } else {
        if(radius > 0){
            if (p < 0.6){
                 radius -= 0.0001;
            }
        }
        if (x1 >= 12.0){
            switch1 = 1;
            printf("SWITCH %f\n",x1);
            
            
        }
        x1 += 0.01;
        x2 -= 0.01;
        
        
    }
    glutPostRedisplay();
    //printf(%);
    //printf("%f\n",radius);
        
    
}

void keyboard(unsigned char key, int x, int y)
{
	if(key == 'a'){
		glutIdleFunc(animate);
	} else if (key == 27){
		exit(0);
	} else if (key == 'r'){
        glutIdleFunc(NULL);
        printf("%.42f\n",x1);
        x1 = -10.0;
        x2 = 50.0;
        radius = 8;
        glutPostRedisplay();
        //glutIdleFunc(animate);
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow(argv[0]);
    init();
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    //glutIdleFunc(animate);
    glutMainLoop();
    return 0;
}
