/*
 * Track.cpp: A class that draws the train and its track.
 *
 * (c) 2001-2002: Stephen Chenney, University of Wisconsin at Madison.
 */


#include "Track.h"
#include <stdio.h>
#include <FL/math.h>
#include "libtarga.h"
#include <OpenGL/glu.h>


// The control points for the track spline.
const int   Track::TRACK_NUM_CONTROLS = 4;
const float Track::TRACK_CONTROLS[TRACK_NUM_CONTROLS][3] =
		{ { -20.0, -20.0, -18.0 }, { 20.0, -20.0, 40.0 },
		  { 20.0, 20.0, -18.0 }, { -20.0, 20.0, 40.0 } };

// The carriage energy and mass
const float Track::TRAIN_ENERGY = 150.0f;


// Normalize a 3d vector.
static void
Normalize_3(float v[3])
{
    double  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    if ( l == 0.0 )
	return;

    v[0] /= (float)l;
    v[1] /= (float)l;
    v[2] /= (float)l;
}


// Destructor
Track::~Track(void)
{
    if ( initialized )
    {
	glDeleteLists(track_list, 1);
	glDeleteLists(train_list, 1);
    }
}


// Initializer. Would return false if anything could go wrong.
bool
Track::Initialize(void)
{
    CubicBspline    refined(3, true);
    int		    n_refined;
    float	    p[3];
    int		    i;

    // Create the track spline.
    track = new CubicBspline(3, true);
    for ( i = 0 ; i < TRACK_NUM_CONTROLS ; i++ )
	track->Append_Control(TRACK_CONTROLS[i]);

    // Refine it down to a fixed tolerance. This means that any point on
    // the track that is drawn will be less than 0.1 units from its true
    // location. In fact, it's even closer than that.
    track->Refine_Tolerance(refined, 0.1f);
    n_refined = refined.N();

    // Create the display list for the track - just a set of line segments
    // We just use curve evaluated at integer paramer values, because the
    // subdivision has made sure that these are good enough.
    track_list = glGenLists(1);
    glNewList(track_list, GL_COMPILE);
	glColor3f(1.0f, 1.0, 1.0f);
	glBegin(GL_LINE_STRIP);
	    for ( i = 0 ; i <= n_refined ; i++ )
	    {
		refined.Evaluate_Point((float)i, p);
		glVertex3fv(p);
	    }
	glEnd();
    glEndList();


    // image texture based crap.
    ubyte   *image_data;
    int	    image_height, image_width;

    // Load the image for the texture. The texture file has to be in
    // a place where it will be found.
    if ( ! ( image_data = (ubyte*)tga_load("./bee.tga", &image_width,
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
    train_list = glGenLists(1);
    glNewList(train_list, GL_COMPILE);
    glColor3f(1.0, 1.0, 1.0); //white
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_obj);
    glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0, 0.0); // new
	glVertex3f(0.5f, 0.5f, 1.0f);
        glTexCoord2f(0.0, 1.0); // new
	glVertex3f(-0.5f, 0.5f, 1.0f);
        glTexCoord2f(1.0, 1.0); //new
	glVertex3f(-0.5f, -0.5f, 1.0f);
        glTexCoord2f(1.0, 0.0); //new
	glVertex3f(0.5f, -0.5f, 1.0f);

	glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0, 0.0); // new
	glVertex3f(0.5f, -0.5f, 0.0f);
        glTexCoord2f(0.0, 1.0); // new
	glVertex3f(-0.5f, -0.5f, 0.0f);
        glTexCoord2f(1.0, 1.0); //new
	glVertex3f(-0.5f, 0.5f, 0.0f);
        glTexCoord2f(1.0, 0.0); //new
	glVertex3f(0.5f, 0.5f, 0.0f);

	glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0, 0.0); // new
	glVertex3f(0.5f, 0.5f, 0.0f);
        glTexCoord2f(0.0, 1.0); // new
	glVertex3f(0.5f, 0.5f, 1.0f);
        glTexCoord2f(1.0, 1.0); //new
	glVertex3f(0.5f, -0.5f, 1.0f);
        glTexCoord2f(1.0, 0.0); //new
	glVertex3f(0.5f, -0.5f, 0.0f);

	glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0, 0.0); // new
	glVertex3f(-0.5f, 0.5f, 1.0f);
        glTexCoord2f(0.0, 1.0); // new
	glVertex3f(-0.5f, 0.5f, 0.0f);
        glTexCoord2f(1.0, 1.0); //new
	glVertex3f(-0.5f, -0.5f, 0.0f);
        glTexCoord2f(1.0, 0.0); //new
	glVertex3f(-0.5f, -0.5f, 1.0f);

	glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0, 0.0); // new
	glVertex3f(0.5f, 0.5f, 1.0f);
        glTexCoord2f(0.0, 1.0); // new
	glVertex3f(0.5f, 0.5f, 0.0f);
        glTexCoord2f(1.0, 1.0); //new
	glVertex3f(-0.5f, 0.5f, 0.0f);
        glTexCoord2f(1.0, 0.0); //new
	glVertex3f(-0.5f, 0.5f, 1.0f);

	glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0, 0.0); // new
	glVertex3f(0.5f, -0.5f, 0.0f);
        glTexCoord2f(0.0, 1.0); // new
	glVertex3f(0.5f, -0.5f, 1.0f);
        glTexCoord2f(1.0, 1.0); //new
	glVertex3f(-0.5f, -0.5f, 1.0f);
        glTexCoord2f(1.0, 0.0); //new
	glVertex3f(-0.5f, -0.5f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glEndList();

    initialized = true;

    return true;
}


// Draw
void
Track::Draw(void)
{
    float   posn[3];
    float   tangent[3];
    double  angle;

    if ( ! initialized )
	return;

    glPushMatrix();

    // Draw the track
    glCallList(track_list);

    glPushMatrix();

    // Figure out where the train is
    track->Evaluate_Point(posn_on_track, posn);

    // Translate the train to the point
    glTranslatef(posn[0], posn[1], posn[2]);

    // ...and what it's orientation is
    track->Evaluate_Derivative(posn_on_track, tangent);
    Normalize_3(tangent);

    // Rotate it to poitn along the track, but stay horizontal
    angle = atan2(tangent[1], tangent[0]) * 180.0 / M_PI;
    glRotatef((float)angle, 0.0f, 0.0f, 1.0f);

    // Another rotation to get the tilt right.
    angle = asin(-tangent[2]) * 180.0 / M_PI;
    glRotatef((float)angle, 0.0f, 1.0f, 0.0f);

    // Draw the train
    glCallList(train_list);

    glPopMatrix();
    glPopMatrix();
}


void
Track::Update(float dt)
{
    float   point[3];
    float   deriv[3];
    double  length;
    double  parametric_speed;

    if ( ! initialized )
	return;

    // First we move the train along the track with its current speed.

    // Get the derivative at the current location on the track.
    track->Evaluate_Derivative(posn_on_track, deriv);

    // Get its length.
    length = sqrt(deriv[0]*deriv[0] + deriv[1]*deriv[1] + deriv[2]*deriv[2]);
    if ( length == 0.0 )
	return;

    // The parametric speed is the world train speed divided by the length
    // of the tangent vector.
    parametric_speed = speed / length;

    // Now just evaluate dist = speed * time, for the parameter.
    posn_on_track += (float)(parametric_speed * dt);

    // If we've just gone around the track, reset back to the start.
    if ( posn_on_track > track->N() )
	posn_on_track -= track->N();

    // As the second step, we use conservation of energy to set the speed
    // for the next time.
    // The total energy = z * gravity + 1/2 speed * speed, assuming unit mass
    track->Evaluate_Point(posn_on_track, point);
    //if ( TRAIN_ENERGY - 9.81 * point[2] < 0.0 )
    float gravity = 5.00;
    if ( TRAIN_ENERGY - gravity * point[2] < 0.0 )
	speed = 0.0;
    else
	speed = (float)sqrt(2.0 * ( TRAIN_ENERGY - gravity * point[2] ));
}


void Track::View_point_setup(float *eye, float *look_at_posn, float *up){
        float   x_coord[3], y_coord[3], z_coord[3];
        float   position[3];
        // Figure out where the train is
        track->Evaluate_Point(posn_on_track, position);
        // ...and what it's orientation is
        track->Evaluate_Derivative(posn_on_track, y_coord);
        Normalize_3(y_coord);
        x_coord[0] = y_coord[1];
        x_coord[1] = -y_coord[0];
        x_coord[2] = 0;
        z_coord[0] = x_coord[1] * y_coord[2] - x_coord[2] * y_coord[1];
        z_coord[1] = x_coord[2] * y_coord[0] - x_coord[0] * y_coord[2];
        z_coord[2] = x_coord[0] * y_coord[1] - x_coord[1] * y_coord[0];
        for(int i =0; i < 3; i++){
                eye[i]                  = position[i] + 1.1 * z_coord[i];
                look_at_posn[i] = 100.0 * y_coord[i];
                up[i]                   = z_coord[i];
        }
        return;
}
