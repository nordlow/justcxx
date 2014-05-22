/*!
 * \file t_penrose_tiling.cpp
 * \brief Two-Dimensional Penrose Tiling
 * Drawn recursively using Up-Down generation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>

// Constants.  See http://en.wikipedia.org/wiki/Penrose_tiling
// Heading: Kite and dart tiling (P2) for a diagram of the
// shapes used herein.
const double g_phi   = 1.6180339887498948;
const double g_sin36 = 0.5877852522924731; ///< Sine of 36 Degrees.
const double g_cos36 = 0.8090169943749474; ///< Cosine of 36 Degrees.
const double g_sin72 = 0.9510565162951536; ///< Sine of 72 Degrees.
const double g_cos72 = 0.3090169943749474; ///< Cosine of 72 Degrees.

// Global variables to implement various options: wireframe vs. filled,
// Amman bars, zoom, pan, and numeric labeling.
int fill = 0;
int bars = 0;
int xpos = -10;
int ypos = -0;
int zoom = 25;
int rec = 0;
int numbers = 0;
int kiteLabel = 0;
int dartLabel = 0;

void *font = GLUT_BITMAP_TIMES_ROMAN_10;

// Declare the functions so the C compiler doesn't get confused.
void kiteGen(int);
void dartGen(int);
void dart();
void kite();

// Set up the viewing window: 750 x 750 window with origin at center */
void windowInit()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-50.0, 50.0, -50.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

void output(int x, int y, char *string)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
}

// Draws a "half dart," with provisions for Amman bars and numeric labeling
void halfDart()
{
    glColor3f(1, 0, 0); //Draw darts in red.
    //Here's the juice - the three points that define a triangle that makes up a half-dart
    float dartPoints[6] = {0,0,
                           -static_cast<float>(g_cos72),
                           -static_cast<float>(g_sin72),
                           1,0};
    //Draw the triangle
    glBegin(GL_TRIANGLES);
    glVertex2f(dartPoints[0], dartPoints[1]);
    glVertex2f(dartPoints[2], dartPoints[3]);
    glVertex2f(dartPoints[4], dartPoints[5]);
    glEnd();

    //Draw the Amman bars
    if (bars == 1)
    {
        glLineWidth(3);
        glBegin(GL_LINES);
        glColor3f(1, 0, 1);
        glVertex3f(dartPoints[4]/2, 0.0, 0.0);
        glVertex3f((dartPoints[2]+dartPoints[4])/2, (dartPoints[3]+dartPoints[5])/2, 0.0);
        glColor3f(1, 1, 0);
        glVertex3f(dartPoints[2]/2, dartPoints[3]/2, 0.0);
        glVertex3f(dartPoints[4]/2, 0.0, 0.0);
        glEnd();
        glLineWidth(1);
    }
    //This labels the half-darts - perhaps useful for generating
    //a grid for a Penrose tile cellular automata?!
    if (numbers == 1)
    {
        glPushMatrix();
        glTranslatef(g_cos36*g_phi/2 + 0.25, g_sin36*g_phi/2 - 0.25, 0);
        char c[50];
        sprintf (c, "%d", dartLabel);
        output(0, 0, c );
        dartLabel += 1;
        glPopMatrix();
    }
}

// Draws a "half kite," with provisions for Amman bars and numeric labeling
void halfKite()
{
    glColor3f(0, 0, 1); //Draw kites in blue.
    //Here's the points that define the half-kite triangle.
    float kitePoints[6] = {0,0,g_cos36*g_phi,g_sin36*g_phi,g_cos36*g_phi+(g_cos72),0};
    glBegin(GL_TRIANGLES);
    glVertex2f(kitePoints[0], kitePoints[1]);
    glVertex2f(kitePoints[2], kitePoints[3]);
    glVertex2f(kitePoints[4], kitePoints[5]);
    glEnd();

    //Draw the Amman bars
    if (bars == 1)
    {
        glLineWidth(3);
        glBegin(GL_LINES);
        glColor3f(1, 0, 1);
        glVertex3f(kitePoints[2]/2, kitePoints[3]/2, 0.0);
        glVertex3f(kitePoints[4]/2, 0.0, 0.0);
        glColor3f(1, 1, 0);
        glVertex3f((kitePoints[2]+kitePoints[4])/2, (kitePoints[3]+kitePoints[5])/2 , 0.0);
        glVertex3f(kitePoints[4]/2, 0.0, 0.0);
        glEnd( );
        glLineWidth(1);
    }

    //Label the half-kites with incremental numbers
    if (numbers == 1)
    {
        glPushMatrix();
        glTranslatef(g_cos36*g_phi/2 + 0.25, g_sin36*g_phi/2 - 0.25, 0);
        char c[50];
        sprintf (c, "%d", kiteLabel);
        output(0, 0, c );
        kiteLabel += 1;
        glPopMatrix();
    }
}

// This function deflates a half-kite into a (smaller) whole kite plus a half-dart.
// See http://en.wikipedia.org/wiki/Penrose_tiling heading:
// "Deflation for P2 and P3 tilings" for an explanation of the
// decompositions used here.  There are three if/else statements
// which implement the recursion; each if/else pair corresponds to
// one of the "deflated" (smaller) kites/darts used to construct
// the original half-kite.
void kiteGen(int n)
{
    glPushMatrix();
    glScalef(1/g_phi, 1/g_phi, 0);
    glTranslatef(g_phi+0.5, sqrt(g_phi*g_phi-0.25), 0);
    glRotatef(-108, 0, 0, 1);
    if (n==0) halfKite();
    else kiteGen(n-1);
    glRotatef(180, 1, 0, 0);
    if (n==0) halfKite();
    else kiteGen(n-1);
    glTranslatef(g_cos36*g_phi, g_sin36*g_phi, 0);
    glRotatef(-144, 0, 0, 1);
    glRotatef(180, 0, 1, 0);
    if (n==0) halfDart();
    else dartGen(n-1);
    glPopMatrix();
}

// This function decomposes a half-dart into a (smaller) half-kite
// and a half-dart. The two if/else statements
void dartGen(int n)
{
    glPushMatrix();
    glScalef(1/g_phi, 1/g_phi, 1);
    glTranslatef(g_phi, 0, 0);
    glRotatef(180, 0, 1, 0);
    if (n==0) halfKite();
    else kiteGen(n-1);
    glTranslatef(g_cos36*g_phi, g_sin36*g_phi, 0);
    glRotatef(180, 0, 1, 0);
    glRotatef(144, 0, 0, 1);
    if (n==0) halfDart();
    else dartGen(n-1);
    glPopMatrix();
}

void keyboard(unsigned char key, int x, int y)
{
    //Toggle wire-frame mode.
    if (key == 'w')
    {
        if (fill==0)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(1.0);
            fill = 1;
            glutPostRedisplay();

        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            fill = 0;
            glutPostRedisplay();
        }
    }

    // Toggle Amman bars.
    if (key == 'b')
    {
        if (bars==0)
        {
            bars = 1;
            glutPostRedisplay();

        }
        else
        {
            bars = 0;
            glutPostRedisplay();
        }
    }

    // Toggle numeric labels. Half-kites and half-darts
    // have their own independent counters.
    if (key == 'l')
    {
        if (numbers==0)
        {
            numbers = 1;
            glutPostRedisplay();
        }
        else
        {
            numbers = 0;
            kiteLabel = 0;
            dartLabel = 0;
            glutPostRedisplay();
        }
    }

    // Zoom in.
    if (key == 'a')
    {
        zoom += 25;
        glutPostRedisplay();
    }

    // Zoom out.
    if (key == 'z')
    {
        zoom -= 25;
        glutPostRedisplay();
    }

    // Increase the level of recursion.
    if (key == 'r')
    {
        rec += 1;
        glutPostRedisplay();
    }

    // Decrease recursion.
    if (key == 't')
    {
        if (rec > 0)
        {
            rec -= 1;
            glutPostRedisplay();
        }
    }

    // Exit.
    if (key == 'q') exit(0);
}

// The arrow keys pan the display as expected.
void Special_Keys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT :
        xpos += 5;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT :
        xpos -= 5;
        glutPostRedisplay();
        break;
    case GLUT_KEY_UP :
        ypos -= 5;
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN :
        ypos += 5;
        glutPostRedisplay();
        break;
    }
}

// The display function.  Kites and darts can be combined at this
// level as well, and the boundaries still line up, as long as one
// adheres to the matching rules. Cool!
void display( void )
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(xpos, ypos, 0);
    glScalef(zoom, zoom, 0);
    dartGen(rec);
    glRotatef(180, 1, 0, 0);
    dartGen(rec);

    glPushMatrix();
    glTranslatef(-g_phi, 0, 0);
    glRotatef(-36, 0, 0, 1);
    kiteGen(rec);
    glRotatef(180, 1, 0, 0);
    kiteGen(rec);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-g_phi, 0, 0);
    glRotatef(36, 0, 0, 1);
    kiteGen(rec);
    glRotatef(180, 1, 0, 0);
    kiteGen(rec);
    glPopMatrix();

    glFlush();
}

// Standard OpenGL main-ness.
int main(int argc, char* argv[], char* envp[])
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(750,750);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Penrose!");
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(Special_Keys);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glutDisplayFunc(display);
    windowInit();
    glutMainLoop();
    return 0;
}
