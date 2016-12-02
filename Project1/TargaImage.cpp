///////////////////////////////////////////////////////////////////////////////
//
//      TargaImage.cpp                          Author:     Stephen Chenney
//                                              Modified:   Eric McDaniel
//                                              Date:       Fall 2004
//                                              Modified:   Feng Liu
//                                              Date:       Winter 2011
//                                              Why:        Change the library file 
//      Implementation of TargaImage methods.  You must implement the image
//  modification functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "Globals.h"
#include "TargaImage.h"
#include "libtarga.h"
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <time.h>

using namespace std;

// constants
const int           RED             = 0;                // red channel
const int           GREEN           = 1;                // green channel
const int           BLUE            = 2;                // blue channel
const unsigned char BACKGROUND[3]   = { 0, 0, 0 };      // background color


// Computes n choose s, efficiently
double Binomial(int n, int s)
{
    double        res;

    res = 1;
    for (int i = 1 ; i <= s ; i++)
        res = (n - i + 1) * res / i ;

    return res;
}// Binomial


///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage() : width(0), height(0), data(NULL)
{}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h) : width(w), height(h)
{
   data = new unsigned char[width * height * 4];
   ClearToBlack();
}// TargaImage



///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables to values given.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h, unsigned char *d)
{
    int i;

    width = w;
    height = h;
    data = new unsigned char[width * height * 4];

    for (i = 0; i < width * height * 4; i++)
	    data[i] = d[i];
}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Copy Constructor.  Initialize member to that of input
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(const TargaImage& image) 
{
   width = image.width;
   height = image.height;
   data = NULL; 
   if (image.data != NULL) {
      data = new unsigned char[width * height * 4];
      memcpy(data, image.data, sizeof(unsigned char) * width * height * 4);
   }
}


///////////////////////////////////////////////////////////////////////////////
//
//      Destructor.  Free image memory.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::~TargaImage()
{
    if (data)
        delete[] data;
}// ~TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Converts an image to RGB form, and returns the rgb pixel data - 24 
//  bits per pixel. The returned space should be deleted when no longer 
//  required.
//
///////////////////////////////////////////////////////////////////////////////
unsigned char* TargaImage::To_RGB(void)
{
    unsigned char   *rgb = new unsigned char[width * height * 3];
    int		    i, j;

    if (! data)
	    return NULL;

    // Divide out the alpha
    for (i = 0 ; i < height ; i++)
    {
	    int in_offset = i * width * 4;
	    int out_offset = i * width * 3;

	    for (j = 0 ; j < width ; j++)
        {
	        RGBA_To_RGB(data + (in_offset + j*4), rgb + (out_offset + j*3));
	    }
    }

    return rgb;
}// TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Save the image to a targa file. Returns 1 on success, 0 on failure.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Save_Image(const char *filename)
{
    TargaImage	*out_image = Reverse_Rows();

    if (! out_image)
	    return false;

    if (!tga_write_raw(filename, width, height, out_image->data, TGA_TRUECOLOR_32))
    {
	    cout << "TGA Save Error: %s\n", tga_error_string(tga_get_last_error());
	    return false;
    }

    delete out_image;

    return true;
}// Save_Image


///////////////////////////////////////////////////////////////////////////////
//
//      Load a targa image from a file.  Return a new TargaImage object which 
//  must be deleted by caller.  Return NULL on failure.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Load_Image(char *filename)
{
    unsigned char   *temp_data;
    TargaImage	    *temp_image;
    TargaImage	    *result;
    int		        width, height;

    if (!filename)
    {
        cout << "No filename given." << endl;
        return NULL;
    }// if

    temp_data = (unsigned char*)tga_load(filename, &width, &height, TGA_TRUECOLOR_32);
    if (!temp_data)
    {
        cout << "TGA Error: %s\n", tga_error_string(tga_get_last_error());
	    width = height = 0;
	    return NULL;
    }
    temp_image = new TargaImage(width, height, temp_data);
    free(temp_data);

    result = temp_image->Reverse_Rows();

    delete temp_image;

    return result;
}// Load_Image


///////////////////////////////////////////////////////////////////////////////
//
//      Convert image to grayscale.  Red, green, and blue channels should all 
//  contain grayscale value.  Alpha channel shoould be left unchanged.  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::To_Grayscale()
{

    if(!data){
      return NULL;
    }    

    // Use the formula I = 0.299r + 0.587g + 0.114b to convert color images to grayscale. 
    // This will be a key pre-requisite for many other operations. This operation should not affect alpha in any way. 

    for(int i = 0; i < width * height * 4; i += 4){
      // char is 8 bits/1byte in c++
      unsigned char gray_pixel;
      unsigned char rgb[3];
      // Remove Alpha Channel since we don't need to change it
      RGBA_To_RGB(data + i, rgb); 
      gray_pixel = (unsigned char)(0.299*(float)rgb[0] + 0.587*(float)rgb[1] + 0.114*(float)rgb[2]);
      // reassign pixels to new grayscale color
      data[i] = gray_pixel;
      data[i+1] = gray_pixel;
      data[i+2] = gray_pixel;
    }

    return true;

}// To_Grayscale


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using uniform quantization.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Quant_Uniform()
{
    if(!data){
      return NULL;
    }    


    /*
All of these operations assume that the current image has 24 bits of color information. They should still produce 24 bit images, but there should only be 256 different colors in the resulting image (so the image could be stored as an 8 bit indexed color image). Don't be concerned with what happens if you run these operations on something that is already quantized. These operations should not affect alpha - we will only test them on images with alpha = 1 (fully opaque images).

    so we want to take 24 bits (3 bytes) per. We want to reduce that to 8 bits (1 byte) through color quantization. This means blue gets 2^2 levels, red, green gets 2^3

    Use the uniform quantization algorithm to convert the current image from a 24 bit color image to an 8 bit color image. Use 4 levels of blue, 8 levels of red, and 8 levels of green in the quantized image. 
    */

    for(int i = 0; i < width * height * 4; i += 4){
      unsigned char rgb[3];
      // Remove Alpha Channel since we don't need to change it
      RGBA_To_RGB(data + i, rgb); 
      // Want to keep the upper bits, so we do some shifting and masking.
      //take 8 bits, subtract 3 bits, shift which gives us the opposite mask we want, so then we bitwise not it.
      //which gives us a lower 5 bit mask. We can then mask off the lower 5 bits, and only use the upper 3/2 bits. 
      data[i] = rgb[0] & (~((1 << (8-3))-1));
      data[i+1] = rgb[1] & (~((1 << (8-3))-1));
      data[i+2] = rgb[2] & (~((1 << (8-2))-1));
    }

    return true;
}// Quant_Uniform


///////////////////////////////////////////////////////////////////////////////
//
//      Convert the image to an 8 bit image using populosity quantization.  
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Quant_Populosity()
{
    ClearToBlack();
    return false;
}// Quant_Populosity


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image using a threshold of 1/2.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Threshold()
{
    // From the slides:
    // For all dithering we will assume that the image is gray
    // and that intensities are represented as a value in [0, 1.0)

    if(!data){
      return NULL;
    }
    // Convert to Grayscale
    To_Grayscale();

    // since all pixels are now teh same grayscale value, we only need to look at the first Red pixel. 
    // we are going to be lazy and convert the pixel value from 0-255 to [0-1.0) in place.

    float threshold = 0.5;
    unsigned char white = 255;
    unsigned char black = 0;

    for(int i = 0; i < width * height * 4; i += 4){
      unsigned char rgb[3];
      float fractional_pixel;

      RGBA_To_RGB(data + i, rgb);
      // Convert pixel data to between [0-1.0)

      fractional_pixel = (rgb[0]/(float)256);
      if(fractional_pixel < threshold){
        data[i] = black;
        data[i+1] = black;
        data[i+2] = black;
      }
      else{
        // above the threshold
        data[i] = white;
        data[i+1] = white;
        data[i+2] = white;
      }
    }

    return true;
}// Dither_Threshold


///////////////////////////////////////////////////////////////////////////////
//
//      Dither image using random dithering.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Random()
{
    //From website: Use either a threshold of 0.5 or the brightness preserving threshold - your choice.
    // Therefore the easiest to do is 0.5 threshold!!!!

    // seed random number generator with current time
    srand(time(NULL));

    if(!data){
      return NULL;
    }
    // Convert to Grayscale
    To_Grayscale();

    // since all pixels are now teh same grayscale value, we only need to look at the first Red pixel. 
    // we are going to be lazy and convert the pixel value from 0-255 to [0-1.0) in place.

    float threshold = 0.5;
    unsigned char white = 255;
    unsigned char black = 0;

    //Add random values chosen uniformly from the range [-0.2,0.2]

    for(int i = 0; i < width * height * 4; i += 4){
      unsigned char rgb[3];
      float fractional_pixel;

      RGBA_To_RGB(data + i, rgb);
      // Convert pixel data to between [0-1.0)

      // get random number in range
      float max = 0.2;
      float min = -0.2;

      // c++ for not having good ways to generate randoms
      // float r3 = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));

      float notreallyrandom = min + static_cast <float> (rand())/(static_cast <float> (RAND_MAX/(max-min)));
      //float notreallyrandom = rand() % (max-min + 1) + min;

      fractional_pixel = (rgb[0]/(float)256) + notreallyrandom;
      if(fractional_pixel < threshold){
        data[i] = black;
        data[i+1] = black;
        data[i+2] = black;
      }
      else{
        // above the threshold
        data[i] = white;
        data[i+1] = white;
        data[i+2] = white;
      }
    }

    return true;
}// Dither_Random


///////////////////////////////////////////////////////////////////////////////
//
//      Perform Floyd-Steinberg dithering on the image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_FS()
{
    ClearToBlack();
    return false;
}// Dither_FS


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image while conserving the average brightness.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Bright()
{
    if(!data){
      return NULL;
    }
    // Convert to Grayscale
    To_Grayscale();

    unsigned char white = 255;
    unsigned char black = 0;
    double sum = 0;

    //c++ arrays why can't you be more like python?
    //guess we're going to use a vector becuase that's easiest
    vector<unsigned char> image_vector;

    for(int i = 0; i < width * height * 4; i += 4){
      unsigned char rgb[3];
      RGBA_To_RGB(data + i, rgb);
      sum += rgb[0];
      image_vector.push_back(rgb[0]);
    }

    // Compute average brightness
    float average_percent = sum/(width * height)/(float)255.0;
    // between 0 and 1.0
    float threshold_index = (1 - average_percent)*(width * height);

    // now we need to get the value of the pixel at the threshold_index
    // we need an array of sorted values of all the pixels in the image
    sort(image_vector.begin(), image_vector.end()); 
    unsigned char threshold_pixel_value = image_vector[threshold_index];

    for(int i = 0; i < width * height * 4; i += 4){
      unsigned char rgb[3];

      RGBA_To_RGB(data + i, rgb);
      if(rgb[0] < threshold_pixel_value){
        data[i] = black;
        data[i+1] = black;
        data[i+2] = black;
      }
      else{
        data[i] = white;
        data[i+1] = white;
        data[i+2] = white;
      }

    }
    return true;
}// Dither_Bright


///////////////////////////////////////////////////////////////////////////////
//
//      Perform clustered differing of the image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Cluster()
{

   // Dither an image to black and white using cluster dithering with the matrix shown below. The image pixels should be compared to a threshold that depends on the dither matrix below. The pixel should be drawn white if: I[x][y] >= mask[x%4][y%4]. The matrix is:

   float threshold_matrix[4][4] = {{.75,    .375,   .625,   .25},
                             {.0625,     1,   .875, .4375},
                             {.5,    .8125,  .9375, .1250},
                             {.1875, .5625,  .3125, .6875}}; 
    if(!data){
      return NULL;
    }
    // Convert to Grayscale
    To_Grayscale();

    unsigned char white = 255;
    unsigned char black = 0;
    for(int i = 0; i < height; i++){
      for(int j = 0; j < width; j++){
        int offset = ((i*width) + j) * 4;
        unsigned char rgb[3];

        RGBA_To_RGB(data + offset, rgb);
        float fractional_pixel = (rgb[0]/(float)256);

        if(fractional_pixel >= threshold_matrix[i%4][j%4]){
          data[offset] = white;
          data[offset+1] = white;
          data[offset+2] = white;
          data[offset+3] = 255; // alpha stays the same, all opaque
        }
        else{
          data[offset] = black;
          data[offset+1] = black;
          data[offset+2] = black;
          data[offset+3] = 255; //alpha stays the same, all opaque
        }
      }
    }

    return true;
}// Dither_Cluster


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using Floyd-Steinberg dithering over
//  a uniform quantization - the same quantization as in Quant_Uniform.
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Color()
{
    ClearToBlack();
    return false;
}// Dither_Color


///////////////////////////////////////////////////////////////////////////////
//
//      Composite the current image over the given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Over(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout <<  "Comp_Over: Images not the same size\n";
        return false;
    }

    // loop over base image
    for(int i = 0; i < width * height * 4; i += 4){
      // convert alpha to [0-1.0)
      double alpha = ((double)data[i+3]/255.0);
      for(int channel = 0; channel < 4; channel++){
        // overlay current image over the given image with the correct alpha value
        //c = Fc + Gc = c + (1−α )c
        data[i + channel] = data[i + channel] + (pImage -> data[i + channel] * (1.0 - alpha));
      }
    }

    return true;
}// Comp_Over


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "in" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_In(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_In: Images not the same size\n";
        return false;
    }

    // loop over base image
    for(int i = 0; i < width * height * 4; i += 4){
      // convert alpha to [0-1.0)
      double alpha = ((double)pImage -> data[i+3]/255.0);
      for(int channel = 0; channel < 4; channel++){
        // overlay current image over the given image with the correct alpha value
        //c = Fc + Gc =α c
        data[i + channel] = data[i + channel] * alpha;
      }
    }

    return true;
}// Comp_In


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "out" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Out(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Out: Images not the same size\n";
        return false;
    }
    // loop over base image
    for(int i = 0; i < width * height * 4; i += 4){
      // convert alpha to [0-1.0)
      double alpha = ((double)pImage -> data[i+3]/255.0);
      for(int channel = 0; channel < 4; channel++){
        // overlay current image over the given image with the correct alpha value
        //c = Fc + Gc =(1-α) c
        data[i + channel] = data[i + channel] * (1.0 - alpha);
      }
    }

    return true;
}// Comp_Out


///////////////////////////////////////////////////////////////////////////////
//
//      Composite current image "atop" given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Atop(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Atop: Images not the same size\n";
        return false;
    }
    // loop over base image
    for(int i = 0; i < width * height * 4; i += 4){
      // convert alpha to [0-1.0)
      double alpha_f = ((double)data[i+3]/255.0);
      double alpha_g = ((double)pImage -> data[i+3]/255.0);
      for(int channel = 0; channel < 4; channel++){
        // overlay current image over the given image with the correct alpha value
        data[i + channel] = (data[i + channel] * alpha_g) + (pImage -> data[i + channel] * (1.0 - alpha_f));
      }
    }

    return true;
}// Comp_Atop


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image with given image using exclusive or (XOR).  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Xor(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Xor: Images not the same size\n";
        return false;
    }

    // loop over base image
    for(int i = 0; i < width * height * 4; i += 4){
      // convert alpha to [0-1.0)
      double alpha_f = ((double)data[i+3]/255.0);
      double alpha_g = ((double)pImage -> data[i+3]/255.0);
      for(int channel = 0; channel < 4; channel++){
        // overlay current image over the given image with the correct alpha value
        data[i + channel] = (data[i + channel] * (1.0 - alpha_g)) + (pImage -> data[i + channel] * (1.0 - alpha_f));
      }
    }

    return true;
}// Comp_Xor


///////////////////////////////////////////////////////////////////////////////
//
//      Calculate the difference bewteen this imag and the given one.  Image 
//  dimensions must be equal.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Difference(TargaImage* pImage)
{
    if (!pImage)
        return false;

    if (width != pImage->width || height != pImage->height)
    {
        cout << "Difference: Images not the same size\n";
        return false;
    }// if

    for (int i = 0 ; i < width * height * 4 ; i += 4)
    {
        unsigned char        rgb1[3];
        unsigned char        rgb2[3];

        RGBA_To_RGB(data + i, rgb1);
        RGBA_To_RGB(pImage->data + i, rgb2);

        data[i] = abs(rgb1[0] - rgb2[0]);
        data[i+1] = abs(rgb1[1] - rgb2[1]);
        data[i+2] = abs(rgb1[2] - rgb2[2]);
        data[i+3] = 255;
    }

    return true;
}// Difference


bool TargaImage::Apply_Filter_To_Image(double filter[5][5]){
  // Oh man this was a nightmare to figure out. 
  // I ended up writing pseudo code on paper, them implementing this and hoping it actually worked.
  // 5 nested for loops feels like inception at times.
  // There is some error correcting codes for edge detection, but they are pretty basic. 
  // this is a basic algorithm that has some (mostly edge detection) has been adopted from a few online algorithms (forget source)
  
  // convert our source image to a rgb array without alpha.
  unsigned char * rgb = To_RGB();
  // Look through the image from left to right, wrapping on new rows
  for(int x = 0; x < height; ++x){
    for(int y = 0; y < width; ++y){
      // we have to apply the filter matrix for each RGB color, so we do this 0,1,2 times. 
      for(int color = 0; color < 3; ++color){
        // since we are averaging, we need to compute the sum of all the filter applications within the matrix
        // we do not need to worry about dividing by a value since the filter has that factored in (say 1/81 for bartlett for example)
        double sum = 0;
        for(int row = 0; row < 5; ++row){
          for(int column = 0; column < 5; ++column){
            // loop through all the rows and columns of the filter matrix
            //start with upper left corner of matrix
            int row_position = x - 2 + row;
            //start with upper left corner of matrix
            int column_position = y - 2 + column;

            // edge detection and correction
            if(row_position < 0){
              // if the row position is negative (outside image), reflect about the same axis (same as instructions on project website i think?)
              row_position = -row_position;
            }
            if(row_position >= height){
              // if we go past to the bottom of the image outside
              // adjust to be in the image again
              row_position = ((height * 2) - 2) - row_position;
            }
            if(column_position < 0){
              // if the column position is negative (outside image), reflect about the same axis (same as instructions on project website i think?)
              column_position = -column_position;
            }
            if(column_position >= width){
              // if we go past the width of the image
              // adjust to be in the image again
              column_position = ((width * 2) - 2) - column_position;
            }
            // Add the current rgb position pixel * the filter to get the value for that filter element
            // adjusts for RGB offset with the *3 and adds color to get the correct color for the color we are on in our loop
            sum += rgb[((row_position * width + column_position) * 3) + color] * filter[row][column];
          }
        }
        // assign the output image pixel the sum we just computed with our filter.
        data[(x * width + y) * 4 + color] = sum;
        // clamping
        if(sum > 255) {
          data[(x * width + y) * 4 + color] = 255;          
        }
        else if(sum < 0){
          data[(x * width + y) * 4 + color] = 0;
        }
      }
    }
  }
  // To_RGB says we should clean up after ourselves...
  delete[] rgb;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 box filter on this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Box()
{
    double box[5][5] = {{1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0},
                       {1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0},
                       {1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0},
                       {1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0},
                       {1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0, 1.0/25.0}};

    Apply_Filter_To_Image(box);

    return true;
}// Filter_Box


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Bartlett filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Bartlett()
{

   double filter[5][5] = {{1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0},
                          {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                          {3.0/81.0, 6.0/81.0, 9.0/81.0, 6.0/81.0, 3.0/81.0},
                          {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                          {1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0}};
    Apply_Filter_To_Image(filter);
    return true;
}// Filter_Bartlett


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Gaussian()
{
   double filter[5][5] = {{1.0/256.0, 4.0/256.0, 6.0/256.0, 4.0/256.0, 1.0/256.0},
                          {4.0/256.0, 16.0/256.0, 24.0/256.0, 16.0/256.0, 4.0/256.0},
                          {6.0/256.0, 24.0/256.0, 36.0/256.0, 24.0/256.0, 6.0/256.0},
                          {4.0/256.0, 16.0/256.0, 24.0/256.0, 16.0/256.0, 4.0/256.0},
                          {1.0/256.0, 4.0/256.0, 6.0/256.0, 4.0/256.0, 1.0/256.0}};
    Apply_Filter_To_Image(filter);
    return true;
}// Filter_Gaussian

///////////////////////////////////////////////////////////////////////////////
//
//      Perform NxN Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////

bool TargaImage::Filter_Gaussian_N( unsigned int N )
{
    ClearToBlack();
   return false;
}// Filter_Gaussian_N


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 edge detect (high pass) filter on this image.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Edge()
{
   double filter[5][5] = {{-1.0/256.0, -4.0/256.0, -6.0/256.0, -4.0/256.0, -1.0/256.0},
                          {-4.0/256.0, -16.0/256.0, -24.0/256.0, -16.0/256.0, -4.0/256.0},
                          {-6.0/256.0, -24.0/256.0, 220.0/256.0, -24.0/256.0, -6.0/256.0},
                          {-4.0/256.0, -16.0/256.0, -24.0/256.0, -16.0/256.0, -4.0/256.0},
                          {-1.0/256.0, -4.0/256.0, -6.0/256.0, -4.0/256.0, -1.0/256.0}};
    Apply_Filter_To_Image(filter);
    return true;
}// Filter_Edge


///////////////////////////////////////////////////////////////////////////////
//
//      Perform a 5x5 enhancement filter to this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Enhance()
{
   double filter[5][5] = {{-1.0/256.0, -4.0/256.0, -6.0/256.0, -4.0/256.0, -1.0/256.0},
                          {-4.0/256.0, -16.0/256.0, -24.0/256.0, -16.0/256.0, -4.0/256.0},
                          {-6.0/256.0, -24.0/256.0, 476.0/256.0, -24.0/256.0, -6.0/256.0},
                          {-4.0/256.0, -16.0/256.0, -24.0/256.0, -16.0/256.0, -4.0/256.0},
                          {-1.0/256.0, -4.0/256.0, -6.0/256.0, -4.0/256.0, -1.0/256.0}};
    Apply_Filter_To_Image(filter);
    return true;
}// Filter_Enhance


///////////////////////////////////////////////////////////////////////////////
//
//      Run simplified version of Hertzmann's painterly image filter.
//      You probably will want to use the Draw_Stroke funciton and the
//      Stroke class to help.
// Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::NPR_Paint()
{
    ClearToBlack();
    return false;
}



///////////////////////////////////////////////////////////////////////////////
//
//      Halve the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Half_Size()
{
    /*
    Halve the image size. Use a Bartlett filter to do the reconstruction. That means that for each output pixel (i,j) you place a 3x3 discrete filter at input pixel (2i,2j) and the filter is:
    1/16 1/8 1/16
    1/8  1/4 1/8
    1/16 1/8 1/16
    */

    double filter[3][3] = {{1.0/16.0, 1.0/8.0, 1.0/16.0},
                           {1.0/8.0, 1.0/4.0, 1.0/8.0},
                           {1.0/16.0, 1.0/8.0, 1.0/16.0}};

    unsigned char * rgb = To_RGB();

    unsigned char * new_image = new unsigned char[width * height];
    for(int x = 0; x < height; x += 2){
      for(int y = 0; y < width; y += 2){
        for(int color = 0; color < 3; color++){
          // loop through each rgb color.
          //apply filter to adjacent pixels.
          double sum = 0;
          for(int row = 0; row < 3; row++){
            for(int column = 0; column < 3; column++){
              int row_position = x - 1 + row;
              int column_position = y - 1 + column;
              if(row_position < 0){
                row_position = -row_position;
              }
              if(row_position >= height){
                row_position = ((height * 2) - 1) - row_position;
              }
              if(column_position < 0){
                column_position = -column_position;
              }
              if(column_position >= width){
                column_position = ((width * 2) - 1) - column_position;
              }
              sum += rgb[(row_position * width + column_position) * 3 + color] * filter[row][column];
            }
          }
          //data[(x/2 * (width) + y/2) * 4 + color] = data[(x * (width) + y) * 4 + color];
          // apply the filter sum to the new image
          //cout << x+y << endl;
          // 212
          // 610 
          // 214
          new_image[(x/2 * (width/2) + y/2) * 4 + color] = sum;
        }
        //alpha
        new_image[(x/2 * (width/2) + y/2) * 4 + 4 ] = 255;
      }
    }
    //data = new_image;

    delete[] data;
    data = new unsigned char[width * height];
    for(int i = 0; i < (width * height); i++){
      // copy image pixels
      data[i] = new_image[i];
    }
    height = height / 2;
    width = width / 2;
    // resize image to cut off half of the pixels.
    //width = width / 2;
    //height = height / 2;
    delete[] rgb;

    return true;
}// Half_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Double the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Double_Size()
{
    ClearToBlack();
    return false;
}// Double_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Scale the image dimensions by the given factor.  The given factor is 
//  assumed to be greater than one.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Resize(float scale)
{
    ClearToBlack();
    return false;
}// Resize


//////////////////////////////////////////////////////////////////////////////
//
//      Rotate the image clockwise by the given angle.  Do not resize the 
//  image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Rotate(float angleDegrees)
{
    ClearToBlack();
    return false;
}// Rotate


//////////////////////////////////////////////////////////////////////////////
//
//      Given a single RGBA pixel return, via the second argument, the RGB
//      equivalent composited with a black background.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::RGBA_To_RGB(unsigned char *rgba, unsigned char *rgb)
{
    const unsigned char	BACKGROUND[3] = { 0, 0, 0 };

    unsigned char  alpha = rgba[3];

    if (alpha == 0)
    {
        rgb[0] = BACKGROUND[0];
        rgb[1] = BACKGROUND[1];
        rgb[2] = BACKGROUND[2];
    }
    else
    {
	    float	alpha_scale = (float)255 / (float)alpha;
	    int	val;
	    int	i;

	    for (i = 0 ; i < 3 ; i++)
	    {
	        val = (int)floor(rgba[i] * alpha_scale);
	        if (val < 0)
		    rgb[i] = 0;
	        else if (val > 255)
		    rgb[i] = 255;
	        else
		    rgb[i] = val;
	    }
    }
}// RGA_To_RGB


///////////////////////////////////////////////////////////////////////////////
//
//      Copy this into a new image, reversing the rows as it goes. A pointer
//  to the new image is returned.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Reverse_Rows(void)
{
    unsigned char   *dest = new unsigned char[width * height * 4];
    TargaImage	    *result;
    int 	        i, j;

    if (! data)
    	return NULL;

    for (i = 0 ; i < height ; i++)
    {
	    int in_offset = (height - i - 1) * width * 4;
	    int out_offset = i * width * 4;

	    for (j = 0 ; j < width ; j++)
        {
	        dest[out_offset + j * 4] = data[in_offset + j * 4];
	        dest[out_offset + j * 4 + 1] = data[in_offset + j * 4 + 1];
	        dest[out_offset + j * 4 + 2] = data[in_offset + j * 4 + 2];
	        dest[out_offset + j * 4 + 3] = data[in_offset + j * 4 + 3];
        }
    }

    result = new TargaImage(width, height, dest);
    delete[] dest;
    return result;
}// Reverse_Rows


///////////////////////////////////////////////////////////////////////////////
//
//      Clear the image to all black.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::ClearToBlack()
{
    memset(data, 0, width * height * 4);
}// ClearToBlack


///////////////////////////////////////////////////////////////////////////////
//
//      Helper function for the painterly filter; paint a stroke at
// the given location
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::Paint_Stroke(const Stroke& s) {
   int radius_squared = (int)s.radius * (int)s.radius;
   for (int x_off = -((int)s.radius); x_off <= (int)s.radius; x_off++) {
      for (int y_off = -((int)s.radius); y_off <= (int)s.radius; y_off++) {
         int x_loc = (int)s.x + x_off;
         int y_loc = (int)s.y + y_off;
         // are we inside the circle, and inside the image?
         if ((x_loc >= 0 && x_loc < width && y_loc >= 0 && y_loc < height)) {
            int dist_squared = x_off * x_off + y_off * y_off;
            if (dist_squared <= radius_squared) {
               data[(y_loc * width + x_loc) * 4 + 0] = s.r;
               data[(y_loc * width + x_loc) * 4 + 1] = s.g;
               data[(y_loc * width + x_loc) * 4 + 2] = s.b;
               data[(y_loc * width + x_loc) * 4 + 3] = s.a;
            } else if (dist_squared == radius_squared + 1) {
               data[(y_loc * width + x_loc) * 4 + 0] = 
                  (data[(y_loc * width + x_loc) * 4 + 0] + s.r) / 2;
               data[(y_loc * width + x_loc) * 4 + 1] = 
                  (data[(y_loc * width + x_loc) * 4 + 1] + s.g) / 2;
               data[(y_loc * width + x_loc) * 4 + 2] = 
                  (data[(y_loc * width + x_loc) * 4 + 2] + s.b) / 2;
               data[(y_loc * width + x_loc) * 4 + 3] = 
                  (data[(y_loc * width + x_loc) * 4 + 3] + s.a) / 2;
            }
         }
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke() {}

///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke(unsigned int iradius, unsigned int ix, unsigned int iy,
               unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia) :
   radius(iradius),x(ix),y(iy),r(ir),g(ig),b(ib),a(ia)
{
}

