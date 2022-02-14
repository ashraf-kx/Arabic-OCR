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

    Mat binarizationOTSU(const Mat &image); //! [ 1 ]
    Mat binarizationGLOBALE(const Mat &image, int Threshold);
    Mat contour(const Mat &image);       //! [ 2 ]
    Mat convert2cxx(const Mat &binary);  //! [ 3 ]
    Mat computeSkewAndRotate(Mat image); //! [ 4 ]
    Mat Thinning(Mat image);
    Mat copyRect(const Mat &image, int x1, int y1, int x2, int y2); //! [ 5 ]

    void thinningIteration(Mat &im, int iter); //! [ 6 ]
    void thinning(Mat &im);                    //! [ 7 ]
};

#endif // PREPROCESS_H
