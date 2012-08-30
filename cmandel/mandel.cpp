
#ifdef _OPENMP
#include <omp.h>
#endif

#include <iostream>
#include <cmath>
#include "mandel.h"

MandelbrotRenderer::MandelbrotRenderer(unsigned int width_, unsigned int height_,
                                       double min_x_, double min_y_, double max_x_, double max_y_,
                                       mandelbrot_type quality_, mandelbrot_type *data_) :
    width(width_),
    height(height_),
    min_x(min_x_),
    min_y(min_y_),
    max_x(max_x_),
    max_y(max_y_),
    quality(quality_),
    data(data_),
    cb(NULL)
{
    unsigned int size = width * height;
    statuses = new unsigned char[size];
    std::fill(statuses, statuses+size, '\0');
    step_x = (max_x - min_x) / width;
    step_y = (max_y - min_y) / height;
}

MandelbrotRenderer::~MandelbrotRenderer(){
    delete []statuses;
}

void MandelbrotRenderer::set_progress_cb(progress_cb cb_, void *data){
    cb = cb_;
    cb_data = data;
}

mandelbrot_type MandelbrotRenderer::get_pixel(unsigned int x, unsigned int y){
    return data[y*width + x];
}

void MandelbrotRenderer::set_pixel(unsigned int x, unsigned int y, mandelbrot_type value){
    data[y*width + x] = value;
}

void MandelbrotRenderer::render(){
    unsigned int strip_size = ceil(height/16.0);

    unsigned int done = 0;

#pragma omp parallel for shared(done)
    for (unsigned int y = 0; y < height; y += strip_size){
        unsigned int y1 = (y + strip_size) < height ? (y + strip_size) : height;
        Worker worker(this, y, y1);
        worker.run();
#pragma omp critical
        done += y1 - y;
#ifdef _OPENMP
        if (cb != NULL && omp_get_thread_num() == 0)
#else
        if (cb != NULL)
#endif
            cb(done, cb_data);
    }

    for (unsigned int p = 0; p < width*height - 1; p++)
    if (statuses[p] & Calculated)
        if (!(statuses[p + 1] & Calculated)){
            data[p + 1] = data[p];
            statuses[p + 1] |= Calculated;
        }

    if (cb != NULL)
        cb(height, cb_data);
}

inline unsigned int MandelbrotRenderer::point_to_index(unsigned int x, unsigned int y){
    return y*width + x;
}

inline void MandelbrotRenderer::index_to_point(unsigned int index, unsigned int *x, unsigned int *y){
    *x = index % width;
    *y = index / width;
}

mandelbrot_type MandelbrotRenderer::calculate(unsigned int index, unsigned int x, unsigned int y){
    if (statuses[index] & Calculated)
        return get_pixel(x, y);
    mandelbrot_type res = test_function(min_x + x*step_x, min_y + y*step_y, quality);
    set_pixel(x, y, res);
    statuses[index] |= Calculated;
    return res;
}

void MandelbrotRenderer::add_status(unsigned int index, unsigned char status){
    statuses[index] |= status;
}

bool MandelbrotRenderer::has_status(unsigned int index, unsigned char status){
    return statuses[index] & status;
}

Worker::Worker(MandelbrotRenderer *master_, unsigned int first_line_, unsigned int last_line_) :
    master(master_),
    first_line(first_line_),
    last_line(last_line_)
{

}

void Worker::run(){
    for (unsigned int x = 0; x < master->width; x++){
        enqueue(master->point_to_index(x, first_line));
        enqueue(master->point_to_index(x, last_line - 1));
    }
    for (unsigned int y = first_line + 1; y < last_line - 1; y++){
        enqueue(master->point_to_index(0, y));
        enqueue(master->point_to_index(master->width - 1, y));
    }

    while (!queue.empty()){
        unsigned int index = queue.front();
        queue.pop_front();
        scan(index);
    }
}

void Worker::enqueue(unsigned int index){
    queue.push_back(index);
    master->add_status(index, Queued);
}

void Worker::scan(unsigned int index){
    unsigned int x, y;
    master->index_to_point(index, &x, &y);

    mandelbrot_type center = master->calculate(index, x, y);
    for (char dx = (x == 0 ? 0 : -1); dx < 2; dx++)
        for (char dy = (y == first_line ? 0 : -1); dy < 2; dy++){
            if (dx == 0 && dy == 0)
                continue;
            unsigned int new_index = master->point_to_index(x + dx, y + dy);
            if ( (x + dx < master->width) &&
                 (y + dy < last_line) &&
                 !(master->has_status(new_index, Queued)) &&
                 (master->calculate(new_index, x + dx, y + dy) != center) )
                    enqueue(new_index);
        }
}
mandelbrot_type test_function(double x0, double y0, mandelbrot_type quality){
    long double x, new_x, y;
    mandelbrot_type iteration;

    x = x0;
    y = y0;
    iteration = 0;

    /* Cardioid and bulbs optimizations
    double q;
    q = pow(x-0.25, 2)+pow(y,2);
    if (q*(q+(x-0.25))<0.25*pow(y,2)) {
        return quality;
    }

    if (pow(x+1, 2)+pow(y,2) < 1.0/16){
        return quality;
    }

    if (pow(x+1+5.0/16, 2) + pow(y, 2) < 1.0/256) {
        return quality;
    }
    */

    while ((x*x + y*y <= 4) && (iteration < quality)) {
        new_x = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = new_x;

        iteration++;
    }

    return iteration;
}
