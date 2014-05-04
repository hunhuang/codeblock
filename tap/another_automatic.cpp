#include <GL/gl.h>
#include <GL/glu.h>
#include <glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "helper1.h"
int texImageWidth;
int texImageHeight;
int window;
static GLuint texName;				//texture name
int anglex= 0, angley = 0, anglez = 0;		//rotation angles

char maps[] = {"stone_text.bmp"};		//texture file

//load texture image
GLubyte *makeTexImage( char *loadfile )
{
   int i, j, c, width, height;
   GLubyte *texImage;

   /*
     Only works for .png or .tif images.  NULL is returned if errors occurred.
     loadImageRGA() is from imageio library downloaded from Internet.
   */

   LoadBitmap1( (char *) loadfile,texImage,
                           width, height);
   texImageWidth = width;
   texImageHeight = height;

   return texImage;
}

void init(void)
{
   float light_ambient[4] = { 0.1, 0.1, 0.1, 1.0 }; 	// r, g, b, a
   float light_diffuse[4] = { 1.0, 1.0, 0.9, 1.0 }; 	// r, g, b, a
   //directional used
   float light_position[4] = { 1.0, 1.0, 1.0 , 0.0 }; 	// x, y, z, w

   glClearColor (0, 0, 0, 0.0);				//black
   glShadeModel(GL_FLAT);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_LIGHTING);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glEnable(GL_LIGHT0);


   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   //texName is global
   glGenTextures(1, &texName);
   // enable automatic texture coordinate generation
   GLubyte *texImage;
   LoadBitmap1(maps, texImage, texImageWidth, texImageHeight);
   if ( !texImage ) {
      printf("\nError reading %s \n", maps );
      return;
   }

    glBindTexture(GL_TEXTURE_2D, texName);		//now we work on texName
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //mix with light
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texImageWidth,
               texImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage);

    delete texImage;					//free memory holding texture image

   //automatic texture coordinates generation
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);
   glEnable(GL_TEXTURE_2D);
}


void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();
   glRotatef( anglex, 1.0, 0.0, 0.0);		//rotate along x-axis
   glRotatef( angley, 0.0, 1.0, 0.0);		//rotate along y-axis
   glRotatef( anglez, 0.0, 0.0, 1.0);		//rotate along z-axis

   glutSolidTorus(1.0, 2, 30,30);
   glPopMatrix();
   glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
  switch(key) {
    case 'x':
      anglex = ( anglex + 3 ) % 360;
      break;
    case 'X':
      anglex = ( anglex - 3 ) % 360;
      break;
    case 'y':
      angley = ( angley + 3 ) % 360;
      break;
    case 'Y':
      angley = ( angley - 3 ) % 360;
      break;
    case 'z':
      anglez = ( anglez + 3 ) % 360;
      break;
    case 'Z':
      anglez = ( anglez - 3 ) % 360;
      break;
    case 'r':
      anglex = angley = anglez = 0;
      break;
    case 27: /* escape */
        glutDestroyWindow(window);
        exit(0);
  }
  glutPostRedisplay();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
    glOrtho(-4.0, 4.0, -4.0 * (GLfloat) h / (GLfloat) w,
            4.0 * (GLfloat) h / (GLfloat) w, -10.0, 10.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt ( 5, 0, 0, 0, 0, 0, 0, 0, 1 );
}


int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(500, 500);
   glutInitWindowPosition(100, 100);
   window = glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}
