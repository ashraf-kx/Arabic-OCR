#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "preprocess.h"
#include <QObject>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QImage>
#include <QDateTime>
#include <QtCore5Compat/QTextCodec>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <iostream>

#include <poppler/qt6/poppler-qt6.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Segmentation : public QObject
{
    Q_OBJECT
private:
    Mat binarizedMat;

    Mat Visualization_V;
    Mat Visualization_H;
    QImage imageUnderProcess;

    enum HistogramType
    {
        VERTICAL,
        HORIZONTAL
    };

    int w; // just for the test.

    struct Line
    {
        int start;
        int end;
    };

    struct Word
    {
        int x_start;
        int x_end;
        int y_start;
        int y_end;
    };

    struct pos
    {
        int x;
        int y;
    };

    struct Candidate
    {
        int timeRepeated;
        int value;
    };

    Candidate C;
    Line line;
    Word word;
    QList<Line> geoLines;
    QList<Word> geoWords;

public:
    explicit Segmentation(QObject *parent = 0);

    QList<Mat> tryCutWord(Mat imageIn);
    QList<int> cutWhereWhite(QList<int> list);
    QList<Mat> getCharactersInWord(Mat image, QList<int> posCuts);
    void getPositionCutWord(Mat imageLine, int lineIndex);
    int mostRedundantValue(QList<int> list);
    QList<int> calculateBackProjection(Mat imageIn, HistogramType type);
    Mat smoothingHistogram(Mat image);
    static Mat copyRect(Mat image, int x1, int y1, int x2, int y2);
    static Mat characterNormalization(Mat image);
    QList<Mat> cutLine2Characters(QList<Mat> listImagesLines, int i);
    QList<Mat> getCollectionOfLinesDetected(Mat image);
    QList<Mat> segmentEntireDocument(Mat image);
    void lineDetection(QList<int> histogramData);
    void wordsDetection();
    int averageLineHeight(QList<int> histogramData);
};

#endif // SEGMENTATION_H
