/*
   left EndPoint to Right EndPoint

*/

#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#define ROUND(x) ((int)(x+0.5))

int rx,ry;

void display (void)
 {

  	int i,x,y;
  
 
  	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
 	glBegin(GL_POINTS);
  
    for(i=-500;i<=500;i++)
    {
      glVertex2f(i,0);
      glVertex2f(0,i);
    }
    
    int dry = ry*ry;
    int drx = rx*rx;

    x=0;
    y=ry;
    
    float p1 = dry - drx*ry+(1/4)*drx;

    while( dry*x < drx*y )
    {
    	glVertex2f(x,y);
    	glVertex2f(-x,y);
    	glVertex2f(x,-y);
    	glVertex2f(-x,-y);

    	if( p1 < 0)
    	{
    		p1 = p1 + 2*dry*(x+1) + dry;
    	}
    	else
    	{
    		p1 = p1 + 2*dry*(x+1) - 2*drx*(y-1) + dry;
    		y --;
    	}

    	x++;

    }
    
    float p2 = dry*(x+(1/2))*(x+(1/2)) + drx*(y-1)*(y-1) - drx*dry;

    while( y >= 0 )
    {
    	glVertex2f(x,y);
    	glVertex2f(-x,y);
    	glVertex2f(x,-y);
    	glVertex2f(-x,-y);


    	if( p2 > 0)
    	{
    		p2 = p2 - 2*drx*(y-1) + drx;
    	}
    	else
    	{
    		p2 = p2 + 2*dry*(x+1) - 2*drx*(y-1) + drx;
    		x ++;
    	}

    	y--;

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
 
 printf("\nEnter the horizontal & the vertical radii of the ellipse\n");
 scanf("%d%d",&rx,&ry);
 glutInit(&argc, argv);


 glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
 glutInitWindowSize (500, 500);
 glutInitWindowPosition (100, 100);
 glutCreateWindow (" Circle ");
 init ();
 glutDisplayFunc(display);
 glutMainLoop();
 return 0;
}
