
#pragma once

#include <string>
#include <gd.h>

#include "mandel.h"

void save_png(mandelbrot_type *data, unsigned int width, unsigned int height, mandelbrot_type quality, std::string filename);
