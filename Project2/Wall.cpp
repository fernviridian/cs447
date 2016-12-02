#include "Wall.h"
#include "libtarga.h"
#include <stdio.h>
#include <OpenGL/glu.h>

#include <iostream>
using namespace std;

Wall::~Wall(void)
{
    if ( initialized )
    {
        glDeleteLists(display_list, 1);
        gluDeleteQuadric(qobj);
    }
}


bool Wall::Initialize(void)
{

// shit from track.cpp
    // image texture based crap.
    ubyte   *image_data;
    int	    image_height, image_width;

    // Load the image for the texture. The texture file has to be in
    // a place where it will be found.
    if ( ! ( image_data = (ubyte*)tga_load("./wall.tga", &image_width,
					   &image_height, TGA_TRUECOLOR_24) ) )
    {
	fprintf(stderr, "Ground::Initialize: Couldn't load grass.tga\n");
	return false;
    }

    // This creates a texture object and binds it, so the next few operations
    // apply to this texture.
    glGenTextures(1, &texture_obj);
    glBindTexture(GL_TEXTURE_2D, texture_obj);

    // This sets a parameter for how the texture is loaded and interpreted.
    // basically, it says that the data is packed tightly in the image array.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // This sets up the texture with high quality filtering. First it builds
    // mipmaps from the image data, then it sets the filtering parameters
    // and the wrapping parameters. We want the grass to be repeated over the
    // ground.
    gluBuild2DMipmaps(GL_TEXTURE_2D,3, image_width, image_height, 
		      GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		    GL_NEAREST_MIPMAP_LINEAR);

    // This says what to do with the texture. Modulate will multiply the
    // texture by the underlying color.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 


    // Set up the train. At this point a cube is drawn. NOTE: The
    // x-axis will be aligned to point along the track. The origin of the
    // train is assumed to be at the bottom of the train.
    display_list = glGenLists(1);
    glNewList(display_list, GL_COMPILE);
    glColor3f(1.0, 1.0, 1.0); //white
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_obj);
    glBegin(GL_QUADS);
        // top
	glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0, 0.0); // texture
	glVertex3f(-width/2, length/2, height);
        glTexCoord2f(0.0, 1.0); // texture
	glVertex3f(-width/2, -length/2, height);
        glTexCoord2f(1.0, 1.0); //texture
	glVertex3f(width/2, -length/2, height);
        glTexCoord2f(1.0, 0.0); //texture
	glVertex3f(width/2, length/2, height); // problem coordinate
  
       //bottom
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0, 0.0); //texture
	glVertex3f(width/2, length/2, 0.0f); // problem coordinate
        glTexCoord2f(1.0, 1.0); //texture
	glVertex3f(width/2, -length/2, 0.0f);
        glTexCoord2f(0.0, 1.0); // texture
	glVertex3f(-width/2, -length/2, 0.0f);
        glTexCoord2f(0.0, 0.0); // texture
	glVertex3f(-width/2, length/2, 0.0f);



        // fix texture scaling problems
        // left
	glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0, 0.0); // texture
	glVertex3f(-width/2, -length/2, 0.0f);

        glTexCoord2f(0.0, 4.0); // texture
	glVertex3f(-width/2, -length/2, height);

        glTexCoord2f(4.0, 4.0); //texture
	glVertex3f(-width/2, length/2, height);

        glTexCoord2f(4.0, 0.0); //texture
	glVertex3f(-width/2, length/2, 0.0f);

        // right
	glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(4.0, 0.0); //texture
	glVertex3f(width/2, length/2, 0.0f);
        glTexCoord2f(4.0, 4.0); //texture
	glVertex3f(width/2, length/2, height);
        glTexCoord2f(0.0, 4.0); // texture
	glVertex3f(width/2, -length/2, height);
        glTexCoord2f(0.0, 0.0); // texture
	glVertex3f(width/2, -length/2, 0.0f);

        // front
	glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(4.0, 0.0); //texture
	glVertex3f(width/2, -length/2, 0.0f);
        glTexCoord2f(4.0, 4.0); //texture
	glVertex3f(width/2, -length/2, height);
        glTexCoord2f(0.0, 4.0); // texture
	glVertex3f(-width/2, -length/2, height);
        glTexCoord2f(0.0, 0.0); // texture
	glVertex3f(-width/2, -length/2, 0.0f);

        //back
	glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(4.0, 0.0); // texture
	glVertex3f(-width/2, length/2, 0.0f);
        glTexCoord2f(4.0, 4.0); // texture
	glVertex3f(-width/2, length/2, height);
        glTexCoord2f(0.0, 4.0); //texture
	glVertex3f(width/2, length/2, height);
        glTexCoord2f(0.0, 0.0); //texture
	glVertex3f(width/2, length/2, 0.0f);
        

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glEndList();

    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;

    return true;
}


// Draw just calls the display list we set up earlier.
void Wall::Draw(void)
{
    glPushMatrix();
    
    // Translate the tree to the point
    glTranslatef(posn[0], posn[1], posn[2]);

    glCallList(display_list);
    glPopMatrix();
}

