/*
   left EndPoint to Right EndPoint

*/

#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#define ROUND(x) ((int)(x+0.5))

int xa,xb,ya,yb;

void display (void)
 {

  	int i,j;
  	float x,y,m,dx,dy,slope,xEnd,yEnd,xBeg,yBeg;
  
 
  	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
 	glBegin(GL_POINTS);
  
    for(i=-500;i<=500;i++)
    {
      glVertex2f(i,0);
      glVertex2f(0,i);
    }
    
    if( xa < xb )
	{
		xBeg=xa;
		yBeg=ya;
		xEnd=xb;
		yEnd = yb;
	
	}
	else
	{
		xBeg=xb;
		yBeg=yb;
		xEnd = xa;
		yEnd = ya;
	}
	
	dy = yEnd - yBeg;
	dx = xEnd - xBeg;
    
   	m = dy/(float)dx;
    
   	slope = m;
    
    printf("\tslope = %f \n",slope);
     
   
    printf("\nm = %f\n",m);
 
 	
    	
 
	if(slope >=-1 && slope <= 1)
	{
		glVertex2f(ROUND(xBeg),ROUND(yBeg));
		
		printf("\t*****8Initial value***** \n");
		
		printf("\t %f  \t %f \t %f\n",xBeg,yBeg,m);
		
		printf("\n");

		for(i=xBeg ;i<=xEnd ;i++)
		{
			yBeg = yBeg + m;
			xBeg = xBeg + 1;
			glVertex2f(ROUND(xBeg),ROUND(yBeg));
			printf("\t %f  \t %f \t %f\n",xBeg,yBeg,m);
		}				

	}

	else if( slope > 1 )
	{
		glVertex2f(ROUND(xBeg),ROUND(yBeg));

		for(i=yBeg ;i<=yEnd ;i++)
		{
			xBeg = xBeg + (1/m);
			yBeg = yBeg + 1;
			glVertex2f(ROUND(xBeg),ROUND(yBeg));
			printf("\t %f  \t %f \t %f\n",xBeg,yBeg,m);
		}
	}
	else if( slope < -1 )
	{
	
		printf("\t*****8Initial value***** \n");
		
		printf("\t %f  \t %f \t %f\n",xBeg,yBeg,m);
		
		glVertex2f(ROUND(xBeg),ROUND(yBeg));

		for(i=xBeg ;i<=xEnd ;i++)
		{
			xBeg = xBeg - (1/m);  				// since m is negative and x value should be incremented
			yBeg = yBeg - 1;
			glVertex2f(ROUND(xBeg),ROUND(yBeg));
			printf("\t %f  \t %f \t %f\n",xBeg,yBeg,m);
		}	
	
	
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
