/*
 * CS559 Maze Project
 *
 * Class file for the WorldWindow class.
 *
 * (c) Stephen Chenney, University of Wisconsin at Madison, 2001-2002
 *
 */

#include "WorldWindow.h"
#include <Fl/math.h>
#include <Fl/gl.h>
#include <Fl/Fl_Gl_Window.H>
#include <stdio.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <iostream>
using namespace std;


const double WorldWindow::FOV_X = 45.0;

WorldWindow::WorldWindow(int x, int y, int width, int height, char *label)
	: Fl_Gl_Window(x, y, width, height, label)
{
    button = -1;
    view_flag = 0;
    merry_flag = 0;
    tree_flag = 0;
    top_flag = 0;
    bee_flag = 0;

    // Initial viewing parameters.
    phi = 45.0f;
    theta = 0.0f;
    dist = 200.0f;
    x_at = 0.0f;
    y_at = 0.0f;

    // build walkways
    //to merry
    w1 = new Walkway(3,10,25,0,0.1f);
    // right
    w2 = new Walkway(3,15,0,12,0.1f);
    // left
    w5 = new Walkway(3,15,0,-12,0.1f);
    //front
    w3 = new Walkway(15,3,12,0,0.1f);
    // back
    w4 = new Walkway(15,3,-12,0,0.1f);

    // make merry go round on origin
    merry = new Merry(0.0f,0.0f,0.0f);


    // BUILD THE WALL
    // AND MAKE OPENGL PAY FOR IT
    int wall_length = 72;
    int max_dist = 35;
    int wall_height = 8;
    int wall_width = 2;
    //height, lentgth, width
    north_wall = new Wall(wall_height,wall_length,wall_width,0,max_dist,0);
    south_wall = new Wall(wall_height,wall_length,wall_width,0,-max_dist,0);
    east_wall = new Wall(wall_height,wall_width,wall_length,max_dist,0,0);
    west_wall = new Wall(wall_height,wall_width,wall_length,-max_dist,0,0);

    // tree bullshilllllllllllll
    int tree_height = 8;
    int tree_base = 2;
    int tree_rand_height_seed = 5;
    int tree_rand_base_seed = 2;
    
    // Make some motherfucking trees!
    //Tree(void) { display_list = 0; initialized = false; height = 10.0; base = 3.0; posn[0] = posn[1] = posn[2] = 0.0f; };
    for (int i = 0; i < NUM_TREES; i++){
      //trees[i] = new Tree(5.0+(rand() % 10), 1+(rand() % 3), -50.0f + (rand() % 100), -50.0f + (rand() % 40), 0.0f);
      int rand1 = (rand() % 100);
      int rand2 = (rand() % 10);
      int x = -50.0f + rand1;
      int y = -50.0f + rand2;
      int height_rand = rand() % tree_rand_height_seed;
      int base_rand = rand() % tree_rand_base_seed;
      // print debug shitballsmotherfuerkcererekkkkrkekrke
      /*
      cout << x;
      cout << ":";
      cout << rand1;
      cout << ",";
      cout << y;
      cout << ",";
      cout << rand2;
      cout << endl;
      */
      tree_row_1[i] = new Tree(tree_height+height_rand, tree_base+base_rand, x, y, 0.0f);
      //Tree(height, base, posn-x, pos-y, posz)
    }
    
    // make another side
    for (int j = 0; j < NUM_TREES; j++){
      //cout << rand() << endl;
      int height_rand = rand() % tree_rand_height_seed;
      int base_rand = rand() % tree_rand_base_seed;
      tree_row_2[j] = new Tree(tree_height+height_rand, tree_base+base_rand, 50.0f - (rand() % 100), 50.0f - (rand() % 10), 0.0f);
    }

    for (int k = 0; k < NUM_TREES; k++){
      int rand1 = (rand() % 10);
      int rand2 = (rand() % 100);
      int x = -50.0f + rand1;
      int y = 50.0f - rand2;
      int base_rand = rand() % tree_rand_base_seed;
      int height_rand = rand() % tree_rand_height_seed;
      /*
      cout << x;
      cout << ":";
      cout << rand1;
      cout << ",";
      cout << y;
      cout << ",";
      cout << rand2;
      cout << endl;
      */
      tree_row_3[k] = new Tree(tree_height+height_rand, tree_base+base_rand, x, y, 0.0f);
    }

    for (int l = 0; l < NUM_TREES; l++){
      int x = 50.0f - (rand() % -10);
      int y = -50.0f + (rand() % 100);
      int height_rand = rand() % tree_rand_height_seed;
      int base_rand = rand() % tree_rand_base_seed;
      tree_row_4[l] = new Tree(tree_height+height_rand, tree_base+base_rand, x, y, 0.0f);
    }
    
}


void
WorldWindow::draw(void)
{
    double  eye[3];
    float   color[4], dir[4];

    if ( ! valid() )
    {
	// Stuff in here doesn't change from frame to frame, and does not
	// rely on any coordinate system. It only has to be done if the
	// GL context is damaged.

	double	fov_y;

	// Sets the clear color to sky blue.
	glClearColor(0.53f, 0.81f, 0.92f, 1.0);

	// Turn on depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Turn on back face culling. Faces with normals away from the viewer
	// will not be drawn.
	glEnable(GL_CULL_FACE);

	// Enable lighting with one light.
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	// Ambient and diffuse lighting track the current color.
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Turn on normal vector normalization. You don't have to give unit
	// normal vector, and you can scale objects.
	glEnable(GL_NORMALIZE);

	// Set up the viewport.
	glViewport(0, 0, w(), h());

	// Set up the persepctive transformation.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	fov_y = 360.0f / M_PI * atan(h() * tan(FOV_X * M_PI / 360.0) / w());
	gluPerspective(fov_y, w() / (float)h(), 1.0, 1000.0);

	// Do some light stuff. Diffuse color, and zero specular color
	// turns off specular lighting.
	color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
	color[0] = 0.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_SPECULAR, color);

	// Initialize all the objects.
	ground.Initialize();
	traintrack.Initialize();


        
        for (int i = 0; i < NUM_TREES; i++){
          tree_row_1[i]->Initialize();
          tree_row_2[i]->Initialize();
          tree_row_3[i]->Initialize();
          tree_row_4[i]->Initialize();
        }

        north_wall -> Initialize();
        east_wall -> Initialize();
        south_wall -> Initialize();
        west_wall -> Initialize();

        w1 -> Initialize();
        w2 -> Initialize();
        w3 -> Initialize();
        w4 -> Initialize();
        w5 -> Initialize();
        merry -> Initialize();

        
    }

    // Stuff out here relies on a coordinate system or must be done on every
    // frame.

    // Clear the screen. Color and depth.
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // roller coaster mode:
    if(view_flag){
      roller_coaster_view();
    }
    else if(merry_flag){
      merry_view();
    }
    else if(top_flag){
      top_view();
    }
    else if(tree_flag){
      tree_view();
    } 
    else if(bee_flag){
      bee_view();
    }   
    else {
      // Set up the viewing transformation. The viewer is at a distance
      // dist from (x_at, y_ay, 2.0) in the direction (theta, phi) defined
      // by two angles. They are looking at (x_at, y_ay, 2.0) and z is up.
      eye[0] = x_at + dist * cos(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
      eye[1] = y_at + dist * sin(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
      eye[2] = 2.0 + dist * sin(phi * M_PI / 180.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      cout << "view" << endl;
      cout << "eye[0]: " << eye[0] << endl;
      cout << "eye[1]: " << eye[1] << endl;
      cout << "eye[2]: " << eye[2] << endl;
      cout << "x_at: " << x_at << endl;
      cout << "y_at: " << y_at << endl;
      gluLookAt(eye[0], eye[1], eye[2], x_at, y_at, 2.0, 0.0, 0.0, 1.0);
    }

    // Position the light source. This has to happen after the viewing
    // transformation is set up, so that the light stays fixed in world
    // space. This is a directional light - note the 0 in the w component.
    dir[0] = 1.0; dir[1] = 1.0; dir[2] = 1.0; dir[3] = 0.0;
    glLightfv(GL_LIGHT0, GL_POSITION, dir);

    // Draw stuff. Everything.
    ground.Draw();
    traintrack.Draw();
    
    for (int i = 0; i < NUM_TREES; i++){
      tree_row_1[i]->Draw();
      tree_row_2[i]->Draw();
      tree_row_3[i]->Draw();
      tree_row_4[i]->Draw();
    }
    north_wall ->Draw();
    east_wall ->Draw();
    west_wall ->Draw();
    south_wall ->Draw();
    

    w2 -> Draw();
    w1 -> Draw();
    w3 -> Draw();
    w4 -> Draw();
    w5 -> Draw();

    merry -> Draw();
}


void
WorldWindow::Drag(float dt)
{
    int	    dx = x_down - x_last;
    int     dy = y_down - y_last;

    switch ( button )
    {
      case FL_LEFT_MOUSE:
	// Left button changes the direction the viewer is looking from.
	theta = theta_down + 360.0f * dx / (float)w();
	while ( theta >= 360.0f )
	    theta -= 360.0f;
	while ( theta < 0.0f )
	    theta += 360.0f;
	phi = phi_down + 90.0f * dy / (float)h();
	if ( phi > 89.0f )
	    phi = 89.0f;
	if ( phi < -5.0f )
	    phi = -5.0f;
	break;
      case FL_MIDDLE_MOUSE:
	// Middle button moves the viewer in or out.
	dist = dist_down - ( 0.5f * dist_down * dy / (float)h() );
	if ( dist < 1.0f )
	    dist = 1.0f;
	break;
      case FL_RIGHT_MOUSE: {
	// Right mouse button moves the look-at point around, so the world
	// appears to move under the viewer.
	float	x_axis[2];
	float	y_axis[2];

	x_axis[0] = -(float)sin(theta * M_PI / 180.0);
	x_axis[1] = (float)cos(theta * M_PI / 180.0);
	y_axis[0] = x_axis[1];
	y_axis[1] = -x_axis[0];

	x_at = x_at_down + 100.0f * ( x_axis[0] * dx / (float)w()
				    + y_axis[0] * dy / (float)h() );
	y_at = y_at_down + 100.0f * ( x_axis[1] * dx / (float)w()
				    + y_axis[1] * dy / (float)h() );
	} break;
      default:;
    }
}


bool
WorldWindow::Update(float dt)
{
    // Update the view. This gets called once per frame before doing the
    // drawing.

    if ( button != -1 ) // Only do anything if the mouse button is down.
	Drag(dt);

    // Animate the train.
    traintrack.Update(dt);
    // update merry
    merry -> Update(dt);

    return true;
}


int
WorldWindow::handle(int event)
{
    // Event handling routine. Only looks at mouse events.
    // Stores a bunch of values when the mouse goes down and keeps track
    // of where the mouse is and what mouse button is down, if any.
    switch ( event )
    {
      case FL_PUSH:
        button = Fl::event_button();
	x_last = x_down = Fl::event_x();
	y_last = y_down = Fl::event_y();
	phi_down = phi;
	theta_down = theta;
	dist_down = dist;
	x_at_down = x_at;
	y_at_down = y_at;
	return 1;
      case FL_DRAG:
	x_last = Fl::event_x();
	y_last = Fl::event_y();
	return 1;
      case FL_RELEASE:
        button = -1;
	return 1;
      case FL_KEYBOARD:
        // press 'r' on keyboard ascii is 114
        if ( Fl::event_key() == 114){
          if(view_flag){
            // turn off roller coaster mode flag
            view_flag = 0;
          }
          else
            view_flag = 1;
          return 1;
        }
        else if (Fl::event_key() == 116){
        // press 't' on keyboard
          if(merry_flag){
            // turn off merry view;
            merry_flag = 0;
          }
          else
            merry_flag = 1;
          return 1;
        }
        else if (Fl::event_key() == 121){
        // press 'y' on keyboard
          if(tree_flag){
            // turn off merry view;
            tree_flag = 0;
          }
          else
            tree_flag = 1;
          return 1;
        }
        else if (Fl::event_key() == 117){
        // press 'u' on keyboard
          if(top_flag){
            // turn off merry view;
            top_flag = 0;
          }
          else
            top_flag = 1;
          return 1;
        }
        else if (Fl::event_key() == 105){
        // press 'i' on keyboard
          if(bee_flag){
            // turn off merry view;
            bee_flag = 0;
          }
          else
            bee_flag = 1;
          return 1;
        }



    }

    // Pass any other event types on the superclass.
    return Fl_Gl_Window::handle(event);
}

void WorldWindow::roller_coaster_view(){
        float eye[3];
        float look_at_posn[3];
        float up[3];
        traintrack.View_point_setup(eye, look_at_posn, up);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(eye[0], eye[1], eye[2], look_at_posn[0], look_at_posn[1], look_at_posn[2], up[0], up[1], up[2]);

}

void WorldWindow::merry_view(){
      /*
      view
      eye[0]: 26.8773
      eye[1]: -19.4391
      eye[2]: 10.7837
      x_at: -6.09532
      y_at: 4.76513
      */
      glMatrixMode(GL_MODELVIEW);
      //gluLookAt(eye[0], eye[1], eye[2], x_at, y_at, 2.0, 0.0, 0.0, 1.0);
      glLoadIdentity();
      gluLookAt(26.8773, -19.4391, 10.7837, -6.09532, 4.76513, 2.0, 0.0, 0.0, 1.0);
}


void WorldWindow::top_view(){
      glMatrixMode(GL_MODELVIEW);
      //gluLookAt(eye[0], eye[1], eye[2], x_at, y_at, 2.0, 0.0, 0.0, 1.0);
      glLoadIdentity();
      gluLookAt(-6.74342, 5.00894, 229.746, -6.72391, 1.03367, 2.0, 0.0, 0.0, 1.0);
}


void WorldWindow::tree_view(){
      glMatrixMode(GL_MODELVIEW);
      //gluLookAt(eye[0], eye[1], eye[2], x_at, y_at, 2.0, 0.0, 0.0, 1.0);
      glLoadIdentity();
      gluLookAt(85.4871, -70.9285, 15.2027, -6.72391, 1.03367, 2.0, 0.0, 0.0, 1.0);
}


void WorldWindow::bee_view(){
      glMatrixMode(GL_MODELVIEW);
      //gluLookAt(eye[0], eye[1], eye[2], x_at, y_at, 2.0, 0.0, 0.0, 1.0);
      glLoadIdentity();
      gluLookAt(23.454, -26.6217, 27.108, -12.739, 17.0408, 2.0, 0.0, 0.0, 1.0);
}
