#ifndef RECOGNITION_H
#define RECOGNITION_H


#include "segmentation.h"

#include <QObject>
#include <QTextCodec>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <iostream>

//////////////////////// INCLUDE OPEN Computer Vision /////////////////////
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml//ml.hpp>
////////////////////////            END               /////////////////////

#define	NTRAINING_SAMPLES	100			// Number of training samples per class
#define FRAC_LINEAR_SEP		0.9f	    // Fraction of samples which compose the linear separable part

using namespace cv;
using namespace std;

class Recognition : public QObject
{
    Q_OBJECT
private:
    CvSVM       svm;
    CvSVMParams params;

    QString AllLettres;
    Mat     labels;

    QList<Mat> allWordsImages;
    Mat        charactersRecognized;
    QString    fileName;

public:
    explicit Recognition(QObject *parent = 0);

    float        diagonalAverage(Mat mask);
    QList<float> extractFeaturesVector(Mat image);
    void    setSVMParameters(int kernalType,double C,double degree,double gamma);
    int     countCXX(const Mat &image);
    float   calculateWHRation(const Mat &image);
    float   calculateHWRation(const Mat &image);

public slots:
    Mat     getTrainingSet(QList<Mat> CharactersSet);
    Mat     getTestingSet(QList<Mat> CharactersSet);
    void    trainTheMachine(Mat trainingSet);
    void    loadTrainingFile(QString fileName);
    void    saveTraining(QString fileName);
    void    loadLabels(QString labels);
    QString recognize(Mat TestingSet);
    QString recognizeTest(Mat TestingSet);

};

#endif // RECOGNITION_H
