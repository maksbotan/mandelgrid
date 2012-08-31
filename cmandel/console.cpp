
#include <sys/ioctl.h>
#include <cmath>
#include <iostream>

#include "mandel.h"

void get_size(int *width, int *height){
    struct winsize size;
    ioctl(0, TIOCGWINSZ, &size);

    *width = size.ws_col;
    *height = size.ws_row;
}

void colorize_output(mandelbrot_type value, mandelbrot_type quality){
    if (value == quality)
        std::cout << " ";
    else {
        int color = (double)(quality-value)/quality*14;
        std::cout << "\033[3" << color/7 << ";4" << color % 7 << "m";
        std::cout << ((value < quality/20) ? "x" : "X");
        std::cout << "\033[0m";
    }
}

int main(){
    int w, h;
    get_size(&w, &h);
    w -= w % 3;
    h -= h % 2;

    mandelbrot_type *data = new mandelbrot_type[w*h];
    MandelbrotRenderer renderer(
        w, h,
        -2.0, -1.0, 1.0, 1.0,
        64, data);
    renderer.render();

    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++)
            colorize_output(data[y*w+x], 64);
        std::cout << std::endl;
    }

    delete []data;
    return 0;
}
