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
#include <poppler/qt6/poppler-qt6.h>   // install the package [pacman -Syu poppler-qt6] for <<Arch Linux>>

//////////////////////// INCLUDE OPEN Computer Vision /////////////////////
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml//ml.hpp>
////////////////////////            END               /////////////////////

using namespace std;
using namespace Poppler;
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Preprocess    *preprocessing;
    Recognition   *recognition;
    Segmentation  *segmentation;

    Ui::MainWindow      *ui;
    QString             fileName;

    QGraphicsScene      *scene;
    QGraphicsPixmapItem *item_pixmapIMG;

    // Vertical Histogram
    QGraphicsScene      *sceneVHist;
    QGraphicsPixmapItem *item_pixmapIMG_VHist;

    // Horizontal Histogram
    QGraphicsScene      *sceneHHist;
    QGraphicsPixmapItem *item_pixmapIMG_HHist;

    // images used.
    QImage imgOrigin;
    QImage imgScaled;
    QImage imgBinarized;
    QImage imgInProcessus;

    // OpenCV
    Mat _imgOrigin;
    Mat _imgInProcessus;
    Mat _imgGray;
    Mat _imgRGB;
    Mat _imgBW;
    Mat _imgBX_INV;

    Mat trainSet;
    Mat testingSet;

    QList<Mat> allWordsImages;
    QList<Mat> CharactersSet;

    //Mat _imgCountor;
    VideoCapture _WebCam;

    bool exitCam;

    //!
    int binVal;
    int numberPagePDF;
    int wordImageNumber;

    QList<Mat> ALLCHARACTERS;

    // Histogram Data.
    QList<int>  histData;

    // PDF ATTRIBUTE.
    Document *document;
    Page     *page;
    QSizeF    PSizeF;

//!  >>>>>>>>>>>>>>>>>>>>>>>>>> METHODES >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void   display(const QImage& image,const QString message);
    void   displayHistogramme(const QImage& image);
    void   activateButton(QPushButton *BT);
    void   deactivateButton(QPushButton *BT);

    QImage Mat2QImage(Mat const& src);
    Mat    QImage2Mat(QImage const& src);

private slots:

    void browse();
    void loadPDF();
    void previous_page();
    void next_page();
    void loadSpecificPage();
    void changeVisibility();

    //! SEGMENTATION
    void binarizationOTSU();
    void binarizationGLOBALE(int Threshold);
    void thinning();
    void countor();
    void CXX();
    void skew();

    //! PREPROCESSUSING
    void segmenteEntierDocument();
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
    void saveImg();
    void reset();
    void about();

};

#endif // MAINWINDOW_H
