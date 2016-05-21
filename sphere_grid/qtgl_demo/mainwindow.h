#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MyGLWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    MyGLWidget* myglwidget_;
};

#endif // MAINWINDOW_H
