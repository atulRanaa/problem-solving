
#include<GL/gl.h>
#include<GL/glut.h>
#include<stdio.h>
#include<math.h>

void printTriangle(float x1,float y1,float x2,float y2,float x3,float y3,float r,float g,float b){
	
	glColor3f(r,g,b);
	glBegin(GL_POLYGON);
			glVertex2f(x1,y1);
			glVertex2f(x2,y2);
			glVertex2f(x3,y3);		
	glEnd();	
	glFlush();
 	
}
void triangleTransform(){	
	glFlush();	
	float x1,c,y1,x2,y2,x3,y3,tx,ty,sx,sy,theta;
	printf("enter the points coordinates");
	scanf("%f",&x1);
	scanf("%f",&y1);
	scanf("%f",&x2);
	scanf("%f",&y2);
	scanf("%f",&x3);
	scanf("%f",&y3);
	
	
	glClear(GL_COLOR_BUFFER_BIT);
	printTriangle(x1,y1,x2,y2,x3,y3,1,1,1);
 	int ch;
 	while(1){
	 	printf("enter your choice:\n1.translation\n2.Scaling\n3.Shearing\n4.rotation about origin\nYour choice: ");
	 	scanf("%d",&ch);
	 	switch(ch){
	 		case 1:
	 		
	 			printf("enter tx and ty ");
	 			scanf("%f",&tx);
	 			scanf("%f",&ty);
	 			printTriangle(x1+tx,y1+ty,x2+tx,y2+ty,x3+tx,y3+ty,0.1,0.2,0.3);
	 			break;
	 		case 2:
	 	
	 			printf("enter sx and sy ");
	 			scanf("%f",&sx);
	 			scanf("%f",&sy);
	 			printTriangle(x1*sx,y1*sy,x2*sx,y2*sy,x3*sx,y3*sy,0.2,0.1,0.3);
	 			break;
	 		case 3:
	 			
	 			printf("enter sx and sy ");
	 			scanf("%f",&sx);
	 			scanf("%f",&sy);
	 			printTriangle(x1+y1*sx,y1+x1*sy,x2+y2*sx,y2+x2*sy,x3+y3*sx,y3+x3*sy,0.3,0.1,0.2);
	 			break;
	 		case 4:
	 			
	 			printf("enter theta in degrees(+ve for anticlockwise) ");
	 			scanf("%f",&theta);
	 			theta=(theta*3.14)/180;
	 			printTriangle(x1*cos(theta)-y1*sin(theta),y1*cos(theta)+x1*sin(theta),
	 					x2*cos(theta)-y2*sin(theta),y2*cos(theta)+x2*sin(theta),
	 					x3*cos(theta)-y3*sin(theta),y3*cos(theta)+x3*sin(theta), 					
	 					0.2,0.3,0.1);
	 			break;
	 		default:
	 			printf("please enter a valid choice!!\n");
	 	
	 	}
	 	
 	}
}
void Init()
{
  glClearColor(0.0,0.5,0.6,0.0);
  gluOrtho2D(-500 ,500 , -500 , 500);
}
void main(int argc, char **argv)
{
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(1024,768);
  glutCreateWindow("Transformations");
  Init();
  glutDisplayFunc(triangleTransform);
  glutMainLoop();
}

