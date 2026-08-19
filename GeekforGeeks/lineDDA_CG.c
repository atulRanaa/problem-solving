#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

float X1, Y1, X2, Y2;

float round_value(float v){
  return (int)(v + 0.5);
}

void LineDDA(void){

  float dx=(X2-X1);
  float dy=(Y2-Y1);
  float steps;
  float xInc,yInc,x=X1,y=Y1;
  
  steps=(abs(dx)>abs(dy))?(abs(dx)):(abs(dy));
  xInc=dx/steps;
  yInc=dy/steps;

 
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_POINTS);
  glVertex2d(x,y);
  int k;
  for(k=0;k<steps;k++){
    x+=xInc;
    y+=yInc;
    glVertex2d(round_value(x), round_value(y));
  }
  glEnd();
  glFlush();
}
void reshape(int width,int height){
  glViewport(0,0,width,height);
}
void Init()
{
  glClearColor(1.0,3.0,0.6,0);
  glColor3f(0.0,0.0,0.0);
  gluOrtho2D(-700, 700 , -700 , 700);
}
void main(int argc, char **argv){

  printf("Enter two end points of the line to be drawn:\n");
  printf("Enter Point1( X1 , Y1):");
  scanf("%f %f",&X1,&Y1);
  printf("Enter Point1( X2 , Y2):");
  scanf("%f %f",&X2,&Y2);
  
  
  glutInit(&argc,argv); 
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
  glutInitWindowPosition(0,0);
  glutInitWindowSize(1000,1000); 
  glutCreateWindow("DDA_Line");  
  Init();  
  glutDisplayFunc(LineDDA);
  glutReshapeFunc(reshape);
  glutMainLoop();
}



