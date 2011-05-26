

def scale(bounds, screen_bounds):
    """
    Calculates (xstep, ystep) for fitting bounds (x1, x2, y1, y2) into screen_bounds (w, h)
    """
    
    xdelta = bounds[1] - bounds[0]
    ydelta = bounds[3] - bounds[2]
    return float(xdelta)/screen_bounds[0], float(ydelta)/screen_bounds[1]

def test_function(x0, y0, quality):
    """
    Complex function of Mandelbrot set
    """
    
    iteration = 0
    x, y = x0, y0
    while (x**2 + y**2 <= 4) and (iteration < quality):
        x, y = x**2 - y**2 + x0, 2*x*y + y0
        iteration += 1
    
    return iteration

def construct_row(x1, x2, y, step, quality):
    """
    Builds one row of Mandelbrot set
    """
    x = x1
    row = []
    while x <= x2:
        row.append(test_function(x, y, quality))
        x += step

    return row

def build_set(bounds, screen_bounds, quality):
    """
    Build the set
    """
    x1, x2, y1, y2 = bounds
    xstep, ystep = scale(bounds, screen_bounds)
    mandel_set = []
    
    y = y1
    while y <= y2:
        mandel_set.append(construct_row(x1, x2, y, xstep, quality))
        y += ystep

    return mandel_set

def console_set(mandel_set, quality):
    """
    Test function, render set on console
    """
    c = 1.0 / quality #Coefficient for translating iteration into binary gradations
    for row in mandel_set:
        for pixel in row:
            print {1: "0", 0: "."}[int(round(pixel*c))],
        print 

#Test
if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--width", 
                        default=80,
                        type=int,
                        help="Width of the screen")
    parser.add_argument("--height",
                        default=25,
                        type=int,
                        help="Height of the screen")
    parser.add_argument("-q", "--quality",
                        default=64,
                        type=int,
                        help="Quality (number of iterations)")
    args = parser.parse_args()
    s = build_set((-2, 1, -1, 1), (args.width, args.height), args.quality)
    console_set(s, args.quality)
