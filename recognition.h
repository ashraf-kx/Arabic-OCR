#ifndef RECOGNITION_H
#define RECOGNITION_H


#include "segmentation.h"

#include <QObject>
#include <QTextCodec>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <iostream>
#include <QRegularExpression>

#include <opencv2/opencv.hpp>

#define	NTRAINING_SAMPLES	100			// Number of training samples per class
#define FRAC_LINEAR_SEP		0.9f	    // Fraction of samples which compose the linear separable part

using namespace std;
using namespace cv;
using namespace ml;

class Recognition : public QObject
{
    Q_OBJECT
private:
    Ptr<SVM> svm;

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
