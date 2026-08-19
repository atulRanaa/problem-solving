#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


const int xmin=30;
const int xmax=140;
const int ymin=40;
const int ymax=150;

// Defining region codes
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

double ax,ay,bx,by;

void line(int ax,int ay,int bx,int by)
{
	glBegin(GL_LINES);
	glVertex2f(ax,ay);
	glVertex2f(bx,by);
	glEnd();
	glFlush();
}

int regionCode(int ax,int ay)
{
	int c=0;

	if( ay > ymax)
		c += 8;
	if( ay < ymin)
		c += 4;
	if( ax > xmax)
		c += 2;
	if( ax < xmin)
		c += 1;

	return c;
}

void display (void)
{
 
  	glClear (GL_COLOR_BUFFER_BIT);
	  glColor3f (1.0, 1.0, 1.0);

    line(-500,0,500,0);  // drawing the x-axis
    line(0,-500,0,500); // drawing the y-axis

    // drawing the clipping Region
    glColor3f(0.0,1.0,0.0);
    line(xmin,ymin,xmax,ymin);
    line(xmin,ymin,xmin,ymax);
    line(xmax,ymin,xmax,ymax);
    line(xmax,ymax,xmin,ymax);

    // // displaying the original line
    // glColor3f(1.0,0.0,0.0);
    // line(ax,ay,bx,by);

    int c1 = regionCode(ax,ay);
    int c2 = regionCode(bx,by);

    // calculating the slope

    double slope = (by - ay)/(bx - ax);

    printf("%d\n", c1);
    printf("%d\n", c2);

    int accept = 0;

  	while(1)
  	{
  		if( (c1 == 0) && (c2 == 0))
  		{
  			accept = 1;
  			break;
  		}
  		else if( c1 & c2 )
  			break;
  		else
  		{
  			int code_out;
  			double x,y;

  			// select the the endpoint which is outside the rectangle
  			// select that point

  			if( c1 != 0)
  				code_out = c1;
  			else
  				code_out = c2;

  			// finding the intersection points

  			if( code_out & TOP)
  			{
  				// point is above the clip rectangle
  				x=ax + (ymax - ay)/slope;
  				y=ymax;
  			}
  			else if( code_out & BOTTOM)
  			{
  				// point is below the rectangle
  				x=ax+(ymin - ay)/slope;
  				y=ymin;
  			}
  			else if( code_out & RIGHT)
  			{
  				//point is right side of the rectangle
  				y=ay + slope*(xmax - ax);
  				x=xmax;
  			}
  			else if( code_out & LEFT)
  			{
  				// point is left side of rectangle
  				y=ay + slope*(xmin - ax);
  				x=xmin;
  			}


  			// now intersection point x,y is found
  			//we replace point outside rectangle
  			// by intersection point

  			if( code_out == c1 )
  			{
  				ax=x;
  				ay=y;
  				c1=regionCode(ax,ay);
  			}
  			else
  			{
  				bx=x;
  				by=y;
  				c2=regionCode(bx,by);
  			}
  		}
  	}

  	if( accept )
  		printf("Line accepted\n");
  	else
  		printf("Line rejected\n");

    //displaying the clipped line
    glColor3f(1.0,1.0,1.0);
  	line(ax,ay,bx,by);

  	
 
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
  printf("Enter the endpoints of the line\n");
  scanf("%lf%lf%lf%lf",&ax,&ay,&bx,&by);


	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (100, 100);
	glutCreateWindow (" Clipping ");
	init ();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
