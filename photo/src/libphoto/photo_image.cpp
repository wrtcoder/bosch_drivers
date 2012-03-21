/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2009, Robert Bosch LLC.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Robert Bosch nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <photo/photo_image.h>

photo_image_p photo_image_initialize()
{
  photo_image_p image;
  image = (photo_image_p)calloc(1, sizeof(photo_image_t));

  image->width = 0;
  image->height = 0;
  image->bytes_per_pixel = 3;
  image->image_size = 0;
  image->data = NULL;

  return image;
}

/* frees a photo image */
void photo_image_free(photo_image_p image)
{
  if(image->data)
    free(image->data);
  free(image);
}

void photo_image_set_size(photo_image_p image, int image_width, int image_height)
{
  if(image->data)
    free(image->data);

  image->width = image_width;
  image->height = image_height;
  image->image_size = image->width*image->height*image->bytes_per_pixel;
  image->data = (char *) calloc(image->image_size, sizeof(char));
  memset(image->data, 0, image->image_size*sizeof(char));
}

int photo_image_read(photo_image_p image, const char *filename)
{
  int r,c,n=0;
  IplImage* img = cvLoadImage(filename,1);
  if(!img) return 0;
  int w = img->width;
  int h = img->height;
  if(image->width!=w || image->height!=h)
    photo_image_set_size(image, w, h);
  char* data = image->data;
  for(r=0;r<h;++r) {
    for(c=0;c<w;++c) {
      data[n] = ((unsigned char *)(img->imageData + r*img->widthStep))[c*img->nChannels + 2]; ++n; // R
      data[n] = ((unsigned char *)(img->imageData + r*img->widthStep))[c*img->nChannels + 1]; ++n; // G
      data[n] = ((unsigned char *)(img->imageData + r*img->widthStep))[c*img->nChannels + 0]; ++n; // B
    }
  }
  cvReleaseImage(&img);
  return 1;
}

int photo_image_write(photo_image_p image, const char *filename)
{
  int res, r,c,n=0;
  int w = image->width;
  int h = image->height;

  CvSize size = {w,h};
  IplImage* img = cvCreateImage(size,IPL_DEPTH_8U,3);
  char* data = image->data;
  for(r=0;r<h;++r) {
    for(c=0;c<w;++c) {
       ((unsigned char *)(img->imageData + r*img->widthStep))[c*img->nChannels + 2] = data[n]; n++; // R
       ((unsigned char *)(img->imageData + r*img->widthStep))[c*img->nChannels + 1] = data[n]; n++; // G
       ((unsigned char *)(img->imageData + r*img->widthStep))[c*img->nChannels + 0] = data[n]; n++; // B
    }
  }
  res = cvSaveImage(filename, img);
  cvReleaseImage(&img);
  return 1;
}

