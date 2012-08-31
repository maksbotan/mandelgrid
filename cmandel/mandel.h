
#pragma once

#include <deque>

typedef unsigned int mandelbrot_type;

typedef enum {
    New,
    Queued,
    Calculated
} PointStatus;

typedef void(*progress_cb)(unsigned int lines, void *data);

class MandelbrotRenderer {
public:
    MandelbrotRenderer(unsigned int width_, unsigned int height_,
                       double min_x_, double min_y_, double max_x_, double max_y_,
                       mandelbrot_type quality_, mandelbrot_type *data_);
    ~MandelbrotRenderer();
    void set_progress_cb(progress_cb cb, void *data);
    void set_julia_mode(double cx, double cy);
    void render();
    mandelbrot_type get_pixel(unsigned int x, unsigned int y);
    void set_pixel(unsigned int x, unsigned int y, mandelbrot_type value);
    unsigned int point_to_index(unsigned int x, unsigned int y);
    void index_to_point(unsigned int index, unsigned int *x, unsigned int *y);
    mandelbrot_type calculate(unsigned int index, unsigned int x, unsigned int y);
    void add_status(unsigned int index, unsigned char status);
    bool has_status(unsigned int index, unsigned char status);
    unsigned int width, height;
private:
    unsigned char *statuses;
    double min_x, min_y, max_x, max_y;
    mandelbrot_type quality;
    mandelbrot_type *data;
    double step_x, step_y;
    progress_cb cb;
    void *cb_data;
    bool julia;
    double cx, cy;
};

class Worker {
public:
    Worker(MandelbrotRenderer *master, unsigned int first_line_, unsigned int last_line_);
    void run();
private:
    std::deque<unsigned int> queue;
    MandelbrotRenderer *master;
    unsigned int first_line, last_line;
    void enqueue(unsigned int index);
    void scan(unsigned int index);
};

mandelbrot_type test_function(double x0, double y0, mandelbrot_type quality, bool julia = false, double cx = 0, double cy = 1);
