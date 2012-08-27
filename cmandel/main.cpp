
#include "config.h"

#ifdef HAVE_CLOCK
#include <time.h>
#endif
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "mandel.h"
#include "gdinterface.h"

void usage(){
    std::cout << "Command-line mandelbrot set renderer. Available options:" << std::endl;
    std::cout << "-w\tpicture width\t\tdefault: 1200" << std::endl;
    std::cout << "-h\tpicture height\t\tdefault: 800" << std::endl;
    std::cout << "-l\tset left border\t\tdefault: -2" << std::endl;
    std::cout << "-d\tset down border\t\tdefault: -1" << std::endl;
    std::cout << "-r\tset right border\tdefault: 1" << std::endl;
    std::cout << "-u\tset up border\t\tdefault: 1" << std::endl;
    std::cout << "-q\tquality\t\t\tdefault: 256" << std::endl;
}

void error(){
    std::cerr << "Not enough command-line arguments" << std::endl;
    usage();
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
    unsigned int width = 1200, height = 800;
    double left = -2.0, down = -1.0, right = 1.0, up = 1.0;
    mandelbrot_type quality = 256;

    for (int i = 1; i < argc; i++){
        if (!strcmp(argv[i], "--help")){
            usage();
            exit(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-w")){
            if (i != argc-1)
                sscanf(argv[++i], "%u", &width);
            else
                error();
        } else if (!strcmp(argv[i], "-h")){
            if (i != argc-1)
                sscanf(argv[++i], "%u", &height);
            else
                error();
        } else if (!strcmp(argv[i], "-l")){
            if (i != argc-1)
                sscanf(argv[++i], "%lf", &left);
            else
                error();
        } else if (!strcmp(argv[i], "-d")){
            if (i != argc-1)
                sscanf(argv[++i], "%lf", &down);
            else
                error();
        } else if (!strcmp(argv[i], "-r")){
            if (i != argc-1)
                sscanf(argv[++i], "%lf", &right);
            else
                error();
        } else if (!strcmp(argv[i], "-u")){
            if (i != argc-1)
                sscanf(argv[++i], "%lf", &up);
            else
                error();
        } else if (!strcmp(argv[i], "-q")){
            if (i != argc-1)
                sscanf(argv[++i], "%u", &quality);
            else
                error();
        }
    }

    unsigned int size = width*height;
    mandelbrot_type *data = new mandelbrot_type[size];
    std::fill(data, data + size, '\0');


    MandelbrotRenderer renderer(
        width, height,
        left, down, right, up,
        quality,
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

#ifdef HAVE_CLOCK
    time_start = clock();
#endif
    save_png(data, width, height, quality, "mandelbrot.png");
#ifdef HAVE_CLOCK
    time_stop = clock();
    std::cout << "Made PNG in " << (double)(time_stop - time_start) / CLOCKS_PER_SEC << " seconds" << std::endl;
#endif

    delete []data;

    return 0;
}
