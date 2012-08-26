
#include "config.h"

#ifdef HAVE_CLOCK
#include <time.h>
#endif
#include <iostream>

#include "mandel.h"
#ifdef HAVE_GD
#include "gdinterface.h"
#endif

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

#ifdef HAVE_CLOCK
    clock_t time_start = clock();
#endif
    renderer.render();
#ifdef HAVE_CLOCK
    clock_t time_stop = clock();
    std::cout << "Rendered in " << (double)(time_stop - time_start) / CLOCKS_PER_SEC << " seconds" << std::endl;
#endif

#ifdef HAVE_GD
#ifdef HAVE_CLOCK
    time_start = clock();
#endif
    save_png(data, WIDTH, HEIGHT, QUALITY, "mandelbrot.png");
#ifdef HAVE_CLOCK
    time_stop = clock();
    std::cout << "Made PNG in " << (double)(time_stop - time_start) / CLOCKS_PER_SEC << " seconds" << std::endl;
#endif
#else
    std::cout << "This is quite useless without LibGD support :)" << std::endl;
#endif

    delete []data;

    return 0;
}
