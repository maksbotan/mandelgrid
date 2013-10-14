
#include "config.h"

#include <time.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "mandel.h"
#include "gdinterface.h"
#ifdef BUILD_NETWORK_MASTER
#include "server.h"
#endif

void usage(){
    std::cout << "Command-line mandelbrot set renderer. Available options:" << std::endl;
    std::cout << "-j\tenable julia mode (must be _first_ argument)" << std::endl;
    std::cout << "-cx\tjulia set C.x\t\tdefault: 0" << std::endl;
    std::cout << "-cy\tjulia set C.y\t\tdefault: 1" << std::endl;
    std::cout << "-w\tpicture width\t\tdefault: 1200" << std::endl;
    std::cout << "-h\tpicture height\t\tdefault: 800" << std::endl;
    std::cout << "-l\tset left border\t\tdefault: -2" << std::endl;
    std::cout << "-d\tset down border\t\tdefault: -1" << std::endl;
    std::cout << "-r\tset right border\tdefault: 1" << std::endl;
    std::cout << "-u\tset up border\t\tdefault: 1" << std::endl;
    std::cout << "-q\tquality\t\t\tdefault: 256" << std::endl;
#ifdef _OPENMP
    std::cout << "-t\tthread number\t\tdefault: OpenMP choice" << std::endl;
#endif
#ifdef BUILD_NETWORK_MASTER
    std::cout << "-n\tStart master node" << std::endl;
    std::cout << "-p\tPort to listen on\tdefault: 1573" << std::endl;
#endif
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
    bool julia = false;
    double cx = 0.0, cy = 1.0;
#ifdef BUILD_NETWORK_MASTER
    bool master = false;
    int port = 1573;
#endif

    for (int i = 1; i < argc; i++){
        if (!strcmp(argv[i], "--help")){
            usage();
            exit(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-j")){
            julia = true;
            down = -2.0;
            right = 2.0;
            up = 2.0;
            width = 800;
        } else if (!strcmp(argv[i], "-cx")){
            if (i != argc-1)
                sscanf(argv[++i], "%lf", &cx);
            else
                error();
        } else if (!strcmp(argv[i], "-cy")){
            if (i != argc-1)
                sscanf(argv[++i], "%lf", &cy);
            else
                error();
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
#ifdef _OPENMP
        } else if (!strcmp(argv[i], "-t")){
            if (i != argc-1){
                int threads;
                sscanf(argv[++i], "%d", &threads);
                omp_set_num_threads(threads);
            }
            else
                error();
#endif
#ifdef BUILD_NETWORK_MASTER
        } else if (!strcmp(argv[i], "-n"))
            master = true;
        else if (!strcmp(argv[i], "-p")){
            if (i != argc-1){
                sscanf(argv[++i], "%d", &port);
            }
            else
                error();
#endif
        }
    }

    unsigned int size = width*height;
    mandelbrot_type *data = new mandelbrot_type[size];
    std::fill(data, data + size, '\0');

#ifdef BUILD_NETWORK_MASTER
    if (master){
        try {
            MandelbrotServer server(
                width, height,
                left, down, right, up,
                quality,
                data,
                port
            );
            server.run();
        } catch(...) {
            std::cerr << "Error in networking code" << std::endl;
            delete []data;
            exit(EXIT_FAILURE);
        }
    }
    else
#endif
    {
        MandelbrotRenderer renderer(
            width, height,
            left, down, right, up,
            quality,
            data
        );
        if (julia)
            renderer.set_julia_mode(cx, cy);

        clock_t time_start = clock();
        renderer.render();
        clock_t time_stop = clock();
        std::cout << "Rendered in " << (double)(time_stop - time_start) / CLOCKS_PER_SEC << " seconds" << std::endl;
    }

    clock_t time_start = clock();
    save_png(data, width, height, quality, "mandelbrot.png");
    clock_t time_stop = clock();
    std::cout << "Made PNG in " << (double)(time_stop - time_start) / CLOCKS_PER_SEC << " seconds" << std::endl;

    delete []data;

    return 0;
}
