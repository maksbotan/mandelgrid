
from PIL import Image
import mandel, imaging

class AbstractWorker():
    """
    Abstract class for implementing Mandelbrot workers
    """

    def __init__(self, bounds, screen_bounds, quality):
        self.bounds = bounds
        self.screen_bounds = screen_bounds
        self.quality = quality

        self.x = self.bounds[0]
        self.y = self.bounds[2]
        self.xstep, self.ystep = mandel.scale(bounds, screen_bounds)
        self.ready_blocks = []

    def blocks(self):
        """
        Return bounds of 32x32 blocks from top-left of image
        """
        while 1:
            xdelta, ydelta = 0, 0

            if self.x >= self.bounds[1]:
                self.x = self.bounds[0]
                self.y += 32 * self.ystep
                if self.y >= self.bounds[3]:
                    raise StopIteration
                xdelta = 32 * self.xstep
            elif self.x + 32 * self.xstep >= self.bounds[1]:
                xdelta = self.bounds[1] - self.x
            else:
                xdelta = 32 * self.xstep
            if self.y + 32 * self.ystep > self.bounds[3]:
                ydelta = self.bounds[3] - self.y
            else:
                ydelta = 32 * self.ystep
            block = (self.x, self.x + xdelta + self.xstep, self.y, self.y + ydelta + self.ystep)
            self.x += xdelta
            yield block

    def construct_image(self):
        im = Image.new("RGB", self.screen_bounds)
        for block, mandel_set in self.ready_blocks:
            t_bounds = int((block[1] - block[0]) / self.xstep), int((block[3] - block[2]) / self.ystep)
            t_box = int((block[0] - self.bounds[0])/ self.xstep), int((block[2] - self.bounds[2]) / self.ystep)
            t_im = Image.new("RGB", t_bounds)
            imaging.fill_image(t_im, mandel_set, self.quality)
            im.paste(t_im, t_box)

        return im

    def run(self):
        raise NotImplementedError

import multiprocessing as mp

class LocalIPCWorker(AbstractWorker):
    """
    This worker uses multiprocessing for parallelizing generation
    """

    def __init__(self, bounds, screen_bounds, quality, n_threads):
        AbstractWorker.__init__(self, bounds, screen_bounds, quality)
        self.n_threads = n_threads

    @staticmethod
    def render_block(q, res, scale, quality):
        while not q.empty():
            block = q.get()
            res.put((block, map(list, mandel.build_set(block, scale, quality))))
        res.put(None)

    def run(self):
        Q = mp.Queue()
        res = mp.Queue()
        for block in self.blocks():
            Q.put(block)
        for i in range(self.n_threads):
            process = mp.Process(target=LocalIPCWorker.render_block,
                        args=(Q, res, (self.xstep, self.ystep), self.quality))
            process.start()
        c = 0
        while c < self.n_threads:
            r = res.get()
            if r == None:
                c += 1
            else:
                self.ready_blocks.append(r)
