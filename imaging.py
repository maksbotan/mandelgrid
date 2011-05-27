
from PIL import Image

import mandel

def colorize(iterations, quality, colordepth=8):
    """
    Convert iteration number to (R, G, B) according to quality and target color
    depth as bits per color in pixel
    """
    iterations = quality - iterations
    color_range = 2**(colordepth * 3) #Number of colors
    coefficient = (color_range - 1) / float(quality)
    color = int(round(iterations * coefficient))
    r, g, b = (color >> (colordepth * 2),
            (color & ((2**colordepth - 1) << colordepth)) >> colordepth,
            color & (2**colordepth - 1)) #Binary magic
    return (r, g, b)

def grayscalize(iterations, quality, colordepth=8):
    """
    Conver iteration number to grayscale value according to quality and target
    color depth
    """
    iterations = quality - iterations
    color_range = 2**colordepth
    coefficient = (color_range - 1) / float(quality)
    color = int(round(iterations * coefficient))
    return color

def fill_image(image, mandel_set, quality, grayscale=False):
    """
    Fill PIL.Image image with colors determined by mandel_set and quality
    """
    pix = image.load() #Get pixel access to image
    colorize_f = grayscalize if grayscale else colorize
    for y, row in enumerate(mandel_set):
        for x, pixel in enumerate(row):
            try:
                pix[x, y] = colorize_f(pixel, quality)
            except IndexError:
                pass

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--width", 
                        default=300,
                        type=int,
                        help="Width of the screen")
    parser.add_argument("--height",
                        default=200,
                        type=int,
                        help="Height of the screen")
    parser.add_argument("-q", "--quality",
                        default=64,
                        type=int,
                        help="Quality (number of iterations)")
    parser.add_argument("-o", "--output",
                        default="mandelbrot.png",
                        help="Output file")
    args = parser.parse_args()
    
    s = mandel.build_set((-2, 1, -1, 1), (args.width, args.height), args.quality)
    
    im = Image.new("RGB", (args.width, args.height))
    fill_image(im, s, args.quality)
    with open(args.output, "w") as fo:
        im.save(fo)

