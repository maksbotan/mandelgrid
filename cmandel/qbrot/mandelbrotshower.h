#ifndef MANDELBROTSHOWER_H
#define MANDELBROTSHOWER_H

#include <cmath>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>
#include <QProgressBar>
#include <QDebug>

#include "mandel.h"

class MandelbrotShower : public QWidget
{
    Q_OBJECT
public:
    explicit MandelbrotShower(QProgressBar *pb_, QWidget *parent = 0);
    ~MandelbrotShower();
private:
    QImage *image;
    QSize size;
    mandelbrot_type quality;
    QRgb *palette;
    bool moving, lock_proportions;
    QPoint move_start;
    QPoint move_now;
    qreal proportion;
    qreal xmin, ymax;
    qreal xres, yres;
    QProgressBar *pb;
    void build_palette();
signals:
    void region_selected(QRectF region, QSize new_size);
    void update_current_pos(QPointF pos);
public slots:
    void render_set(QSize set_size, QRectF set_rect, mandelbrot_type quality);
    void save(QString file_name);
    void set_lock_proportions(bool lock);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
};

void report_progress(unsigned int lines, void* data);

#endif // MANDELBROTSHOWER_H
