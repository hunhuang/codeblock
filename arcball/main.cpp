#include <windows.h>
#include <glut.h>
#include <stdio.h>
#include "vector.h"
#define PI 3.1416
int last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;

int arcball_on = false;
int pan_on =false;

int screen_height, screen_width;
float angle;
Vector3 rotation;
Vector3 translation(0,0,0);
GLfloat pos[]= {0.7,0.7,0,0};
GLfloat rotMat[]={1,0,0,0,
                    0,1,0,0, 0,0,1,0, 0,0,0,1};
void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //glEnable(GL_DEPTH_TEST);
}
void drawAxis()
{
    glDisable(GL_LIGHTING);
    glViewport(0,0,50,50);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(rotMat);
    glBegin(GL_LINES);
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);

    glColor3f (0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);

    glColor3f (1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);

    glEnd();

    glEnable(GL_LIGHTING);

}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, screen_width, screen_height);
    glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    glFrustum(-1,1,-1,1,-1,1);

    glMatrixMode(GL_MODELVIEW);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLoadIdentity();
    glTranslated(translation.x, translation.y, translation.z);
    glMultMatrixf(rotMat);
    glColor3f(1,1,0);
    glutWireTeapot(0.5);

    drawAxis();

    glFlush();
}
void reshape(int w, int h)
{


    screen_height= h;
    screen_width=w;

}
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
        GLfloat matrix[16];
        glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
        for (int i=0; i<4;i++)
        {
            for (int j=0;j<4;j++)
            printf("%4.4f ", matrix[i*4+j]);
            printf("\n");
        }
        break;
    case 'x':
        exit(0);
    default:
        break;
    }
}

Vector3 get_arcball_vector(int x, int y)
{
    Vector3 P = Vector3(1.0*x/screen_width*2 - 1.0,
                        1.0*y/screen_height*2 - 1.0,
                        0);
    P.y = -P.y;
    float OP_squared = P.x * P.x + P.y * P.y;
    if (OP_squared <= 1*1)
        P.z = sqrt(1*1 - OP_squared);  // Pythagore
    else
        P.Normalize(); // nearest point
    return P;
}
//angle is in radian
void computeAngleAndAxis(Vector3 * rotation, float * angle)
{
    if (cur_mx != last_mx || cur_my != last_my)
    {
        Vector3 va = get_arcball_vector(last_mx, last_my);
        Vector3 vb = get_arcball_vector( cur_mx,  cur_my);
        *angle = acos(va.dotProduct(vb))*180.0/PI;
        *rotation = va.crossProduct(vb);

    }

}

void onMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            last_mx  = x;
            last_my  = y;
            arcball_on = true;

        }
        else if (state== GLUT_UP)
        {

            arcball_on=false;
        }
    }
    if (button ==GLUT_RIGHT_BUTTON)
    {
        if (state== GLUT_DOWN)
        {
            last_mx = x;
            last_my = y;
            pan_on=true;

        }
        else
        {
            pan_on=false;
        }

    }


}

void onMotion(int x, int y)
{

    if (arcball_on)    // if left button is pressed
    {
        cur_mx = x;
        cur_my = y;
        computeAngleAndAxis(&rotation, &angle);

        glPushMatrix();
        glLoadIdentity();
            glMultMatrixf(rotMat);
            glRotated(angle, rotation.x, rotation.y, rotation.z);
            glGetFloatv (GL_MODELVIEW_MATRIX, rotMat);
        glPopMatrix();
        glutPostRedisplay();
    }
    if (pan_on)
    {
        cur_mx = x;
        cur_my = y;
        translation.x+=(cur_mx- last_mx)*0.1/screen_width;
        translation.y+= -(cur_my-last_my)*0.1/screen_height;
        glutPostRedisplay();

    }
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);

    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMotion);
    glutMainLoop();
    return 0;
}
