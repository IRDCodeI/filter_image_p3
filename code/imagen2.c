#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <omp.h>
#include <time.h>
#define NB_PIXELS 256
typedef struct color_pixel_struct {
    unsigned char r,g,b;
} color_pixel_type;
typedef struct color_image_struct
{
  int width, height;
  color_pixel_type * pixels;
} color_image_type;
typedef struct grey_image_struct
{
  int width, height;
  unsigned char * pixels;
} grey_image_type;
color_image_type * loadColorImage(char *filename){
  int i, width,height,max_value;
  char format[8];
  color_image_type * image;
  FILE * f = fopen(filename,"r");
  if (!f){
    fprintf(stderr,"No se puede abrir el archivo %s...\n",filename);
    exit(-1);
  }
  fscanf(f,"%s\n",format);
  assert( (format[0]=='P' && format[1]=='3'));  
  while(fgetc(f)=='#') 
    {
      while(fgetc(f) != '\n'); // THE EJECUTION ENDS
    }
  fseek( f, -1, SEEK_CUR);
  fscanf(f,"%d %d\n", &width, &height);
  fscanf(f,"%d\n", &max_value);
  image = malloc(sizeof(color_image_type));
  assert(image != NULL);
  image->width = width;
  image->height = height;
  image->pixels = malloc(width*height*sizeof(color_pixel_type));
  assert(image->pixels != NULL);
  for(i=0 ; i<width*height ; i++){
      int r,g,b;
      fscanf(f,"%d %d %d", &r, &g, &b);
      image->pixels[i].r = (unsigned char) r;
      image->pixels[i].g = (unsigned char) g;
      image->pixels[i].b = (unsigned char) b;
    }
  fclose(f);
  return image;
}
grey_image_type * createGreyImage(int width, int height){
  grey_image_type * image = malloc(sizeof(grey_image_type));
  assert(image != NULL);
  image->width = width;
  image->height = height;
  image->pixels = malloc(width*height*sizeof(unsigned char));
  assert(image->pixels != NULL);
  return(image);
}
void saveGreyImage(char * filename, grey_image_type *image){
  int i;
  FILE * f = fopen(filename,"w");
  if (!f){
    fprintf(stderr,"No se puede abrir el archivo %s...\n",filename);
    exit(-1);
  }
  fprintf(f,"P2\n%d %d\n255\n",image->width,image->height);
  for(i=0 ; i<image->width*image->height ; i++){
    fprintf(f,"%d\n",image->pixels[i]);
  }
  fclose(f);
}
grey_image_type* colorImageToGrey(color_image_type *colorImage){
  double t, start, stop;
  start = omp_get_wtime();
  int width=colorImage->width, height=colorImage->height;
  grey_image_type *greyImage=createGreyImage(width, height);
  int r, g, b;
#ifdef PARALLEL
#pragma omp parallel for private(r, g, b) num_threads(PARALLEL)
#endif
  for (int i = 0; i < width * height; i++){
    r=colorImage->pixels[i].r;
    g=colorImage->pixels[i].g;
    b=colorImage->pixels[i].b;
    greyImage->pixels[i] = (299 * r +587 * g + 114 * b) / 1000;
  }
  stop = omp_get_wtime();
  t = stop - start;
  printf("TEMPS-colorToGrey: %lf\n", t);
  return greyImage;
}
// OPERACION QUE CALCULA EL RELIEVE DE PIXELES, ES DECIR CAMPO DE PIXELES
char greyPixelToColor(unsigned char* pixels, int index, int width, int height){
  return 128 - 2*pixels[index - width -1 ] - pixels[index-width] - pixels[index -1]
   + pixels[index +1] + pixels[index +width] + 2*pixels[index + width +1];
}
grey_image_type* greyImageToColor(grey_image_type* greyImage){
  double t, start, stop;
  start = omp_get_wtime();
  int width=greyImage->width, height=greyImage->height;
  grey_image_type* colorImage=createGreyImage(width, height);
#ifdef PARALLEL
#pragma omp parallel for num_threads(PARALLEL)
#endif
  for (int i = 0; i < width*height; i++){
    if(i < width || i % width == 0 || (i+1)% width ==0|| (height-1)*width <= i ){
      colorImage->pixels[i]=greyImage->pixels[i];
    }else{
      colorImage->pixels[i]= greyPixelToColor(greyImage->pixels, i, width, height);
    }
  }
  stop=omp_get_wtime();
  t = stop - start;
  printf("TEMPS-greyToColor: %lf\n", t);
  return colorImage;
}
int main(int argc, char* argv[]){
  saveGreyImage("../output/carblackwhite2_grey.jpg", greyImageToColor(colorImageToGrey(loadColorImage("../images/auto_azul_p3.ppm"))));
  saveGreyImage("../output/carblackwhite_grey.jpg", greyImageToColor(colorImageToGrey(loadColorImage("../images/red_car_p3.ppm"))));
  return 0;
}
