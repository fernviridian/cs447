/*
 * Walkway.h: Header file for a class that draws the ground.
 *
 * (c) 2001-2002: Stephen Chenney, University of Wisconsin at Madison.
 */


#ifndef _WALKWAY_H_
#define _WALKWAY_H_

#include <OpenGL/glu.h>
#include <Fl/gl.h>

class Walkway {
  private:
    GLubyte display_list;   // The display list that does all the work.
    GLuint  texture_obj;    // The object for the grass texture.
    bool    initialized;    // Whether or not we have been initialised.
    GLdouble    width;
    GLdouble    length;
    float       posn[3];



  public:
    // Constructor. Can't do initialization here because we are
    // created before the OpenGL context is set up.
    Walkway(void) { display_list = 0; initialized = false; width = 20.0; length = 20.0; posn[0] = posn[1] = posn[2] = 0.0f; };
    Walkway(GLdouble my_width, GLdouble my_length, float x, float y, float z) { 
        display_list = 0; initialized = false; 
        length = my_length;
        width = my_width;

        posn[0] = x;
        posn[1] = y;
        posn[2] = z;
    };


    // Destructor. Frees the display lists and texture object.
    ~Walkway(void);

    // Initializer. Creates the display list.
    bool    Initialize(void);

    // Does the drawing.
    void    Draw(void);
};


#endif
