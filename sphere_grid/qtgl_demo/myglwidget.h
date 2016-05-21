#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>

class MyGLWidget : public QGLWidget {
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);

protected:
    void initializeGL ();
    void resizeGL (int, int);
    void resizeViewport ();
    void paintGL ();

signals:

public slots:
    void update ();
};

#endif // MYGLWIDGET_H
