#pragma once

#include <string>
#include <vector>
#include <utility>

#include <QPolygon>

int simplify_mark_file(const std::string& fname);

std::vector<std::pair<int, QPolygon>> load_marks(const std::string& fname, QSize imageSize);

int add_mark(FILE* fp, const std::pair<int, QPolygon>& mark, QSize imageSize);

int remove_mark(FILE* fp, const std::pair<int, QPolygon>& mark, QSize imageSize);
