/*
   Circle's center can be at any place 

*/

#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#define ROUND(x) ((int)(x+0.5))

int r,xc,yc;

void display (void)
{

    int i,j,p,x,y;
  
 
  glClear (GL_COLOR_BUFFER_BIT);
  glColor3f (1.0, 1.0, 1.0);
  
  glBegin(GL_POINTS);
  
    for(i=-500;i<=500;i++)
    {
      glVertex2f(i,0);
      glVertex2f(0,i);
    }
    
    p=1-r;

    x=0;
    y=r;

    while( x < y )
    {
      glVertex2f(x + xc,y+yc);
      glVertex2f(y+xc,x+yc);
      glVertex2f(y+xc,-x + yc);
      glVertex2f(x+xc,yc-y);
      glVertex2f(-y+xc,x+yc);
      glVertex2f(-x+xc,y+yc);
      glVertex2f(-x+xc,-y+yc);
      glVertex2f(-y+xc,-x+yc);


      if( p < 0)
      {
        p = p + 2*x + 3;
      }
      else
      {
        p = p + 2*x - 2*y + 5;
        y --;
      }

      x++;

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
 
 printf("Enter the center and the radius of the circle\n");
 scanf("%d%d%d",&xc,&yc,&r);
 glutInit(&argc, argv);


 glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
 glutInitWindowSize (700, 700);
 glutInitWindowPosition (100, 100);
 glutCreateWindow (" Circle ");
 init ();
 glutDisplayFunc(display);
 glutMainLoop();
 return 0;
}
