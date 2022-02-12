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

//////////////////////// INCLUDE OPEN Computer Vision /////////////////////
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml//ml.hpp>
////////////////////////            END               /////////////////////

using namespace cv;
using namespace std;

class Segmentation : public QObject
{
    Q_OBJECT
private:
    Mat _imgBW;

    Mat    Visualization_V;
    Mat    Visualization_H;
    QImage imgInProcessus;

    enum HistType { V_Hist, H_Hist };

    int w;  // just for the test.

    struct line
    {
        int start;
        int end;
    };

    struct word
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

    struct candidate
    {
        int timeRepeated;
        int value;
    };

    candidate C;
    line L;
    word W;
    QList<line> geoLines;
    QList<word> geoWords;

public:
    explicit Segmentation(QObject *parent = 0);

    QList<Mat> tryCutWord(Mat imageIn);
    QList<int> cutWhereWhite(QList<int> list);
    QList<Mat> getCharactersInWord(Mat image,QList<int> posCuts);
    void       getPosCutWord(Mat imageLine, int lineNbr);
    int        mostRedundantValue(QList<int> list);
    QList<int> calculateBackProjection(Mat imageIn,HistType type);
    Mat        smoothingHistogramme(Mat image);
    static Mat copyRect(Mat image,int x1,int y1,int x2,int y2);
    static Mat characterNormalization(Mat image);
    QList<Mat> cutLine2Characters(QList<Mat> listImagesLines, int i);
    QList<Mat> getAllImagesLines(Mat image);
    QList<Mat> segmenteEntierDocument(Mat image);
    void       lineDetection(QList<int> HistDATA);
    void       wordsDetection();
    int        averageLineHeight(QList<int> list);

};

#endif // SEGMENTATION_H
