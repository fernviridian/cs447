/*
 * Track.h: Header file for a class that draws the train and its track.
 *
 * (c) 2001: Stephen Chenney, University of Wisconsin at Madison.
 */


#ifndef _TRAINTRACK_H_
#define _TRAINTRACK_H_

#include <Fl/gl.h>
#include "CubicBspline.h"

class Track {
  private:
    GLubyte 	    track_list;	    // the display list for the track.
    GLubyte 	    train_list;	    // the display list for the train.
    bool    	    initialized;    // whether or not we have been initialized.
    CubicBspline    *track;	    // the spline that defines the track.
    float	    posn_on_track;  // the train's parametric position on the
				    // track.
    float	    speed;	    // the train's speed, in world coordinates

    static const int   TRACK_NUM_CONTROLS;     // Constants about the track.
    static const float         TRACK_CONTROLS[][3];
    static const float         TRAIN_ENERGY;
    GLuint          texture_obj;    // The object for the grass texture.

  public:
    // Constructor
    Track(void) { initialized = false; posn_on_track = 0.0f; speed = 0.0f; };

    // Destructor
    ~Track(void);

    bool    Initialize(void);	// Gets everything set up for drawing.
    void    Update(float);	// Updates the location of the train
    void    Draw(void);		// Draws everything.
    void    View_point_setup(float *eye, float *look_at_posn, float *up);
};


#endif

