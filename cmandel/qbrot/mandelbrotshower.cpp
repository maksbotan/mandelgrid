#include "mandelbrotshower.h"

MandelbrotShower::MandelbrotShower(QProgressBar *pb_, QWidget *parent) :
    QWidget(parent),
    image(NULL),
    quality(0),
    palette(new QRgb[1]),
    moving(false),
    lock_proportions(true),
    pb(pb_),
    julia(false)
{
    setMouseTracking(true);
}

MandelbrotShower::~MandelbrotShower(){
    delete []palette;
    if (image != NULL)
        delete image;
}

void MandelbrotShower::build_palette(){
    delete []palette;
    palette = new QRgb[quality+1];
    for (mandelbrot_type i = 0; i < quality; i++){
        palette[i] = qRgb(round(255*(cos(i/15.0)+1)/2), round(255*(cos(i/30.0)+1)/2), round(255*(cos(i/45.0)+1)/2));
    }
    palette[quality] = qRgb(0, 0, 0);
}

void MandelbrotShower::render_set(QSize set_size, QRectF set_rect, mandelbrot_type quality_, bool julia, double cx, double cy){
    if (size != set_size){
        if (image != NULL)
            delete image;
        setFixedSize(set_size);
        image = new QImage(set_size, QImage::Format_ARGB32);
        size = set_size;
    }
    proportion = (qreal)set_size.height() / set_size.width();
    xres = set_rect.width() / set_size.width();
    yres = fabs(set_rect.height()) / set_size.height();
    xmin = set_rect.left();
    ymax = set_rect.top();
    if (quality != quality_){
        quality = quality_;
        build_palette();
    }

    pb->setMaximum(set_size.height());
    pb->setValue(0);

    mandelbrot_type *data = new mandelbrot_type[set_size.width()*set_size.height()];
    MandelbrotRenderer renderer(set_size.width(),
                                set_size.height(),
                                set_rect.left(),
                                set_rect.bottom(),
                                set_rect.right(),
                                set_rect.top(),
                                quality,
                                data);
    if (julia)
        renderer.set_julia_mode(cx, cy);
    this->julia = julia;
    renderer.set_progress_cb((progress_cb)&report_progress, (void*) pb);
    clock_t start = clock();
    renderer.render();
    clock_t stop = clock();
    double t_render = (double)(stop - start) / CLOCKS_PER_SEC;

    for (int y = 0; y < set_size.height(); y++){
        QRgb *line = (QRgb*)(image->scanLine(y));
        unsigned int offset = (set_size.height()-y-1)*set_size.width();
        for (int x = 0; x < set_size.width(); x++)
            line[x] = palette[data[offset + x]];
    }
    delete []data;

    update();

    emit rendering_complete(t_render);
}

void MandelbrotShower::paintEvent(QPaintEvent *){
    if (image == NULL)
        return;
    QPainter p(this);
    p.drawImage(0, 0, *image);
    if (moving){
        QPen pen(Qt::red);
        p.setPen(pen);
        p.drawRect(QRect(move_start, move_now));
    }
}

void MandelbrotShower::save(QString file_name){
    image->save(file_name);
}

void MandelbrotShower::mousePressEvent(QMouseEvent *e){
    move_now = move_start = e->pos();
    moving = true;
    update();
}

void MandelbrotShower::mouseReleaseEvent(QMouseEvent *e){
    moving = false;
    update();

    move_now = e->pos();

    QPointF start(xmin + move_start.x()*xres, ymax - move_start.y()*yres);
    QPointF stop(xmin + move_now.x()*xres, ymax - move_now.y()*yres);
    QRectF new_rect(start, stop);

    QSize new_size;
    if (!lock_proportions){
        new_size = size;
        if (size.width() < size.height())
            new_size.setHeight(size.width()*fabs(new_rect.height()/new_rect.width()));
        else
            new_size.setWidth(size.height()*fabs(new_rect.width()/new_rect.height()));
    }

    if (!new_rect.isNull())
        emit region_selected(new_rect, new_size);
    else if (!julia)
        emit julia_point(stop);
}

void MandelbrotShower::mouseMoveEvent(QMouseEvent *e){
    if (moving){
        move_now = e->pos();
        if (lock_proportions){
            move_now.setY(move_start.y() + (move_now.x() - move_start.x()) * proportion);
            QCursor::setPos(mapToGlobal(move_now));
        }
        update();
    }
    emit update_current_pos(QPointF(xmin + e->pos().x()*xres, ymax - e->pos().y()*yres));
}

void MandelbrotShower::set_lock_proportions(bool lock){
    lock_proportions = lock;
}

void report_progress(unsigned int lines, void *data){
    QProgressBar *pb = (QProgressBar*)(data);
    if (pb)
        pb->setValue(lines);
}
