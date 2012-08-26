
#include <time.h>
#include <iostream>

#include "mandel.h"
#include "gdinterface.h"

#define WIDTH 1200
#define HEIGHT 800
#define QUALITY 512

int main(void){
    unsigned int size = WIDTH*HEIGHT;
    mandelbrot_type *data = new mandelbrot_type[size];
    std::fill(data, data + size, '\0');


    MandelbrotRenderer renderer(
        WIDTH, HEIGHT, //WxH
        -2, -1, 1, 1, //Bounds
        QUALITY, //Quality
        data
    );

    clock_t time_start = clock();
    renderer.render();
    clock_t time_stop = clock();

    std::cout << "Rendered in " << (double)(time_stop - time_start) / CLOCKS_PER_SEC << " seconds" << std::endl;

    time_start = clock();
    save_png(data, WIDTH, HEIGHT, QUALITY, "mandelbrot.png");
    time_stop = clock();

    std::cout << "Made PNG in " << (double)(time_stop - time_start) / CLOCKS_PER_SEC << " seconds" << std::endl;

    delete []data;

    return 0;
}
