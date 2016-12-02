/*
 * CS559 Maze Project
 *
 * Class header file for the WorldWindow class. The WorldWindow is
 * the window in which the viewer's view of the world is displayed.
 *
 * (c) Stephen Chenney, University of Wisconsin at Madison, 2001-2002
 *
 */

#ifndef _WORLDWINDOW_H_
#define _WORLDWINDOW_H_

#include <Fl/Fl.h>
#include <Fl/Fl_Gl_Window.h>
#include "Ground.h"
#include "Track.h"
#include "Tree.h"
#include "Wall.h"
#include "Walkway.h"
#include "Merry.h"

#define NUM_TREES 20

// Subclass the Fl_Gl_Window because we want to draw OpenGL in here.
class WorldWindow : public Fl_Gl_Window {
    public:
	// Constructor takes window position and dimensions, the title.
	WorldWindow(int x, int y, int w, int h, char *label);

	// draw() method invoked whenever the view changes or the window
	// otherwise needs to be redrawn.
	void	draw(void);

	// Event handling method. Uses only mouse events.
	int	handle(int);

	// Update the world according to any events that have happened since
	// the last time this method was called.
	bool	Update(float);

        // roller coaster mode ftw
        void    roller_coaster_view();

        // merry go round view
        void    merry_view();
        
        //tree view
        void    tree_view();

        //top down view
        void    top_view();

        //rollercoaster  texture view
        void    bee_view();


    private:
	Ground	    ground;	    // The ground object.
	Track  traintrack;	    // The train and track.
        // make a list of pointers to our trees.
        Tree * tree_row_1[NUM_TREES];
        Tree * tree_row_2[NUM_TREES];
        Tree * tree_row_3[NUM_TREES];
        Tree * tree_row_4[NUM_TREES];
        Wall * north_wall; // first wall
        Wall * south_wall; // first wall
        Wall * east_wall; // first wall
        Wall * west_wall; // first wall
        // walkways
        Walkway * w1; 
        Walkway * w2;
        Walkway * w3;
        Walkway * w4;
        Walkway * w5;
        // merry go round
        Merry * merry;

	static const double FOV_X; // The horizontal field of view.

	float	phi;	// Viewer's inclination angle.
	float	theta;	// Viewer's azimuthal angle.
	float	dist;	// Viewer's distance from the look-at point.
	float	x_at;	// The x-coord to look at.
	float	y_at;	// The y-coord to look at. z_at is assumed 2.0.

	int     button;	// The mouse button that is down, -1 if none.
	int	x_last;	// The location of the most recent mouse event
	int	y_last;
	int	x_down; // The location of the mouse when the button was pushed
	int	y_down;
	float   phi_down;   // The view inclination angle when the mouse
			    // button was pushed
	float   theta_down; // The view azimuthal angle when the mouse
			    // button was pushed
	float	dist_down;  // The distance when the mouse button is pushed.
	float	x_at_down;  // The x-coord to look at when the mouse went down.
	float	y_at_down;  // The y-coord to look at when the mouse went down.

        int     view_flag;
        int     merry_flag;
        int     top_flag;
        int     tree_flag; 
        int     bee_flag;

	void	Drag(float);	// The function to call for mouse drag events
};


#endif
