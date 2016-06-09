#include "treeimagelabel.h"
#include "mark_file_helper.h"

#include <stdio.h>

#include <algorithm>
#include <sstream>
#include <fstream>

#include <QMouseEvent>

static const int NOT_TREE = 1;
static const int TREE = 2;

TreeImageLabel::TreeImageLabel()
        : QLabel(nullptr), fp_(nullptr) {
    this->setMouseTracking(true);
}

TreeImageLabel::~TreeImageLabel() {
    if (fp_ != nullptr) {
        fclose(fp_);
        simplify_mark_file(mark_file_name_);
    }
}

void TreeImageLabel::setMarkFile(const std::string& txt_fn) {
    if (fp_ != nullptr) {
        fclose(fp_);
        simplify_mark_file(mark_file_name_);
    }
    
    cur_.second.clear();
    marks_.clear();

    mark_file_name_ = txt_fn;
    marks_ = load_marks(mark_file_name_, this->pixmap()->size());
    fp_ = fopen(mark_file_name_.c_str(), "a");

    update();
}

void TreeImageLabel::mousePressEvent(QMouseEvent* event) {
    mouse_pos_ = event->pos();
    if (event->button() == Qt::LeftButton) {
        if (cur_.second.empty()) {
            cur_.first = TREE;
        }
        cur_.second << event->pos();
    } else if (event->button() == Qt::RightButton) {
        if (cur_.second.empty()) {
            cur_.first = NOT_TREE;
        }
        cur_.second << event->pos();
    }
    update();
}

void TreeImageLabel::mouseDoubleClickEvent(QMouseEvent* event) {
    if (cur_.second.empty()) {
        this->mousePressEvent(event);
    } else {
        cur_.second << event->pos();

        add_mark(fp_, cur_, this->pixmap()->size());

        marks_.push_back(cur_);
        cur_.second.clear();
        cur_.first = 0;

        update();
    }
}

void TreeImageLabel::mouseMoveEvent(QMouseEvent* event){
    this->QLabel::mouseMoveEvent(event);

    if (!cur_.second.empty()) {
        mouse_pos_ = event->pos();
        update();
    }
}

void TreeImageLabel::undo() {
    if (!marks_.empty()) {
        std::pair<int, QPolygon> m = marks_.back();
        marks_.pop_back();

        remove_mark(fp_, m, this->pixmap()->size());

        update();
    }
}

void TreeImageLabel::paintEvent(QPaintEvent *event){
    this->QLabel::paintEvent(event);

    painter_.begin(this);

    QBrush blueBrush(Qt::blue);
    QPen bluePen(blueBrush, 1);
    QBrush redBrush(Qt::red);
    QPen redPen(redBrush, 1);

    for (const auto& it : marks_) {
        if (it.first == TREE) {
            painter_.setPen(bluePen);
            painter_.drawPolygon(it.second);
        } else if (it.first == NOT_TREE) {
            painter_.setPen(redPen);
            painter_.drawPolygon(it.second);
        }
    }

    if (!cur_.second.empty()) {
        QPoint lastPoint = cur_.second.point(cur_.second.size() - 1);
        if (cur_.first == TREE) {
            painter_.setPen(bluePen);
            painter_.drawPolyline(cur_.second);
            painter_.drawLine(lastPoint, mouse_pos_);
        } else if (cur_.first == NOT_TREE) {
            painter_.setPen(redPen);
            painter_.drawPolyline(cur_.second);
            painter_.drawLine(lastPoint, mouse_pos_);
        }
    }

    painter_.end();
}
