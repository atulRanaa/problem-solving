#include<stdio.h>
#include<GL/glut.h>
#include<GL/gl.h>

void display(){
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(0,0,0);  
  glBegin(GL_POLYGON);
    glVertex2d(200,200);
    glVertex2d(200,-200);
    glVertex2d(-200,-200);
    glVertex2d(-200,200);
  glEnd();
  glFlush();
}
void init(){
  glClearColor(1,1,1,0);
  gluOrtho2D(-500,500,-500,500);
}
int main(int argc,char** argv){
 
 glutInit(&argc,argv);
 glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
 glutInitWindowPosition(100,100);
 glutInitWindowSize(600,600);
 glutCreateWindow("Window");
 init();
 glutDisplayFunc(display);
 glutMainLoop();
 return 0;
}
