#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


double xa,ya,xb,yb,xc,yc,xd,yd;

void line(int ax,int ay,int bx,int by)
{
	glBegin(GL_LINES);
	glVertex2f(ax,ay);
	glVertex2f(bx,by);
	glEnd();
	glFlush();
}

double square(double a){ return (a*a);}
double cube(double a){ return (a*a*a);}

void display (void)
{
 
  	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (0.0, 1.0, 1.0);
	
	double x,y,v1,v2,v3,v4,u=0.0;
	
	for(u=0.0;u<1.0;u=u+0.0001)
	{
		v1 = cube(1-u);
		v2 = 3*u*square(1-u);
		v3=  3*square(u)*(1-u);
		v4=  cube(u);
		
		x=(xa*v1)+(xb*v2)+(xc*v3)+(xd*v4);
		y=(ya*v1)+(yb*v2)+(yc*v3)+(yd*v4);
		
		glBegin(GL_POINTS);
		glColor3f (0.0, 1.0, 1.0);
		glVertex2f(x,y);
		glFlush();
	}

  	
 
    glEnd();
    glFlush();
}
 
void init(void)
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glOrtho(-500.0, 500.0, -500.0, 500.0, -1.0, 1.0);
}

int main(int argc, char** argv)
{
	printf("Enter the four control points of the cubic bezier curve\n");
	scanf("%lf%lf%lf%lf%lf%lf%lf%lf",&xa,&ya,&xb,&yb,&xc,&yc,&xd,&yd);


	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (100, 100);
	glutCreateWindow (" Bezier Curve ");
	init ();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

