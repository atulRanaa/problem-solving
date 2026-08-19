#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

void CircleMidPoint(float x0,float y0,float r){
  float x = 0,y = r; //first point
  float pk = 3/2 - r;   
       while(x <= y){
           x++;
           if(pk < 0) pk = pk+2*x+1; 
           else{
               y = y-1;
               pk = pk+2*x+1-2*y;
           }       
           glVertex2f(y+x0,x+y0);
           glVertex2f(x+x0,y+y0);
           glVertex2f(x+x0,-y+y0);
           glVertex2f(y+x0,-x+y0);
           glVertex2f(-y+x0,-x+y0);
           glVertex2f(-x+x0,-y+y0);
           glVertex2f(-x+x0,y+y0);
           glVertex2f(-y+x0,x+y0);
           
       }
       glFlush();
}

void draw(){
  glClear(GL_COLOR_BUFFER_BIT);
  
  glBegin(GL_LINES);
        glVertex2f(-700,0);
        glVertex2f(700,0);
        glVertex2f(0,700);        
        glVertex2f(0,-700);
  glEnd();
  
  float x,y,r;
  printf("Enter two end points of the line to be drawn:");
  scanf("%f %f %f",&x,&y,&r);
  
  glBegin(GL_POINTS);
        CircleMidPoint(x,y,r);
  glEnd();
  glFlush();
}
void Init()
{
  glClearColor(1.0,3.0,0.6,0);
  glColor3f(0.0,0.0,0.0);
  gluOrtho2D(-700, 700 , -700 , 700);
}
void main(int argc, char **argv){ 
 
  glutInit(&argc,argv); 
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
  glutInitWindowPosition(0,0);
  glutInitWindowSize(700,700);
  glViewport(0,0,100,100); 
  glutCreateWindow("Circle Mod Point");  
  Init();  
  glutDisplayFunc(draw);
  glutMainLoop();
}

