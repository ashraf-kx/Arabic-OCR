#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showMaximized();
    this->setWindowIcon(QIcon(QPixmap(":/Icons/IconAOCR.png")));
    this->setWindowTitle("AOCR Release V 0.0.1");

    ui->widget->setMaximumWidth(200);
    ui->widgetTrainingSVM->setMaximumWidth(200);
    ui->widget->setEnabled(false);
    ui->viewVerticalHistogram->setMaximumWidth(0);

    scene = new QGraphicsScene();
    sceneVerticalHistogram = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
    ui->viewVerticalHistogram->setVisible(false);
    ui->TE_FinalText->setVisible(false);
    ui->widgetTrainingSVM->setVisible(false);

    preprocessing = new Preprocess();
    recognition = new Recognition();
    segmentation = new Segmentation();

    numberPagePDF = 0;

    ui->splitter->setHandleWidth(1);

    // Associate every Action with An Icon
    ui->actionOpen->setIcon(QIcon(QPixmap(":/Icons/Add.ico")));
    ui->actionRestart->setIcon(QIcon(QPixmap(":/Icons/Available-Updates.ico")));
    ui->actionShowPhoto->setIcon(QIcon(QPixmap(":/Icons/Cancel.ico")));
    ui->actionCapturePhoto->setIcon(QIcon(QPixmap(":/Icons/Camera.ico")));
    ui->actionSaveImage->setIcon(QIcon(QPixmap(":/Icons/saveImage.png")));
    ui->actionLoadPDF->setIcon(QIcon(QPixmap(":/Icons/IconPDF.png")));

    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSaveImage->setShortcut(QKeySequence::Save);
    ui->actionExit->setShortcut(QKeySequence::Close);
    ui->actionFullScreen->setShortcut(QKeySequence(Qt::CTRL, Qt::Key_F));
    ui->actionExitFullScreen->setShortcut(QKeySequence(Qt::CTRL, Qt::SHIFT, Qt::Key_F));

    // Actions
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(browse()));
    connect(ui->actionLoadPDF, SIGNAL(triggered()), this, SLOT(loadPDF()));
    connect(ui->actionRestart, SIGNAL(triggered()), this, SLOT(reset()));
    connect(ui->actionFullScreen, SIGNAL(triggered()), this, SLOT(showFullScreen()));
    connect(ui->actionExitFullScreen, SIGNAL(triggered()), this, SLOT(showMaximized()));
    connect(ui->actionSaveImage, SIGNAL(triggered()), this, SLOT(saveImage()));
    connect(ui->actionCapturePhoto, SIGNAL(triggered()), this, SLOT(startCamera()));
    connect(ui->actionShowPhoto, SIGNAL(triggered()), this, SLOT(stopCapture()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    // connect(ui->actionGet_All_Vectors_NOW,SIGNAL(triggered()),segmentation,SLOT(cutAllWords2AllCharacters())); // SPECIAL ONE >>

    // Sliders

    // connect(ui->horizontalSlider_threshold,SIGNAL(valueChanged(int)),ui->labelThreshold,SLOT(setNum(int)));

    // Buttons
    connect(ui->btnPdfBack, SIGNAL(clicked()), this, SLOT(previousPage()));
    connect(ui->btnPdfForward, SIGNAL(clicked()), this, SLOT(nextPage()));
    connect(ui->btnNextWord, SIGNAL(clicked()), this, SLOT(moveToNextWord()));
    connect(ui->btnPreviousWord, SIGNAL(clicked()), this, SLOT(moveToPreviousWord()));
    connect(ui->btnTrainingPhase, SIGNAL(clicked()), this, SLOT(changeVisibility()));
    connect(ui->spinboxPDF, SIGNAL(editingFinished()), this, SLOT(loadSpecificPage()));

    // Preprocessing
    connect(ui->btnBinarization, SIGNAL(clicked()), this, SLOT(binarization()));
    connect(ui->btnContourDetection, SIGNAL(clicked()), this, SLOT(contour()));
    connect(ui->btnThining, SIGNAL(clicked()), this, SLOT(thinning()));
    connect(ui->btnCXX, SIGNAL(clicked()), this, SLOT(CXX()));
    connect(ui->btnCorrectSkew, SIGNAL(clicked()), this, SLOT(skew()));
    connect(ui->sliderBinarization, SIGNAL(valueChanged(int)), this, SLOT(binarization(int)));

    // Segmentation
    connect(ui->btnSegmentIntoWords, SIGNAL(clicked()), this, SLOT(segmentEntireDocument()));
    connect(ui->btnCutWord, SIGNAL(clicked()), this, SLOT(cutCharacters()));

    // Recognition
    connect(ui->btnGetFeatureSetTraining, SIGNAL(clicked()), this, SLOT(extractTrainingData()));
    connect(ui->btnGetTestingSet, SIGNAL(clicked()), this, SLOT(extractTestingData()));
    connect(ui->btnTrainTheMachine, SIGNAL(clicked()), this, SLOT(trainTheMachine()));
    connect(ui->btnRecognizeCharacters, SIGNAL(clicked()), this, SLOT(recognize()));
    connect(ui->btnCheck, SIGNAL(clicked()), this, SLOT(recognizeTest()));

    connect(ui->btnLoadSVM, SIGNAL(clicked()), this, SLOT(loadTrainingFile()));
    connect(ui->btnSaveSVM, SIGNAL(clicked()), this, SLOT(saveTraining()));
    connect(ui->btnLoadLabels, SIGNAL(clicked()), this, SLOT(loadLabels()));
}

//! PREPROCESS
void MainWindow::binarization(int threshold)
{
    matUnderProcess = preprocessing->binarization(rgbMat, threshold);

    imageUnderProcess = QImage((uchar *)matUnderProcess.data, matUnderProcess.cols, matUnderProcess.rows, matUnderProcess.step, QImage::Format_Indexed8);
    imageUnderProcess.bits();
    display(imageUnderProcess, "image binarizated." + QString::number(threshold));
    activateButton(ui->btnBinarization);
}

void MainWindow::thinning()
{
    matUnderProcess = preprocessing->Thinning(matUnderProcess);

    imageUnderProcess = QImage((uchar *)matUnderProcess.data, matUnderProcess.cols, matUnderProcess.rows, matUnderProcess.step, QImage::Format_Indexed8);
    imageUnderProcess.bits();
    display(imageUnderProcess, "Thinning Done.");
    activateButton(ui->btnThining);
}

void MainWindow::contour()
{
    matUnderProcess = preprocessing->contour(matUnderProcess);

    imageUnderProcess = QImage((uchar *)matUnderProcess.data, matUnderProcess.cols, matUnderProcess.rows, matUnderProcess.step, QImage::Format_Indexed8);
    imageUnderProcess.bits();
    display(imageUnderProcess, "Detection Countor.");
    activateButton(ui->btnContourDetection);
}

void MainWindow::CXX()
{
    matUnderProcess = preprocessing->convert2cxx(matUnderProcess);

    imageUnderProcess = QImage((uchar *)matUnderProcess.data, matUnderProcess.cols, matUnderProcess.rows, matUnderProcess.step, QImage::Format_Indexed8);
    imageUnderProcess.bits();
    display(imageUnderProcess, "Connexe Compenent.");
    activateButton(ui->btnCXX);
}

void MainWindow::skew()
{
    matUnderProcess = preprocessing->computeSkewAndRotate(matUnderProcess);

    imageUnderProcess = QImage((uchar *)matUnderProcess.data, matUnderProcess.cols, matUnderProcess.rows, matUnderProcess.step, QImage::Format_Indexed8);
    imageUnderProcess.bits();
    display(imageUnderProcess, "Skew Corrected.");
    activateButton(ui->btnCorrectSkew);
}

//!  SEGMENTATION
void MainWindow::segmentEntireDocument()
{
    charactersSet.clear();
    charactersSet = segmentation->segmentEntireDocument(matUnderProcess);
    for (int i = 0; i < charactersSet.length(); ++i)
    {
        // imwrite("/home/ash/Desktop/XC1/Model/"+QString::number(i).toStdString()+".png",CharactersSet.at(i));
    }
    activateButton(ui->btnNextWord);
    activateButton(ui->btnPreviousWord);

    imageUnderProcess = QImage((uchar *)matUnderProcess.data, matUnderProcess.cols, matUnderProcess.rows, matUnderProcess.step, QImage::Format_Indexed8);
    imageUnderProcess.bits();
    display(imageUnderProcess, "Document Segmented Into " + QString::number(charactersSet.length()) + " Character.");
    activateButton(ui->btnSegmentIntoWords);
}

void MainWindow::cutCharacters()
{
    QList<Mat> CharactersSubSet;
    CharactersSubSet.clear();
    charactersSet.clear();

    QList<Mat> LinesSet = segmentation->getAllImagesLines(matUnderProcess);
    for (int i = 0; i < LinesSet.length(); i++)
    {
        CharactersSubSet = segmentation->cutLine2Characters(LinesSet, i);
        for (int j = CharactersSubSet.length() - 1; j >= 0; --j)
        {
            charactersSet << CharactersSubSet.at(j);
            // imwrite("/home/ash/Desktop/XC1/Characters/"+QString::number(CharactersSubSet.length()-1-j).toStdString()+".png",CharactersSubSet.at(j));
        }
        // imwrite("/home/ash/Desktop/XC1/Lines/"+QString::number(i).toStdString()+".png",LinesSet.at(i));
    }

    imageUnderProcess = QImage((uchar *)matUnderProcess.data, matUnderProcess.cols, matUnderProcess.rows, matUnderProcess.step, QImage::Format_Indexed8);
    imageUnderProcess.bits();
    display(imageUnderProcess, "Set Of " + QString::number(charactersSet.length()) + " Character Image Located.");
    activateButton(ui->btnCutWord);
}

//!                RECOGNITION
void MainWindow::extractTrainingData()
{
    trainSet = recognition->getTrainingSet(charactersSet);
    activateButton(ui->btnGetFeatureSetTraining);
    ui->statusBar->showMessage("Train Data Extracted.", 10000);
}

void MainWindow::extractTestingData()
{
    testingSet = recognition->getTestingSet(charactersSet);
    activateButton(ui->btnGetTestingSet);
    ui->statusBar->showMessage("Test Data Extracted.", 10000);
}

void MainWindow::trainTheMachine()
{
    ui->statusBar->showMessage("Wait Please ......");
    recognition->setSVMParameters(ui->svmKernal->currentIndex(), ui->svmC->text().toDouble(), ui->svmDegree->text().toDouble(), ui->svmGamma->text().toDouble());
    recognition->trainTheMachine(trainSet);
    activateButton(ui->btnTrainTheMachine);
    ui->statusBar->showMessage("Machine Trained.", 10000);
}

void MainWindow::recognize()
{
    /// if()
    QString Result = recognition->recognize(testingSet);
    ui->TE_FinalText->clear();
    ui->TE_FinalText->setText(Result);
    ui->TE_FinalText->setVisible(true);
    activateButton(ui->btnRecognizeCharacters);
    ui->statusBar->showMessage("recognition Done.", 10000);
}

void MainWindow::recognizeTest()
{
    QString Result = recognition->recognizeTest(trainSet);
    ui->TE_FinalText->clear();
    ui->TE_FinalText->setText(Result);
    ui->TE_FinalText->setVisible(true);
    activateButton(ui->btnCheck);
    ui->statusBar->showMessage("recognition Done.", 10000);
}

void MainWindow::saveTraining()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Save As"));
    recognition->saveTraining(fileName);
    ui->statusBar->showMessage("training file saved.", 10000);
    activateButton(ui->btnSaveSVM);
}

void MainWindow::loadTrainingFile()
{
    fileName = QFileDialog::getOpenFileName(this, "Choose Training File", QDir::homePath(), "(*.xml)");
    recognition->loadTrainingFile(fileName);
    ui->statusBar->showMessage("file SVM  training loaded.", 10000);
    activateButton(ui->btnLoadSVM);
}

void MainWindow::loadLabels()
{
    fileName = QFileDialog::getOpenFileName(this, "Choose Labels File", QDir::homePath(), "(*.txt)");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            QString labels = in.readAll();
            recognition->loadLabels(labels);
            activateButton(ui->btnLoadLabels);
            ui->statusBar->showMessage("Labels loaded.", 15000);
        }
    }
}

void MainWindow::loadPDF()
{
    fileName = QFileDialog::getOpenFileName(this, "Choose your PDF File", QDir::homePath(), "*.pdf");
    if (!fileName.isEmpty())
    {
        document = Document::load(fileName);
        document->setRenderHint(Poppler::Document::TextAntialiasing);
        page = document->page(0);
        qDebug() << "PDF page size: " + QString::number(page->pageSize().width()) + "x" + QString::number(page->pageSize().height());
        // NOTE: Paramtres for "renderToImage(1,1)" result in image size : 8.5x11 (Do the math).
        imageUnderProcess = page->renderToImage(900 / 8.5, 1360 / 11);
        rgbMat = qImage2Mat(imageUnderProcess);
        qDebug() << "Image size (rendered from PDF): " + QString::number(rgbMat.size().width) + "x" + QString::number(rgbMat.size().height);
        ui->pdfLabel->setText(QString::number(document->numPages()));
        display(page->renderToImage(900 / 8.5, 1360 / 11), "PDF displayed as an image at a resolution of : " + QString::number(imageUnderProcess.format()));
        ui->widget->setEnabled(true);
        ui->widget->setVisible(true);
        ui->spinboxPDF->setMaximum(document->numPages());
        ui->spinboxPDF->setMinimum(0);
        buttonsChangeColors();
        activateButton(ui->btnPdfForward);
        activateButton(ui->btnPdfBack);
    }
}

void MainWindow::loadSpecificPage()
{
    numberPagePDF = ui->spinboxPDF->value();
    if (document->numPages() > numberPagePDF)
    {
        page = document->page(numberPagePDF);
        PSizeF = page->pageSizeF();
        imageUnderProcess = page->renderToImage(900 / 8.5, 1360 / 11);
        rgbMat = qImage2Mat(imageUnderProcess);
        display(imageUnderProcess, "PDF page : " + QString::number(numberPagePDF));
        ui->spinboxPDF->setValue(numberPagePDF);
    }
}

void MainWindow::nextPage()
{
    numberPagePDF++;
    if (document->numPages() > numberPagePDF)
    {
        page = document->page(numberPagePDF);
        PSizeF = page->pageSizeF();
        imageUnderProcess = page->renderToImage(900 / 8.5, 1360 / 11);
        rgbMat = qImage2Mat(imageUnderProcess);
        display(imageUnderProcess, "PDF page : " + QString::number(numberPagePDF));
        ui->spinboxPDF->setValue(numberPagePDF);
    }
    else
        numberPagePDF--;
}

void MainWindow::previousPage()
{
    numberPagePDF--;
    if (numberPagePDF > 0)
    {
        page = document->page(numberPagePDF);
        PSizeF = page->pageSizeF();
        imageUnderProcess = page->renderToImage();
        rgbMat = qImage2Mat(imageUnderProcess);
        display(imageUnderProcess, "PDF page : " + QString::number(numberPagePDF));
        ui->spinboxPDF->setValue(numberPagePDF);
    }
    else
        numberPagePDF++;
}

void MainWindow::moveToNextWord()
{
    wordImageNumber++;
    if (wordImageNumber < charactersSet.length())
    {
        binarizedMat = charactersSet.at(wordImageNumber);
        imageUnderProcess = QImage((uchar *)binarizedMat.data, binarizedMat.cols, binarizedMat.rows, binarizedMat.step, QImage::Format_Indexed8);
        imageUnderProcess.bits();

        ui->labelWordsCount->setText(QString::number(wordImageNumber + 1) + " /" + QString::number(charactersSet.length()));
        display(imageUnderProcess, "Word number :" + QString::number(wordImageNumber));
    }
    else
    {
        wordImageNumber = charactersSet.length() - 1;
    }
}

void MainWindow::moveToPreviousWord()
{
    wordImageNumber--;
    if (wordImageNumber >= 0)
    {
        binarizedMat = charactersSet.at(wordImageNumber);
        imageUnderProcess = QImage((uchar *)binarizedMat.data, binarizedMat.cols, binarizedMat.rows, binarizedMat.step, QImage::Format_Indexed8);
        imageUnderProcess.bits();

        ui->labelWordsCount->setText(QString::number(wordImageNumber) + " /" + QString::number(charactersSet.length()));
        display(imageUnderProcess, "Word number :" + QString::number(wordImageNumber));
    }
    else
    {
        wordImageNumber = 0;
    }
}

void MainWindow::startCamera()
{
    ui->actionCapturePhoto->setDisabled(true);
    this->exitCamera = true;
    VideoCapture _WebCam(0); // Open The Default Camera.
    if (!_WebCam.isOpened()) // Check If We Succeeded.
        ui->statusBar->showMessage("Failed");
    do
    {
        Mat frame;
        Mat _tmp;
        _WebCam >> frame;
        imshow("Origine : ", frame); // get a new frame from camera
        rgbMat = frame;
        GaussianBlur(frame, matUnderProcess, Size(1, 3), 0, 0);
        cvtColor(matUnderProcess, matUnderProcess, CV_BGR2RGB);
        cvtColor(matUnderProcess, grayMat, CV_RGB2GRAY);
        //! [ MAKE Choice between (Otsu<Globale> || Gaussian< Adaptative> ]  04/03/2015  19:36.
        threshold(grayMat, _tmp, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);                             // Gloable Otsu.
        adaptiveThreshold(grayMat, binarizedMat, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 11, 2); // Adaptative Gausian.

        imshow("Otsu Binarization", preprocessing->copyRect(_tmp, 25, 25, 480, 480));
        // imshow("Gaussian+ Binarization", _imgBW);

        if (waitKey(30) >= 0)
            break;
    } while (this->exitCamera);
    // the camera will be deinitialized automatically in VideoCapture destructor
}

void MainWindow::stopCapture()
{
    this->exitCamera = false;
    ui->widget->setEnabled(true);
    ui->actionCapturePhoto->setDisabled(false);
}

//! >>>>>>>>>>>>>>>>>>>>>>>>>> EXTRA METHODES TO HELP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
QImage MainWindow::mat2QImage(Mat const &src)
{
    Mat temp;                        // make the same Mat
    cvtColor(src, temp, CV_BGR2RGB); // cvtColor Makes a copt, that what i need
    QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits(); // enforce deep copy, see documentation
    // of QImage::QImage ( const uchar * data, int width, int height, Format format )
    return dest;
}

Mat MainWindow::qImage2Mat(QImage const &src)
{
    Mat tmp(src.height(), src.width(), CV_8UC4, (uchar *)src.bits(), src.bytesPerLine());
    cvtColor(tmp, tmp, CV_RGBA2RGB);
    return tmp;
}

void MainWindow::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"));
    if (!fileName.isEmpty() && !imageUnderProcess.isNull())
    {
        QDateTime dateTime = QDateTime::currentDateTime();
        QString dateTimeString = dateTime.toString("ss mm HH ddd MMMM d yy");
        imageUnderProcess.save(fileName + " " + dateTimeString + ".png");
        ui->statusBar->showMessage("Picture Saved.", 10000);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About AOCR"),
                       tr("Developed by <b>A</b>chraf <br> K."
                          "Version 1.0.0 <br>"
                          "Copyright 2014-2015."));
}

void MainWindow::display(const QImage &image, const QString message)
{
    pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene->clear();
    scene->addItem(pixmapItem);
    ui->graphicsView->update();
    QString msg = message + " :" + QString::number(image.width()) + "X" + QString::number(image.height());
    ui->statusBar->showMessage(msg, 10000);
}

void MainWindow::displayHistogram(const QImage &image)
{
    pixmapVerticalHistogram = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    sceneVerticalHistogram->clear();
    sceneVerticalHistogram->addItem(pixmapVerticalHistogram);
    ui->viewVerticalHistogram->setScene(sceneVerticalHistogram);
    ui->viewVerticalHistogram->setMinimumWidth(10);
    ui->viewVerticalHistogram->update();
}

void MainWindow::browse()
{
    fileName = QFileDialog::getOpenFileName(this, "Choose your image", QDir::homePath(), "Image files (*.png *.xpm *.jpg *.bmp)");
    if (!fileName.isEmpty())
    {
        ui->widget->setVisible(true);
        ui->widget->setEnabled(true);

        rgbMat = imread(fileName.toStdString());
        grayMat = imread(fileName.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

        sourceImage = mat2QImage(rgbMat);
        imageUnderProcess = sourceImage;

        //! Display Image.
        display(imageUnderProcess, "Image loaded.");
        buttonsChangeColors();
    }
}

void MainWindow::reset()
{
    ui->widget->setVisible(false);
    ui->viewVerticalHistogram->setMaximumWidth(0);
    ui->TE_FinalText->setVisible(false);
    scene->clear();

    sourceMat.release();
    matUnderProcess.release();
    grayMat.release();
    rgbMat.release();
    binarizedMat.release();
    invertedBinarizedMat.release();
    // Visualization_V.release();
    // Visualization_H.release();

    // geoLines.clear();
    // geoWords.clear();
    allWordsImages.clear();
    histogramDataPoints.clear();
    fileName.clear();

    document = NULL;

    binVal = 0;
    ui->widget->setEnabled(false);
    ui->labelStatus->clear();
}

void MainWindow::changeVisibility()
{
    ui->widgetTrainingSVM->setVisible(!ui->widgetTrainingSVM->isVisible());
    // DESIGNER >> 2015
    if (!ui->widgetTrainingSVM->isVisible())
        deactivateButton(ui->btnTrainingPhase);
    else
        activateButton(ui->btnTrainingPhase);
}

void MainWindow::buttonsChangeColors()
{
    QString styleScriptCSS = "position: relative;padding: 7px 12px;padding-top: 7px;padding-bottom: 7px;\
            font-size: 15px;font-family: Ubuntu;color: #ffffff;white-space: nowrap;\
            background-color: #a5a5a5; border-radius: 3px;border-top-left-radius: 3px;\
            border-top-right-radius: 3px;border-bottom-right-radius: 3px;border-bottom-left-radius: 3px;\
            border: 1px solid #a5a5a5;border-top-width: 1px;border-bottom-width: 1px;\
            border-top-style: solid;border-bottom-style: solid; vertical-align: middle;";
    QString styleScriptCSS_EXP = "position: relative;padding: 7px 12px;padding-top: 7px;padding-bottom: 7px;\
            font-size: 12px;font-family: Ubuntu;color: #ffffff;white-space: nowrap;\
            background-color: #a5a5a5; border-radius: 3px;border-top-left-radius: 3px;\
            border-top-right-radius: 3px;border-bottom-right-radius: 3px;border-bottom-left-radius: 3px;\
            border: 1px solid #a5a5a5;border-top-width: 1px;border-bottom-width: 1px;\
            border-top-style: solid;border-bottom-style: solid; vertical-align: middle;";

    ui->btnBinarization->setStyleSheet(styleScriptCSS);
    ui->btnContourDetection->setStyleSheet(styleScriptCSS);
    ui->btnCorrectSkew->setStyleSheet(styleScriptCSS);
    ui->btnCutWord->setStyleSheet(styleScriptCSS);
    ui->btnCXX->setStyleSheet(styleScriptCSS);
    ui->btnGetTestingSet->setStyleSheet(styleScriptCSS);
    ui->btnLoadLabels->setStyleSheet(styleScriptCSS);
    ui->btnNextWord->setStyleSheet(styleScriptCSS);
    ui->btnPdfBack->setStyleSheet(styleScriptCSS);
    ui->btnPdfForward->setStyleSheet(styleScriptCSS);
    ui->btnPreviousWord->setStyleSheet(styleScriptCSS);
    ui->btnRecognizeCharacters->setStyleSheet(styleScriptCSS);
    ui->btnSegmentIntoWords->setStyleSheet(styleScriptCSS_EXP);
    ui->btnThining->setStyleSheet(styleScriptCSS);
    ui->btnTrainingPhase->setStyleSheet(styleScriptCSS);
    ui->btnCheck->setStyleSheet(styleScriptCSS);
    ui->btnGetFeatureSetTraining->setStyleSheet(styleScriptCSS_EXP);
    ui->btnLoadSVM->setStyleSheet(styleScriptCSS);
    ui->btnSaveSVM->setStyleSheet(styleScriptCSS);
    ui->btnTrainTheMachine->setStyleSheet(styleScriptCSS);
}

void MainWindow::activateButton(QPushButton *BT)
{
    QString styleScriptCSS = "position: relative;padding: 7px 12px;padding-top: 7px;padding-bottom: 7px;\
            font-size: 15px;font-family: Ubuntu;color: #ffffff;white-space: nowrap;\
            background-color: #E05A2B; border-radius: 3px;border-top-left-radius: 3px;\
            border-top-right-radius: 3px;border-bottom-right-radius: 3px;border-bottom-left-radius: 3px;\
            border: 1px solid #E05A2B;border-top-width: 1px;border-bottom-width: 1px;\
            border-top-style: solid;border-bottom-style: solid; vertical-align: middle;";
    QString styleScriptCSS_EXE = "position: relative;padding: 7px 12px;padding-top: 7px;padding-bottom: 7px;\
            font-size: 12px;font-family: Ubuntu;color: #ffffff;white-space: nowrap;\
            background-color: #E05A2B; border-radius: 3px;border-top-left-radius: 3px;\
            border-top-right-radius: 3px;border-bottom-right-radius: 3px;border-bottom-left-radius: 3px;\
            border: 1px solid #E05A2B;border-top-width: 1px;border-bottom-width: 1px;\
            border-top-style: solid;border-bottom-style: solid; vertical-align: middle;";

    if (BT == ui->btnSegmentIntoWords || BT == ui->btnGetFeatureSetTraining)
        BT->setStyleSheet(styleScriptCSS_EXE);
    else
        BT->setStyleSheet(styleScriptCSS);
}

void MainWindow::deactivateButton(QPushButton *BT)
{
    QString styleScriptCSS = "position: relative;padding: 7px 12px;padding-top: 7px;padding-bottom: 7px;\
                                        font-size: 15px;font-family: Ubuntu;color: #ffffff;white-space: nowrap;\
                                        background-color: #a5a5a5; border-radius: 3px;border-top-left-radius: 3px;\
                                        border-top-right-radius: 3px;border-bottom-right-radius: 3px;border-bottom-left-radius: 3px;\
                                        border: 1px solid #a5a5a5;border-top-width: 1px;border-bottom-width: 1px;\
                                        border-top-style: solid;border-bottom-style: solid; vertical-align: middle;";
    BT->setStyleSheet(styleScriptCSS);
}

MainWindow::~MainWindow()
{
    delete ui;
}
