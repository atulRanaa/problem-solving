/*
   left EndPoint to Right EndPoint

*/
	//fabs() is used to get the absolute value
//bresenham's line algorithm 

#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
//#define ROUND(x) ((int)(x+0.5))

int xa,xb,ya,yb;

void display (void)
 {

  	int i,j;
  	float x,y,m,dx,dy,slope,xEnd,yEnd,xBeg,yBeg;
  
  	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
 	glBegin(GL_POINTS);
 	
 	
 	
 	for(i=-800;i<801;i++)
 	{
 		glVertex2f(i,0);
 		glVertex2f(0,i);
 	}
  
    if( xa < xb)
    {
    	xBeg = xa;
    	yBeg= ya;
    	xEnd = xb;
    	yEnd= yb;
    }
    
    else
    {
    	xBeg = xb;
    	yBeg= yb;
    	xEnd = xa;
    	yEnd= ya;
    }
    
    
    dy = yEnd - yBeg;
    dx = xEnd - xBeg;
    
    slope = dy/dx;
    
   
    
   if( slope >= 0 && slope <= 1)
   {
		dy = fabs(yb - ya);
		dx = fabs(xb - xa);
		
		int p = 2*dy - dx;
		
		while( xBeg <= xEnd)
		{
			glVertex2f(xBeg,yBeg);
			
			if( p < 0)
			{
				p=p+2*dy;
			}
			else
			{
				yBeg ++;
				p=p+2*dy-2*dx;
			}
			
			xBeg ++;
		}
   }
   
   else if( slope < 0 && slope >= -1)
   {
	   	dy = fabs(yb - ya);
		dx = fabs(xb - xa);
		
		int p = 2*dy - dx;
		
		while( xBeg <= xEnd)
		{
			glVertex2f(xBeg,yBeg);
			
			if( p < 0)
			{
				p=p+2*dy;
			}
			else
			{
				yBeg --;
				p=p+2*dy-2*dx;
			}
			
			xBeg ++;
    	
    	}
   
   }
   
   else if( slope > 1)
   {
		dy = fabs(yb - ya);
		dx = fabs(xb - xa);
		
		int p = 2*dx - dy;
		
		while( xBeg <= xEnd)
		{
			glVertex2f(xBeg,yBeg);
			
			if( p < 0)
			{
				p=p+2*dx;
			}
			else
			{
				xBeg ++;
				p=p+2*dx-2*dy;
			}
			
			yBeg ++;
			
		}
   }
   
   else if( slope < -1)
   {
   		dy = fabs(yb - ya);
   		dx = fabs(xb - xa);
   		
   		int p = 2*dx - dy;
   		
   		while( xBeg <= xEnd)
   		{
   			glVertex2f(xBeg,yBeg);
   			
   			if( p < 0)
   			{
   				p = p + 2*dx;
   			}
   			else
   			{
   				xBeg ++;
   				p = p + 2*dx - 2*dy;
   			}
   			
   			yBeg--;
   		}
   
   }
 

    glEnd();
    glFlush();
 }
 
 void init(void)
 {
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glOrtho(-700.0, 700.0, -700.0, 700.0, -1.0, 1.0);
 }

int main(int argc, char** argv)
{
 
 printf("Enter the points\n");
 scanf("%d %d %d %d",&xa,&ya,&xb,&yb);
 glutInit(&argc, argv);


 glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
 glutInitWindowSize (500, 500);
 glutInitWindowPosition (100, 100);
 glutCreateWindow ("Simple DDA ");
 init ();
 glutDisplayFunc(display);
 glutMainLoop();
 return 0;
}
