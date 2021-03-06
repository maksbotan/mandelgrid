#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "mandelbrotshower.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MandelbrotShower *shower;
    QLabel X, Y;
    QProgressBar pb;
    bool julia;
public slots:
    void render_set();
    void save();
    void new_region(QRectF region, QSize new_size);
    void update_pos(QPointF pos);
    void reset_defaults();
    void magnify();
    void set_num_threads(QString text);
    void switch_mode(QString mode);
    void julia_point(QPointF point);
    void show_stats(double t_render);
};

#endif // MAINWINDOW_H
