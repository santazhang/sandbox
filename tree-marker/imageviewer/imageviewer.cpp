/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <stdio.h>

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif

#include <algorithm>
#include <sstream>
#include <fstream>

#include "imageviewer.h"


ClickableLabel::ClickableLabel(const QString& text, QWidget* parent)
    : QLabel(parent), btn_down(0), txt_fp(NULL)
{
    setText(text);
}

ClickableLabel::~ClickableLabel()
{
    if (txt_fp != NULL) {
        fclose(txt_fp);
    }
}

void ClickableLabel::set_mark_txt_fname(const std::string& txt_fn) {
    this->boxes.clear();
    this->update();
    printf("LOAD: %s\n", txt_fn.c_str());
    std::ifstream fin(txt_fn);
    std::string line;
    while (std::getline(fin, line)) {
        QString qs(line.c_str());
        line = qs.trimmed().toStdString();
        printf("LINE: %s\n", line.c_str());
        std::istringstream iss(line);
        std::string op;
        iss >> op;
        double v;
        iss >> v;
        mark_box bx;
        bx.x = v * this->pixmap()->width();
        iss >> v;
        bx.y = v * this->pixmap()->height();
        iss >> v;
        bx.w = v * this->pixmap()->width();
        iss >> v;
        bx.h = v * this->pixmap()->height();
        if (op == "TREE") {
            bx.type = 1;
            boxes.push_back(bx);
        } else if (op == "NOT_TREE") {
            bx.type = -1;
            boxes.push_back(bx);
        } else if (op == "REMOVE") {
            int idx = boxes.size() - 1;
            while (idx >= 0) {
                if (bx.x == boxes[idx].x && bx.y == boxes[idx].y && bx.w == boxes[idx].w && bx.h == boxes[idx].h) {
                    boxes.erase(boxes.begin() + idx);
                    break;
                }
                idx--;
            }
        }
    }

    if (txt_fp != NULL) {
        fclose(txt_fp);
    }

    txt_fp = fopen(txt_fn.c_str(), "a");
}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    printf("press x = %d, y=%d, btn=%d\n", event->x(), event->y(), event->button());
//    emit clicked();
    last_x = event->x();
    last_y = event->y();


    if (event->button() == Qt::LeftButton) {
        btn_down = 1;
        printf("LEFT!\n");
    } else if (event->button() == Qt::RightButton) {
        btn_down = 2;
        printf("RIGHT!\n");
    }
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent* event)
{
    btn_down = 0;
    printf("release x = %d, y=%d\n", event->x(), event->y());

    cur_x = event->x();
    cur_y = event->y();

    int x = std::min(last_x, cur_x);
    int y = std::min(last_y, cur_y);
    int w = std::max(last_x, cur_x) - x;
    int h = std::max(last_y, cur_y) - y;

    mark_box bx;
    bx.x = x;
    bx.y = y;
    bx.w = w;
    bx.h = h;
    if (event->button() == Qt::LeftButton) {
        printf("TREE: x=%d y=%d w=%d h=%d\n", x, y, w, h);
        bx.type = 1;
    } else if (event->button() == Qt::RightButton) {
        printf("NOT TREE: x=%d y=%d w=%d h=%d\n", x, y, w, h);
        bx.type = -1;
    }
    boxes.push_back(bx);
    if (txt_fp != NULL) {
        double fx = 1.0 * bx.x / this->pixmap()->width();
        double fy = 1.0 * bx.y / this->pixmap()->height();
        double fw = 1.0 * bx.w / this->pixmap()->width();
        double fh = 1.0 * bx.h / this->pixmap()->height();

        if (bx.type == 1) {
            fprintf(txt_fp, "TREE %lf %lf %lf %lf\n", fx, fy, fw, fh);
        } else if (bx.type == -1) {
            fprintf(txt_fp, "NOT_TREE %lf %lf %lf %lf\n", fx, fy, fw, fh);
        }
        fflush(txt_fp);
    }

    update();
//    emit clicked();
}


void ClickableLabel::mouseMoveEvent(QMouseEvent* event){
    this->QLabel::mouseMoveEvent(event);

    cur_x = event->x();
    cur_y = event->y();

    //As mouse is moving set the second point again and again
    // and update continuously
//    if(event->type() == QEvent::MouseMove){
//        if(selectedTool == 1){
//            mRect.setBottomRight(event->pos());
//        }
//        else if (selectedTool == 2){
//            mLine.setP2(event->pos());
//        }
//    }

    //it calls the paintEven() function continuously
    update();
}

void ClickableLabel::undo_action() {
//printf("undo action called\n");
    if (boxes.size() > 0) {
        mark_box bx = boxes.back();
        boxes.pop_back();

        if (txt_fp != NULL) {
            double fx = 1.0 * bx.x / this->pixmap()->width();
            double fy = 1.0 * bx.y / this->pixmap()->height();
            double fw = 1.0 * bx.w / this->pixmap()->width();
            double fh = 1.0 * bx.h / this->pixmap()->height();

            fprintf(txt_fp, "REMOVE %lf %lf %lf %lf\n", fx, fy, fw, fh);
            fflush(txt_fp);
        }
    }
    update();
}

void ClickableLabel::paintEvent(QPaintEvent *event){
    this->QLabel::paintEvent(event);

    painter.begin(this);

    QBrush blueBrush(Qt::blue);
    QPen bluePen(blueBrush, 1);
    QBrush redBrush(Qt::red);
    QPen redPen(redBrush, 1);
    painter.setPen(bluePen);
//    printf("called\n");
    for (auto& b : boxes) {
        if (b.type == 1) {
            painter.setPen(bluePen);
            painter.drawRect(b.x, b.y, b.w, b.h);
        } else if (b.type == -1) {
            painter.setPen(redPen);
            painter.drawRect(b.x, b.y, b.w, b.h);
        }
    }

    if (btn_down != 0) {
        if (btn_down == 1) {
            // left button
            painter.setPen(bluePen);
        } else if (btn_down == 2) {
            // right button
            painter.setPen(redPen);
        }

        int x = std::min(last_x, cur_x);
        int y = std::min(last_y, cur_y);
        int w = std::max(last_x, cur_x) - x;
        int h = std::max(last_y, cur_y) - y;
        painter.drawRect(x, y, w, h);
    }

    painter.end();
}

ImageViewer::ImageViewer()
   : imageLabel(new ClickableLabel)
   , scrollArea(new QScrollArea)
   , scaleFactor(1)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);

    createActions();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}


bool ImageViewer::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);
    std::string mark_txt_fname = fileName.toStdString();
    mark_txt_fname = mark_txt_fname.substr(0, mark_txt_fname.length() - 4) + ".tree_marker.txt";
    this->imageLabel->set_mark_txt_fname(mark_txt_fname);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

void ImageViewer::setImage(const QImage &newImage)
{
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;

    scrollArea->setVisible(true);
    printAct->setEnabled(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();
}


bool ImageViewer::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/bmp");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ImageViewer::open()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::saveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::print()
{
    Q_ASSERT(imageLabel->pixmap());
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrintDialog dialog(&printer, this);
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabel->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabel->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
#endif
}

void ImageViewer::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void ImageViewer::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void ImageViewer::undo() {
    imageLabel->undo_action();
}

void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

void ImageViewer::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void ImageViewer::about()
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
               "and QScrollArea to display an image. QLabel is typically used "
               "for displaying a text, but it can also display an image. "
               "QScrollArea provides a scrolling view around another widget. "
               "If the child widget exceeds the size of the frame, QScrollArea "
               "automatically provides scroll bars. </p><p>The example "
               "demonstrates how QLabel's ability to scale its contents "
               "(QLabel::scaledContents), and QScrollArea's ability to "
               "automatically resize its contents "
               "(QScrollArea::widgetResizable), can be used to implement "
               "zooming and scaling features. </p><p>In addition the example "
               "shows how to use QPainter to print an image.</p>"));
}

void ImageViewer::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &ImageViewer::open);
    openAct->setShortcut(QKeySequence::Open);

    saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &ImageViewer::saveAs);
    saveAsAct->setEnabled(false);

    printAct = fileMenu->addAction(tr("&Print..."), this, &ImageViewer::print);
    printAct->setShortcut(QKeySequence::Print);
    printAct->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    undoAct = editMenu->addAction(tr("&Undo"), this, &ImageViewer::undo);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setEnabled(true);

    copyAct = editMenu->addAction(tr("&Copy"), this, &ImageViewer::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    QAction *pasteAct = editMenu->addAction(tr("&Paste"), this, &ImageViewer::paste);
    pasteAct->setShortcut(QKeySequence::Paste);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &ImageViewer::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    helpMenu->addAction(tr("&About"), this, &ImageViewer::about);
    helpMenu->addAction(tr("About &Qt"), &QApplication::aboutQt);
}

void ImageViewer::updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
