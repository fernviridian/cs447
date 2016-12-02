/*
 * Merry.cpp: A class for drawing a Merry-go-round hierarchical animated model.
 *
 */

#include "Merry.h"
#include "libtarga.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

using namespace std;

// Destructor
Merry::~Merry(void)
{
    if ( initialized )
    {
        glDeleteLists(display_list[0], 1);
        glDeleteLists(display_list[1], 1);
        gluDeleteQuadric(qobj[0]);
        gluDeleteQuadric(qobj[1]);
    }
}

// Initializer. Returns false if something went wrong, like not being able to
// load the texture.
bool
Merry::Initialize(void)
{
    for (int i = 0; i < 2; i++)
    {
        qobj[i] = gluNewQuadric();
        gluQuadricNormals(qobj[i], GLU_SMOOTH);
    }

    display_list[0] = glGenLists(1);
    glNewList(display_list[0], GL_COMPILE);
    //fuck yes http://prideout.net/archive/colors.php
        glColor3f(1.000, 0.078, 0.576);

        /*
	void gluCylinder(	GLUquadric*  	quad,
 	GLdouble  	base,
 	GLdouble  	top,
 	GLdouble  	height,
 	GLint  	slices,
 	GLint  	stacks);

	void gluDisk(	GLUquadric* quad,
 	GLdouble inner,
 	GLdouble outer,
 	GLint slices,
 	GLint loops);
        */

        gluCylinder(qobj[0], 5, 5, 2, 20, 16); // does not include top!!
        glTranslatef(0.0, 0.0, 2);
        // draw top of cylinder face
        gluDisk(qobj[1],0,5,20,20);

        glColor3f(0.686, 0.933, 0.933);
        //draw supporting columns
        // gluCylinder(quadObj, base, top, height, slices, stacks);
        // right
        glTranslatef(0.0, 4.5f, 0);
        gluCylinder(qobj[0], 0.5f, 0.5f, 5, 10, 16);
        // left
        glTranslatef(0.0, -9.0f, 0);
        gluCylinder(qobj[0], 0.5f, 0.5f, 5, 10, 16);
        //front
        glTranslatef(0.0, 4.5f, 0);
        glTranslatef(4.5, 0.0f, 0);
        gluCylinder(qobj[0], 0.5f, 0.5f, 5, 10, 16);
        //back
        glTranslatef(-9.0, 0.0f, 0);
        gluCylinder(qobj[0], 0.5f, 0.5f, 5, 10, 16);

        // draw lower disk on top 
        glColor3f(1.000, 0.078, 0.576);
        glTranslatef(4.5f, 0.0f, 5);
        gluDisk(qobj[0], 0,5,20,20);

        //draw roof cylinder
        gluCylinder(qobj[0], 5, 5, 2, 20, 16);
        
 
        //draw conical roof
        glTranslatef(0.0f, 0.0f, 2.0f);
        gluCylinder(qobj[0], 5, 0, 2, 10, 16);

    glEndList();

    display_list[1] = glGenLists(1);
    glNewList(display_list[1], GL_COMPILE);
        glColor3f(0, 0, 1);
        glRotated(90,1,0,0);
        glutSolidTeapot(1.5f);
    glEndList();

    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;

    return true;
}


// Draw just calls the display list we set up earlier.
void
Merry::Draw(void)
{
    glDisable(GL_CULL_FACE);
    glPushMatrix();
    
    // Translate the merry-go-round to the point
    glTranslatef(posn[0], posn[1], posn[2]);

    //glCallList(display_list[0]);

    //for (int i = 0; i < 4; i++) {
        glPushMatrix();
   //     glTranslatef(0,0,1);
   //     glTranslatef(3*cos(baseAngle+((3.14159f/2)*i)),3*sin(baseAngle+((3.14159f/2)*i)),0);
        glRotatef(tpAngle*30,0,0,1);
        glCallList(display_list[0]);
        glPopMatrix();
    //}

    glPopMatrix();
    glEnable(GL_CULL_FACE);
}

void
Merry::Update(float angle)
{
    baseAngle += angle;
    tpAngle += angle*2.0f;
}
