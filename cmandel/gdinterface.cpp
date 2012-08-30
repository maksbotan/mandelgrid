
#include <cstdio>
#include <cmath>
#include "gdinterface.h"

static inline void make_pallete(gdImagePtr image, int *palette, mandelbrot_type quality){
    for (mandelbrot_type i = 0; i < quality; i++)
        palette[i] = gdImageColorAllocate(image, round(255*(cos(i/15.0)+1)/2),
                                                 round(255*(cos(i/30.0)+1)/2),
                                                 round(255*(cos(i/45.0)+1)/2));
    palette[quality] = gdImageColorAllocate(image, 0, 0, 0);
}

void save_png(mandelbrot_type *data, unsigned int width, unsigned int height, mandelbrot_type quality, std::string filename){
    gdImagePtr image = gdImageCreateTrueColor(width, height);

    FILE *file = fopen(filename.c_str(), "wb");

    int *palette = new int[quality+1];
    make_pallete(image, palette, quality);

    for (unsigned int x = 0; x < width; x++)
        for (unsigned int y = 0; y < height; y++){
            gdImageSetPixel(image, x, y, palette[data[(height-y-1)*width + x]]);
        }

    gdImagePng(image, file);
    fclose(file);
    gdImageDestroy(image);
    delete []palette;
}
