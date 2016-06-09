#include "mainwindow.h"
#include "myglwidget.h"

#include <QPushButton>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    myglwidget_ = new MyGLWidget();
    this->setCentralWidget(myglwidget_);

    this->setMinimumSize(800, 640);
    this->resize(960, 640);
}

MainWindow::~MainWindow() {
    delete myglwidget_;
}
