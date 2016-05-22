#include "myglwidget.h"

MyGLWidget::MyGLWidget(QWidget *parent) : QGLWidget(parent) {

}

void MyGLWidget::update () {
    updateGL();
}

void MyGLWidget::initializeGL () {
    glClearColor(0,0,0,0);
    glLoadIdentity();
}

void MyGLWidget::resizeGL (int, int) {
    resizeViewport();
    paintGL();
}

void MyGLWidget::resizeViewport () {
    glViewport(0, 0, width(), height());
}

void MyGLWidget::paintGL () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);
    if (width() && height()) {
        glColor3f(1, 1, 1);
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0.1, 0, 0);
        glVertex3f(0.1, 0.1, 0);
        glVertex3f(0, 0.3, 0.1);
        glEnd();

        glColor3f(1, 0, 0);
        glBegin(GL_LINES);
        glVertex3f(-1, 0, 0);
        glVertex3f(1, 0, 0);
        glEnd();
    }
}
