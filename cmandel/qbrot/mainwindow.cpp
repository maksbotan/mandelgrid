#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    X("X: "),
    Y("Y: ")
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
#ifndef _OPENMP
    ui->threads->hide();
    ui->threadsLabel->hide();
#endif

    ui->statusBar->addWidget(&X);
    ui->statusBar->addWidget(&Y);
    ui->statusBar->addPermanentWidget(&pb, 1);

    shower = new MandelbrotShower(&pb, this);
    ui->horizontalLayout_2->insertWidget(0, shower);
    connect(shower, SIGNAL(region_selected(QRectF, QSize)), this, SLOT(new_region(QRectF, QSize)));
    connect(shower, SIGNAL(update_current_pos(QPointF)), this, SLOT(update_pos(QPointF)));
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

    shower->render_set(set_size, set_rect, ui->quality->text().toInt());
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
