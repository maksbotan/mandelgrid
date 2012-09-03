#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    X("X: "),
    Y("Y: "),
    julia(false)
{
    ui->setupUi(this);
    ui->width->setValidator(new QIntValidator());
    ui->height->setValidator(new QIntValidator());
    ui->left->setValidator(new QDoubleValidator());
    ui->right->setValidator(new QDoubleValidator());
    ui->up->setValidator(new QDoubleValidator());
    ui->down->setValidator(new QDoubleValidator());
    ui->quality->setValidator(new QIntValidator());
    ui->magnification->setValidator(new QDoubleValidator());
    ui->threads->setValidator(new QIntValidator());
    ui->cx->setValidator(new QDoubleValidator());
    ui->cy->setValidator(new QDoubleValidator());
#ifndef _OPENMP
    ui->threads->hide();
    ui->threadsLabel->hide();
#endif

    ui->juliaCRealLabel->hide();
    ui->cx->hide();
    ui->juliaCImagLabel->hide();
    ui->cy->hide();

    ui->statusBar->addWidget(&X);
    ui->statusBar->addWidget(&Y);
    ui->statusBar->addPermanentWidget(&pb, 1);

    shower = new MandelbrotShower(&pb, this);
    ui->horizontalLayout_2->insertWidget(0, shower);
    connect(shower, SIGNAL(region_selected(QRectF, QSize)), this, SLOT(new_region(QRectF, QSize)));
    connect(shower, SIGNAL(update_current_pos(QPointF)), this, SLOT(update_pos(QPointF)));
    connect(shower, SIGNAL(julia_point(QPointF)), this, SLOT(julia_point(QPointF)));
    connect(shower, SIGNAL(rendering_complete(double)), this, SLOT(show_stats(double)));
    connect(ui->checkBox, SIGNAL(toggled(bool)), shower, SLOT(set_lock_proportions(bool)));
    render_set();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::render_set(){
    QSize set_size(ui->width->text().toInt(),
                   ui->height->text().toInt());

    QRectF set_rect;
    set_rect.setLeft(ui->left->text().toDouble());
    set_rect.setRight(ui->right->text().toDouble());
    set_rect.setTop(ui->up->text().toDouble());
    set_rect.setBottom(ui->down->text().toDouble());

    shower->render_set(set_size, set_rect, ui->quality->text().toInt(), julia, ui->cx->text().toDouble(), ui->cy->text().toDouble());
}

void MainWindow::save(){
    QString file_name = QFileDialog::getSaveFileName(this, "Save picture", QString(), "PNG Images (*.png)");
    shower->save(file_name);
}

void MainWindow::new_region(QRectF region, QSize new_size){
    ui->left->setText(QString::number(region.left()));
    ui->right->setText(QString::number(region.right()));
    ui->up->setText(QString::number(region.top()));
    ui->down->setText(QString::number(region.bottom()));

    if (!new_size.isEmpty()){
        ui->width->setText(QString::number(new_size.width()));
        ui->height->setText(QString::number(new_size.height()));
    }

    ui->pushButton->setFocus();
}

void MainWindow::reset_defaults(){
    if (julia)
        new_region(QRectF(QPointF(-2, 2), QPointF(2, -2)), QSize());
    else
        new_region(QRectF(QPointF(-2, 1), QPointF(1, -1)), QSize());
}

void MainWindow::update_pos(QPointF pos){
    X.setText(QString("X: %1").arg(pos.x()));
    Y.setText(QString("Y: %1").arg(pos.y()));
}

void MainWindow::magnify(){
    qreal factor = ui->magnification->text().toDouble();
    int w = round(ui->width->text().toInt()*factor);
    int h = round(ui->height->text().toInt()*factor);
    ui->width->setText(QString::number(w));
    ui->height->setText(QString::number(h));
}

void MainWindow::set_num_threads(QString text){
#ifdef _OPENMP
    omp_set_num_threads(text.toInt());
#else
    Q_UNUSED(text)
#endif
}

void MainWindow::switch_mode(QString mode){
    if (mode == "Mandelbrot"){
        ui->juliaCRealLabel->hide();
        ui->cx->hide();
        ui->juliaCImagLabel->hide();
        ui->cy->hide();
        ui->width->setText("600");
        ui->height->setText("400");
        julia = false;
        reset_defaults();
    } else {
        ui->juliaCRealLabel->show();
        ui->cx->show();
        ui->juliaCImagLabel->show();
        ui->cy->show();
        ui->width->setText("600");
        ui->height->setText("600");
        julia = true;
        reset_defaults();
    }
}

void MainWindow::julia_point(QPointF point){
    if (!julia)
        return;
    ui->cx->setText(QString::number(point.x()));
    ui->cy->setText(QString::number(point.y()));
}

void MainWindow::show_stats(double t_render){
    ui->stats->setText(QString("Rendered in %1 seconds").arg(t_render));
}
