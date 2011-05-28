
import argparse
import worker

parser = argparse.ArgumentParser()
parser.add_argument("-r", "--resolution",
                    default="300x200",
                    type=lambda x: map(int, x.split("x")),
                    help="Screen size")
parser.add_argument("-b", "--box",
                    default="-2,1,-1,1",
                    type=lambda x: map(int, x.split(",")),
                    help="Target box (x1, x2, y1, y2)")
parser.add_argument("-q", "--quality",
                    default=64,
                    type=int,
                    help="Quality (number of iterations)")
parser.add_argument("-o", "--output",
                    default="mandelbrot.png",
                    help="Name of output file")
parser.add_argument("-n", "--threads",
                    default=3,
                    type=int,
                    help="Number of threads (processes) to use")
args = parser.parse_args()

w = worker.LocalIPCWorker(args.box,
                        args.resolution,
                        args.quality,
                        args.threads)
w.run()
im = w.construct_image()
with open(args.output, "w") as fo:
    im.save(fo)
