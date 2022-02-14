#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showMaximized();
    this->setWindowIcon(QIcon(QPixmap(":/Icons/IconAOCR.png")));
    this->setWindowTitle("AOCR Release V 0.0.1");

    ui->widget->setMaximumWidth(195);
    ui->W_trainingSVM->setMaximumWidth(195);
    ui->widget->setEnabled(false);
    ui->graphicsView_VHist->setMaximumWidth(0);

    scene = new QGraphicsScene();
    sceneVHist = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
    ui->graphicsView_VHist->setVisible(false);
    ui->TE_FinalText->setVisible(false);
    ui->W_trainingSVM->setVisible(false);

    preprocessing = new Preprocess();
    recognition   = new Recognition();
    segmentation  = new Segmentation();

    numberPagePDF = 0;

    ui->splitter->setHandleWidth(1);

    // Associate every Action with An Icon
    ui->actionOpen->setIcon(QIcon(QPixmap(":/Icons/Add.ico")));
    ui->actionRestart->setIcon(QIcon(QPixmap(":/Icons/Available-Updates.ico")));
    ui->actionShow_photo->setIcon(QIcon(QPixmap(":/Icons/Cancel.ico")));
    ui->actionCapture_photo->setIcon(QIcon(QPixmap(":/Icons/Camera.ico")));
    ui->actionSave_Image->setIcon(QIcon(QPixmap(":/Icons/saveImage.png")));
    ui->action_Load_PDF->setIcon(QIcon(QPixmap(":/Icons/IconPDF.png")));

    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave_Image->setShortcut(QKeySequence::Save);
    ui->actionExit->setShortcut(QKeySequence::Close);
    ui->actionFullScreen->setShortcut(QKeySequence(Qt::CTRL, Qt::Key_F));
    ui->actionExit_FullScreen->setShortcut(QKeySequence(Qt::CTRL, Qt::SHIFT, Qt::Key_F));

    // Actions
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(browse()));
    connect(ui->action_Load_PDF,SIGNAL(triggered()),this,SLOT(loadPDF()));
    connect(ui->actionRestart,SIGNAL(triggered()),this,SLOT(reset()));
    connect(ui->actionFullScreen,SIGNAL(triggered()),this,SLOT(showFullScreen()));
    connect(ui->actionExit_FullScreen,SIGNAL(triggered()),this,SLOT(showMaximized()));
    connect(ui->actionSave_Image,SIGNAL(triggered()),this,SLOT(saveImg()));
    connect(ui->actionCapture_photo,SIGNAL(triggered()),this,SLOT(startCamera()));
    connect(ui->actionShow_photo,SIGNAL(triggered()),this,SLOT(stopCapture()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(about()));
    //connect(ui->actionGet_All_Vectors_NOW,SIGNAL(triggered()),segmentation,SLOT(cutAllWords2AllCharacters())); // SPECIAL ONE >>

    // Sliders

    //connect(ui->horizontalSlider_threshold,SIGNAL(valueChanged(int)),ui->labelThreshold,SLOT(setNum(int)));

    // Buttons
    connect(ui->Bt_PDF_Back,SIGNAL(clicked()),this,SLOT(previous_page()));
    connect(ui->Bt_PDF_Forword,SIGNAL(clicked()),this,SLOT(next_page()));
    connect(ui->Bt_next_word,SIGNAL(clicked()),this,SLOT(moveToNextWord()));
    connect(ui->Bt_previous_word,SIGNAL(clicked()),this,SLOT(moveToPreviousWord()));
    connect(ui->Bt_trainingPhase,SIGNAL(clicked()),this,SLOT(changeVisibility()));
    connect(ui->SB_PDF,SIGNAL(editingFinished()),this,SLOT(loadSpecificPage()));

    // Preprocessing
    connect(ui->Bt_2binarization,SIGNAL(clicked()),this,SLOT(binarizationOTSU()));
    connect(ui->Bt_ContourDetection,SIGNAL(clicked()),this,SLOT(countor()));
    connect(ui->Bt_thining,SIGNAL(clicked()),this,SLOT(thinning()));
    connect(ui->Bt_CXX,SIGNAL(clicked()),this,SLOT(CXX()));
    connect(ui->Bt_CorrectSkew,SIGNAL(clicked()),this,SLOT(skew()));
    connect(ui->HSliderBinarization,SIGNAL(valueChanged(int)),this,SLOT(binarizationGLOBALE(int)));

    // Segmentation
    connect(ui->Bt_segmente_to_words,SIGNAL(clicked()),this,SLOT(segmenteEntierDocument()));
    connect(ui->Bt_cut_word,SIGNAL(clicked()),this,SLOT(cutCharacters()));

    // Recognition
    connect(ui->BT_getFeatureSetTraining,SIGNAL(clicked()),this,SLOT(extractTrainingData()));
    connect(ui->Bt_getTestingSet,SIGNAL(clicked()),this,SLOT(extractTestingData()));
    connect(ui->BT_trainTheMachine,SIGNAL(clicked()),this,SLOT(trainTheMachine()));
    connect(ui->Bt_recognize_characters,SIGNAL(clicked()),this,SLOT(recognize()));
    connect(ui->BT_check,SIGNAL(clicked()),this,SLOT(recognizeTest()));

    connect(ui->BT_loadSVM,SIGNAL(clicked()),this,SLOT(loadTrainingFile()));
    connect(ui->BT_saveSVM,SIGNAL(clicked()),this,SLOT(saveTraining()));
    connect(ui->Bt_LoadLabels,SIGNAL(clicked()),this,SLOT(loadLabels()));

}

//!                PREPROCESSUS
void MainWindow::binarizationOTSU()
{
    _imgInProcessus = preprocessing->binarizationOTSU(_imgRGB);

    imgInProcessus = QImage((uchar*) _imgInProcessus.data,_imgInProcessus.cols,_imgInProcessus.rows,_imgInProcessus.step,QImage::Format_Indexed8);
    imgInProcessus.bits();
    display(imgInProcessus,"image binarizated.");
    activateButton(ui->Bt_2binarization);
}

void MainWindow::binarizationGLOBALE(int Threshold)
{
    _imgInProcessus = preprocessing->binarizationGLOBALE(_imgRGB,Threshold);

    imgInProcessus = QImage((uchar*) _imgInProcessus.data,_imgInProcessus.cols,_imgInProcessus.rows,_imgInProcessus.step,QImage::Format_Indexed8);
    imgInProcessus.bits();
    display(imgInProcessus,"image binarizated."+QString::number(Threshold));
    activateButton(ui->Bt_2binarization);
}

void MainWindow::thinning()
{
    _imgInProcessus = preprocessing->Thinning(_imgInProcessus);

    imgInProcessus = QImage((uchar*) _imgInProcessus.data,_imgInProcessus.cols,_imgInProcessus.rows,_imgInProcessus.step,QImage::Format_Indexed8);
    imgInProcessus.bits();
    display(imgInProcessus,"Thinning Done.");
    activateButton(ui->Bt_thining);
}

void MainWindow::countor()
{
    _imgInProcessus = preprocessing->contour(_imgInProcessus);

    imgInProcessus = QImage((uchar*) _imgInProcessus.data,_imgInProcessus.cols,_imgInProcessus.rows,_imgInProcessus.step,QImage::Format_Indexed8);
    imgInProcessus.bits();
    display(imgInProcessus,"Detection Countor.");
    activateButton(ui->Bt_ContourDetection);
}

void MainWindow::CXX()
{
    _imgInProcessus = preprocessing->convert2cxx(_imgInProcessus);

    imgInProcessus = QImage((uchar*) _imgInProcessus.data,_imgInProcessus.cols,_imgInProcessus.rows,_imgInProcessus.step,QImage::Format_Indexed8);
    imgInProcessus.bits();
    display(imgInProcessus,"Connexe Compenent.");
    activateButton(ui->Bt_CXX);
}

void MainWindow::skew()
{
    _imgInProcessus = preprocessing->computeSkewAndRotate(_imgInProcessus);

    imgInProcessus = QImage((uchar*) _imgInProcessus.data,_imgInProcessus.cols,_imgInProcessus.rows,_imgInProcessus.step,QImage::Format_Indexed8);
    imgInProcessus.bits();
    display(imgInProcessus,"Skew Corrected.");
    activateButton(ui->Bt_CorrectSkew);
}

//!  SEGMENTATION
void MainWindow::segmenteEntierDocument()
{
    CharactersSet.clear();
    CharactersSet = segmentation->segmenteEntierDocument(_imgInProcessus);
    for (int i = 0; i < CharactersSet.length(); ++i) {
        //imwrite("/home/ash/Desktop/XC1/Model/"+QString::number(i).toStdString()+".png",CharactersSet.at(i));
    }
    activateButton(ui->Bt_next_word);
    activateButton(ui->Bt_previous_word);

    imgInProcessus = QImage((uchar*) _imgInProcessus.data,_imgInProcessus.cols,_imgInProcessus.rows,_imgInProcessus.step,QImage::Format_Indexed8);
    imgInProcessus.bits();
    display(imgInProcessus,"Document Segmented Into "+QString::number(CharactersSet.length())+" Character.");
    activateButton(ui->Bt_segmente_to_words);
}

void MainWindow::cutCharacters()
{
    QList<Mat> CharactersSubSet;
    CharactersSubSet.clear();
    CharactersSet.clear();

    QList<Mat> LinesSet = segmentation->getAllImagesLines(_imgInProcessus);
    for(int i=0; i<LinesSet.length(); i++){
        CharactersSubSet = segmentation->cutLine2Characters(LinesSet,i);
        for (int j = CharactersSubSet.length()-1; j >=0 ; --j) {
            CharactersSet<<CharactersSubSet.at(j);
            //imwrite("/home/ash/Desktop/XC1/Characters/"+QString::number(CharactersSubSet.length()-1-j).toStdString()+".png",CharactersSubSet.at(j));
        }
        //imwrite("/home/ash/Desktop/XC1/Lines/"+QString::number(i).toStdString()+".png",LinesSet.at(i));
    }

    imgInProcessus = QImage((uchar*) _imgInProcessus.data,_imgInProcessus.cols,_imgInProcessus.rows,_imgInProcessus.step,QImage::Format_Indexed8);
    imgInProcessus.bits();
    display(imgInProcessus,"Set Of "+ QString::number(CharactersSet.length())+" Character Image Located.");
    activateButton(ui->Bt_cut_word);
}

//!                RECOGNITION
void MainWindow::extractTrainingData()
{
    trainSet = recognition->getTrainingSet(CharactersSet);
    activateButton(ui->BT_getFeatureSetTraining);
    ui->statusBar->showMessage("Train Data Extracted.",10000);
}

void MainWindow::extractTestingData()
{
    testingSet = recognition->getTestingSet(CharactersSet);
    activateButton(ui->Bt_getTestingSet);
    ui->statusBar->showMessage("Test Data Extracted.",10000);
}

void MainWindow::trainTheMachine()
{
    ui->statusBar->showMessage("Wait Please ......");
    recognition->setSVMParameters(ui->CB_kernal->currentIndex(),ui->LE_c->text().toDouble(),ui->LE_degree->text().toDouble(),ui->LE_gamma->text().toDouble());
    recognition->trainTheMachine(trainSet);
    activateButton(ui->BT_trainTheMachine);
    ui->statusBar->showMessage("Machine Trained.",10000);
}

void MainWindow::recognize()
{
    ///if()
    QString Result = recognition->recognize(testingSet);
    ui->TE_FinalText->clear();
    ui->TE_FinalText->setText(Result);
    ui->TE_FinalText->setVisible(true);
    activateButton(ui->Bt_recognize_characters);
    ui->statusBar->showMessage("recognition Done.",10000);
}

void MainWindow::recognizeTest()
{
    QString Result = recognition->recognizeTest(trainSet);
    ui->TE_FinalText->clear();
    ui->TE_FinalText->setText(Result);
    ui->TE_FinalText->setVisible(true);
    activateButton(ui->BT_check);
    ui->statusBar->showMessage("recognition Done.",10000);
}

void MainWindow::saveTraining()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Save As"));
    recognition->saveTraining(fileName);
    ui->statusBar->showMessage("training file saved.", 10000);
    activateButton(ui->BT_saveSVM);
}

void MainWindow::loadTrainingFile()
{
    fileName = QFileDialog::getOpenFileName(this,"Choose Training File",QDir::homePath(),"(*.xml)");
    recognition->loadTrainingFile(fileName);
    ui->statusBar->showMessage("file SVM  training loaded.", 10000);
    activateButton(ui->BT_loadSVM);
}

void MainWindow::loadLabels()
{
    fileName = QFileDialog::getOpenFileName(this,"Choose Labels File",QDir::homePath(),"(*.txt)");
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            QString labels = in.readAll();
            recognition->loadLabels(labels);
            activateButton(ui->Bt_LoadLabels);
            ui->statusBar->showMessage("Labels loaded.", 15000);
        }
    }
}

//! >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PDF Methods Only  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void MainWindow::loadPDF()
{
    fileName = QFileDialog::getOpenFileName(this,"Choose your PDF File",QDir::homePath(),"*.pdf");
    if(!fileName.isEmpty())
    {
         document = Document::load(fileName);
         document->setRenderHint(Poppler::Document::TextAntialiasing);
         page = document->page(0);
         qDebug()<<"PDF page size: "+QString::number(page->pageSize().width())+"x"+QString::number(page->pageSize().height());
         // NOTE: Paramtres for "renderToImage(1,1)" result in image size : 8.5x11 (Do the math).
         imgInProcessus = page->renderToImage(900/8.5,1360/11);
         _imgRGB = QImage2Mat(imgInProcessus);
         qDebug()<<"Image size (rendered from PDF): "+QString::number(_imgRGB.size().width)+"x"+QString::number(_imgRGB.size().height);
         ui->PDF_Label->setText(QString::number(document->numPages()));
         display(page->renderToImage(900/8.5,1360/11),"PDF displayed as an image at a resolution of : "+QString::number(imgInProcessus.format()));
         ui->widget->setEnabled(true);
         ui->widget->setVisible(true);
         ui->SB_PDF->setMaximum(document->numPages());
         ui->SB_PDF->setMinimum(0);
         buttonsChangeColors();
         activateButton(ui->Bt_PDF_Forword);
         activateButton(ui->Bt_PDF_Back);
    }
}

void MainWindow::loadSpecificPage(){
    numberPagePDF = ui->SB_PDF->value();
    if(document->numPages() > numberPagePDF)
    {
        page = document->page(numberPagePDF);
        PSizeF = page->pageSizeF();
        imgInProcessus = page->renderToImage(900/8.5,1360/11);
        _imgRGB = QImage2Mat(imgInProcessus);
        display(imgInProcessus,"PDF page : "+QString::number(numberPagePDF));
        ui->SB_PDF->setValue(numberPagePDF);
    }
}

void MainWindow::next_page()
{
    numberPagePDF++;
    if(document->numPages() > numberPagePDF)
    {
        page = document->page(numberPagePDF);
        PSizeF = page->pageSizeF();
        imgInProcessus = page->renderToImage(900/8.5,1360/11);
        _imgRGB = QImage2Mat(imgInProcessus);
        display(imgInProcessus,"PDF page : "+QString::number(numberPagePDF));
        ui->SB_PDF->setValue(numberPagePDF);
    }
    else
        numberPagePDF--;
}

void MainWindow::previous_page()
{
    numberPagePDF--;
    if(numberPagePDF > 0)
    {
        page = document->page(numberPagePDF);
        PSizeF = page->pageSizeF();
        imgInProcessus = page->renderToImage();
        _imgRGB = QImage2Mat(imgInProcessus);
        display(imgInProcessus,"PDF page : "+QString::number(numberPagePDF));
        ui->SB_PDF->setValue(numberPagePDF);
    }else
        numberPagePDF++;
}

void MainWindow::moveToNextWord()
{
    wordImageNumber++;
    if(wordImageNumber < CharactersSet.length()){
        _imgBW = CharactersSet.at(wordImageNumber);
        imgInProcessus = QImage((uchar*) _imgBW.data,_imgBW.cols,_imgBW.rows,_imgBW.step,QImage::Format_Indexed8);
        imgInProcessus.bits();

        ui->Lb_nbrWords->setText(QString::number(wordImageNumber+1)+" /"+QString::number(CharactersSet.length()));
        display(imgInProcessus,"Word number :"+QString::number(wordImageNumber));

    }else { wordImageNumber = CharactersSet.length()-1 ;}

}

void MainWindow::moveToPreviousWord()
{
    wordImageNumber--;
    if(wordImageNumber >= 0){
        _imgBW = CharactersSet.at(wordImageNumber);
        imgInProcessus = QImage((uchar*) _imgBW.data,_imgBW.cols,_imgBW.rows,_imgBW.step,QImage::Format_Indexed8);
        imgInProcessus.bits();

        ui->Lb_nbrWords->setText(QString::number(wordImageNumber)+" /"+QString::number(CharactersSet.length()));
        display(imgInProcessus,"Word number :"+QString::number(wordImageNumber));
    }else { wordImageNumber = 0; }
}

void MainWindow::startCamera()
{
    ui->actionCapture_photo->setDisabled(true);
    this->exitCam = true;
    VideoCapture _WebCam(0);                     // Open The Default Camera.
        if(!_WebCam.isOpened())                  // Check If We Succeeded.
            ui->statusBar->showMessage("Failed");
        do
        {
            Mat frame;
            Mat _tmp;
            _WebCam >> frame;
            imshow("Origine : ",frame); // get a new frame from camera
            _imgRGB = frame;
            GaussianBlur(frame,_imgInProcessus,Size(1,3),0,0);
            cvtColor(_imgInProcessus, _imgInProcessus, CV_BGR2RGB);
            cvtColor(_imgInProcessus, _imgGray, CV_RGB2GRAY);
            //! [ MAKE Choice between (Otsu<Globale> || Gaussian< Adaptative> ]  04/03/2015  19:36.
            threshold(_imgGray,_tmp,0,255,CV_THRESH_BINARY+CV_THRESH_OTSU);  // Gloable Otsu.
            adaptiveThreshold(_imgGray,_imgBW,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,11,2);  //Adaptative Gausian.

            imshow("Otsu Binarization",preprocessing->copyRect(_tmp,25,25,480,480));
            //imshow("Gaussian+ Binarization", _imgBW);

            if(waitKey(30) >= 0) break;
        }while(this->exitCam);
        // the camera will be deinitialized automatically in VideoCapture destructor
}

void MainWindow::stopCapture()
{
    this->exitCam = false;
    ui->widget->setEnabled(true);
    ui->actionCapture_photo->setDisabled(false);
}

//! >>>>>>>>>>>>>>>>>>>>>>>>>> EXTRA METHODES TO HELP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
QImage MainWindow::Mat2QImage(Mat const& src)
{
     Mat temp; // make the same Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}

Mat MainWindow::QImage2Mat(QImage const& src)
{
     Mat tmp(src.height(),src.width(),CV_8UC4,(uchar*)src.bits(),src.bytesPerLine());
     cvtColor(tmp, tmp,CV_RGBA2RGB);
     return tmp;
}

void MainWindow::saveImg()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"));
    if(!fileName.isEmpty() && !imgInProcessus.isNull())
    {
        QDateTime dateTime = QDateTime::currentDateTime();
        QString dateTimeString = dateTime.toString("ss mm HH ddd MMMM d yy");
        imgInProcessus.save(fileName+" "+dateTimeString+".png");
        ui->statusBar->showMessage("Picture Saved.",10000);
    }
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About AOCR"),
            tr("Developed by <b>A</b>chraf <br> K."
               "Version 1.0.0 <br>"
               "Copyright 2014-2015."));
}

void MainWindow::display(const QImage& image,const QString message)
{
    item_pixmapIMG = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene->clear();
    scene->addItem(item_pixmapIMG);
    ui->graphicsView->update();
    QString msg = message+" :"+QString::number(image.width())+"X"+QString::number(image.height());
    ui->statusBar->showMessage(msg, 10000);
}

void MainWindow::displayHistogramme(const QImage& image)
{
    item_pixmapIMG_VHist = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    sceneVHist->clear();
    sceneVHist->addItem(item_pixmapIMG_VHist);
    ui->graphicsView_VHist->setScene(sceneVHist);
    ui->graphicsView_VHist->setMinimumWidth(10);
    ui->graphicsView_VHist->update();
}

void MainWindow::browse()
{
    fileName = QFileDialog::getOpenFileName(this,"Choose your image",QDir::homePath(),"Image files (*.png *.xpm *.jpg *.bmp)");
    if(!fileName.isEmpty())
    {
        ui->widget->setVisible(true);
        ui->widget->setEnabled(true);

        _imgRGB   = imread(fileName.toStdString());
        _imgGray  = imread(fileName.toStdString(),CV_LOAD_IMAGE_GRAYSCALE);

        imgOrigin = Mat2QImage(_imgRGB);
        imgInProcessus = imgOrigin;

        //! Display Image.
        display(imgInProcessus,"Image loaded.");
        buttonsChangeColors();

    }
}

void MainWindow::reset()
{
   ui->widget->setVisible(false);
   ui->graphicsView_VHist->setMaximumWidth(0);
   ui->TE_FinalText->setVisible(false);
   scene->clear();

   _imgOrigin.release();
   _imgInProcessus.release();
   _imgGray.release();
   _imgRGB.release();
   _imgBW.release();
   _imgBX_INV.release();
   //Visualization_V.release();
   //Visualization_H.release();

   //geoLines.clear();
   //geoWords.clear();
   allWordsImages.clear();
   histData.clear();
   fileName.clear();

   document = NULL;

   binVal=0;
   ui->widget->setEnabled(false);
   ui->labelStatus->clear();
}

void MainWindow::changeVisibility()
{
    ui->W_trainingSVM->setVisible(!ui->W_trainingSVM->isVisible());
    // DESIGNER >> 2015
    if(!ui->W_trainingSVM->isVisible()) deactivateButton(ui->Bt_trainingPhase);
    else activateButton(ui->Bt_trainingPhase);
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

    ui->Bt_2binarization->setStyleSheet(styleScriptCSS);
    ui->Bt_ContourDetection->setStyleSheet(styleScriptCSS);
    ui->Bt_CorrectSkew->setStyleSheet(styleScriptCSS);
    ui->Bt_cut_word->setStyleSheet(styleScriptCSS);
    ui->Bt_CXX->setStyleSheet(styleScriptCSS);
    ui->Bt_getTestingSet->setStyleSheet(styleScriptCSS);
    ui->Bt_LoadLabels->setStyleSheet(styleScriptCSS);
    ui->Bt_next_word->setStyleSheet(styleScriptCSS);
    ui->Bt_PDF_Back->setStyleSheet(styleScriptCSS);
    ui->Bt_PDF_Forword->setStyleSheet(styleScriptCSS);
    ui->Bt_previous_word->setStyleSheet(styleScriptCSS);
    ui->Bt_recognize_characters->setStyleSheet(styleScriptCSS);
    ui->Bt_segmente_to_words->setStyleSheet(styleScriptCSS_EXP);
    ui->Bt_thining->setStyleSheet(styleScriptCSS);
    ui->Bt_trainingPhase->setStyleSheet(styleScriptCSS);
    ui->BT_check->setStyleSheet(styleScriptCSS);
    ui->BT_getFeatureSetTraining->setStyleSheet(styleScriptCSS_EXP);
    ui->BT_loadSVM->setStyleSheet(styleScriptCSS);
    ui->BT_saveSVM->setStyleSheet(styleScriptCSS);
    ui->BT_trainTheMachine->setStyleSheet(styleScriptCSS);

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

    if(BT == ui->Bt_segmente_to_words || BT == ui->BT_getFeatureSetTraining) BT->setStyleSheet(styleScriptCSS_EXE);
    else BT->setStyleSheet(styleScriptCSS);
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
