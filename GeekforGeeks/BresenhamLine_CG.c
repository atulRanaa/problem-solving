#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>

#define sign(x) ( (x>0) ? 1 : ((x<0)?-1:0) )
void bres_general(int x1, int y1, int x2, int y2){

  int swap=0, temp;
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int s1 = sign(x2-x1);
  int s2 = sign(y2-y1);
  /* Check if dx or dy has a greater range */
  /* if dy has a greater range than dx swap dx and dy */
  if(dy > dx){temp = dx; dx = dy; dy = temp; swap = 1;}
  /* Set the initial decision parameter and the initial point */
  int d = 2 * dy - dx;
  int x = x1;
  int y = y1;

  int i;
  for(i = 1; i <= dx; i++){
    glVertex2i(x,y);        
      while(d >= 0){
        if(swap) x = x + s1;
        else y = y + s2;
        d = d - 2* dx;
      }
    if(swap) y = y + s2;
    else x = x + s1;    
    d = d + 2 * dy;
  }
  glFlush();
}


void draw(){
  glClear(GL_COLOR_BUFFER_BIT);
  
  glBegin(GL_LINES);
        glVertex2f(-500,0);
        glVertex2f(500,0);
        glVertex2f(0,500);        
        glVertex2f(0,-500);
  glEnd();
  
  glBegin(GL_POINTS);
        bres_general(400,-400,40,500);
  glEnd();
  glFlush();
}
void Init()
{
  glClearColor(1.0,1.0,1.0,0);
  glColor3f(0.0,0.0,0.0);
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(-500 , 500 , -500 , 500);
}

int main(int argc , char **argv){
    
  glutInit(&argc,argv); 
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
  glutInitWindowPosition(0,0);
  glutInitWindowSize(640,680); 
  glutCreateWindow("BRESENHUM LINE ALGORITHM");  
  Init();  
  glutDisplayFunc(draw);
  glutMainLoop();

}
