#pragma once

#include <QPainter>
#include <QLabel>
#include <QPolygon>

#include <utility>

class TreeImageLabel : public QLabel {
    Q_OBJECT

public:
    TreeImageLabel();
    ~TreeImageLabel();

    void setMarkFile(const std::string& name);
    void undo();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QPainter painter_;
    std::vector<std::pair<int, QPolygon>> marks_;
    std::pair<int, QPolygon> cur_;
    QPoint mouse_pos_;
    FILE* fp_;
    std::string mark_file_name_;
};
