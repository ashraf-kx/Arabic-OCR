#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "preprocess.h"
#include "recognition.h"
#include "segmentation.h"

#include <QFileDialog>
#include <QGraphicsItem>
#include <QImage>
#include <QDateTime>
#include <QtCore5Compat/QTextCodec>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <iostream>

#include <poppler/qt6/poppler-qt6.h> // install the package [pacman -Syu poppler-qt6] for <<Arch Linux>>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace Poppler;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Preprocess *preprocessing;
    Recognition *recognition;
    Segmentation *segmentation;

    Ui::MainWindow *ui;
    QString fileName;

    QGraphicsScene *scene;
    QGraphicsScene *sceneVerticalHistogram;
    QGraphicsScene *sceneHorizontalHistogram;

    QGraphicsPixmapItem *pixmapItem;
    QGraphicsPixmapItem *pixmapVerticalHistogram;
    QGraphicsPixmapItem *pixmapHorizontalHistogram;

    QImage sourceImage;
    QImage scaledImage;
    QImage binarizedImage;
    QImage imageUnderProcess;

    Mat sourceMat;
    Mat matUnderProcess;
    Mat grayMat;
    Mat rgbMat;
    Mat binarizedMat;
    Mat invertedBinarizedMat;
    Mat trainSet;
    Mat testingSet;

    QList<Mat> allCharacters;
    QList<Mat> allWordsImages;
    QList<Mat> charactersSet;

    // Mat _imgCountor;
    VideoCapture camera;
    bool exitCamera;

    int binVal;
    int numberPagePDF;
    int wordImageNumber;

    QList<int> histogramDataPoints;

    // PDF ATTRIBUTE.
    std::unique_ptr<Document> document;
    std::unique_ptr<Page> page;
    QSizeF PSizeF;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void display(const QImage &image, const QString message);
    void displayHistogram(const QImage &image);
    void activateButton(QPushButton *BT);
    void deactivateButton(QPushButton *BT);

    QImage mat2QImage(Mat const &src);
    Mat qImage2Mat(QImage const &src);

private slots:

    void browse();
    void loadPDF();
    void previousPage();
    void nextPage();
    void loadSpecificPage();
    void changeVisibility();

    //! SEGMENTATION
    void binarization(int threshold = 0);
    void thinning();
    void contour();
    void CXX();
    void skew();

    //! PREPROCESSUSING
    void segmentEntireDocument();
    void cutCharacters();

    //! RECOGNITION
    void extractTrainingData();
    void extractTestingData();
    void trainTheMachine();
    void recognize();
    void recognizeTest();

    void saveTraining();
    void loadTrainingFile();

    void startCamera();
    void stopCapture();

    void moveToNextWord();
    void moveToPreviousWord();

    void buttonsChangeColors();
    void loadLabels();
    void saveImage();
    void reset();
    void about();
};

#endif // MAINWINDOW_H
