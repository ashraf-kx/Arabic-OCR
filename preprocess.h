#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <QObject>
#include <QDebug>
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Preprocess : public QObject
{
    Q_OBJECT
private:
    struct pos
    {
        int x;
        int y;
    };

public:
    explicit Preprocess(QObject *parent = 0);

    Mat binarization(const Mat &image, int thresholdValue = 0);
    Mat contour(const Mat &image);
    Mat convert2cxx(const Mat &binary);
    Mat computeSkewAndRotate(Mat image);
    Mat Thinning(Mat image);
    Mat copyRect(const Mat &image, int x1, int y1, int x2, int y2);

    void thinningIteration(Mat &im, int iter);
    void thinning(Mat &image);
};

#endif // PREPROCESS_H
