#include "Tree.h"
#include "libtarga.h"
#include <stdio.h>
#include <OpenGL/glu.h>

Tree::~Tree(void)
{
    if ( initialized )
    {
        glDeleteLists(display_list, 1);
        gluDeleteQuadric(qobj);
    }
}


bool Tree::Initialize(void)
{
    qobj = gluNewQuadric();
    gluQuadricNormals(qobj, GLU_SMOOTH);

    display_list = glGenLists(1);
    glNewList(display_list, GL_COMPILE);
        // draw tree base, trunk
        glColor3f(0.54, 0.27, 0.07);
        gluCylinder(qobj, base/5, base/5, height/3, 10, 16);
        glTranslatef(0.0, 0.0, height/4);

        // draw the green part of the tree
        glColor3f(0.0, 1.0, 0.0);
        //cone is cylinder with top = 0
        int tree_components = 4;
        int tree_height = height/2; // tree cone height
        int spacing = 1;
        // start at top
        glTranslatef(0.0, 0.0, (height/8)*tree_components+1);
        // assumes equal sized tree components
        for(int i = tree_components+1; i > 1; i--){
          // build from the top down method....for teh lolz
          // stack another tree section on here.
          // gluCylinder(quadObj, base, top, height, slices, stacks);
          gluCylinder(qobj, base*2-((float)i/1.5), 0, tree_height, 10, 16);
          glTranslatef(0.0, 0.0, -(tree_height/2));
        }

    glEndList();

    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;

    return true;
}


// Draw just calls the display list we set up earlier.
void Tree::Draw(void)
{
    glPushMatrix();
    
    // Translate the tree to the point
    glTranslatef(posn[0], posn[1], posn[2]);

    glCallList(display_list);
    glPopMatrix();
}

