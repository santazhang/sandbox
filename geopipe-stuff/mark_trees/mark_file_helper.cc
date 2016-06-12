#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <QSize>

#include "mark_file_helper.h"

static std::vector<std::pair<int, std::vector<std::pair<double, double>>>> load_marks_helper(const std::string& fname) {
    std::vector<std::pair<int, std::vector<std::pair<double, double>>>> marks;
    std::ifstream fin(fname);
    std::string line;
    while (std::getline(fin, line)) {
        QString qs(line.c_str());
        line = qs.trimmed().toStdString();
        std::istringstream iss(line);
        std::pair<int, std::vector<std::pair<double, double>>> mark;
        iss >> mark.first;
        for (;;) {
            double x = -1, y = -1;
            iss >> x >> y;
            if (x < 0 || y < 0) {
                break;
            }
            std::pair<double, double> pt = std::make_pair(x, y);
            if (!mark.second.empty() && mark.second.back() == pt) {
                continue;
            }
            mark.second.push_back(pt);
        }
        if (mark.first == -1) {
            if (!marks.empty()) {
                marks.pop_back();
            }
        } else if (mark.second.size() > 2u) {
            marks.push_back(mark);
        }
    }
    return marks;
}

int simplify_mark_file(const std::string& fname) {
    std::vector<std::pair<int, std::vector<std::pair<double, double>>>> marks = load_marks_helper(fname);
    FILE* fp = fopen((fname + ".tmp").c_str(), "w");
    if (fp == NULL) {
        printf("  *** Failed to open for write: %s\n", (fname + ".tmp").c_str());
        return -1;
    }
    for (const auto& m : marks) {
        fprintf(fp, "%d", m.first);
        for (const auto& p : m.second) {
            fprintf(fp, " %lf %lf", p.first, p.second);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    rename((fname + ".tmp").c_str(), fname.c_str());
    return 0;
}

std::vector<std::pair<int, QPolygon>> load_marks(const std::string& fname, QSize imageSize) {
    std::vector<std::pair<int, QPolygon>> marks;
    std::vector<std::pair<int, std::vector<std::pair<double, double>>>> marks2 = load_marks_helper(fname);
    for (const auto& m2 : marks2) {
        std::pair<int, QPolygon> m;
        m.first = m2.first;
        for (const auto& p : m2.second) {
            m.second << QPoint(p.first * imageSize.width(), p.second * imageSize.height());
        }
        marks.push_back(m);
    }
    return marks;
}

int add_mark(FILE* fp, const std::pair<int, QPolygon>& mark, QSize imageSize) {
    if (fp == nullptr || ferror(fp)) {
        return -1;
    }
    if (mark.second.size() <= 2u) {
        return 0;
    }
    fprintf(fp, "%d", mark.first);
    for (const auto& p : mark.second) {
        fprintf(fp, " %lf %lf", 1.0 * p.x() / imageSize.width(), 1.0 * p.y() / imageSize.height());
    }
    fprintf(fp, "\n");
    return 0;
}

int remove_mark(FILE* fp, const std::pair<int, QPolygon>& mark, QSize imageSize) {
    if (fp == nullptr || ferror(fp)) {
        return -1;
    }
    fprintf(fp, "-1\n");
    return 0;
}
