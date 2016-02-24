#include "recognition.h"

Recognition::Recognition(QObject *parent) :
    QObject(parent)
{
    // Default to avoid errors
     AllLettres ="ﻷ/ﻸ/ﻻ/ﻼ/­ﺂ/£/¤/ﺄ/ﺎ/ب/ت/ث/،/ج/ح/خ/٠/١/٢/٣/٤/٥/٦/٧/٨/٩/ف/؛/س/ش/ص/؟/¢/ء/آ/أ/ؤ/ﻊ/ﺋ/ا/ﺑ/ة/ﺗ/ﺛ/ﺟ/ﺣ/ﺧ/د/ذ/ر/ز/ﺳ/ﺷ/ﺻ/ﺿ/ط/ظ/ﻋ/ﻏ/¦/¬/÷/×/ع/ـ/ﻓ/ﻗ/ﻛ/ﻟ/ﻣ/ﻧ/ﻫ/و/ى/ﻳ/ض/ﻌ/ﻎ/غ/م/ن/ه/ﻬ/ﻰ/ﻲ/ﻐ/ق/ﻵ/ﻶ/ل/ك/ي";
}


void Recognition::loadLabels(QString labels)
{
    AllLettres = labels;
}

Mat  Recognition::getTestingSet(QList<Mat> CharactersSet)
{
    Mat TestingSet;
    QList<QList<float> > sequanceVectorsSet;
    for (int i = 0; i < CharactersSet.length(); ++i) {
       //X imwrite("/home/ash/Desktop/No_Normalizer/"+QString::number(i).toStdString()+".png",CharactersSet.at(i));
        sequanceVectorsSet<<extractFeaturesVector(CharactersSet.at(i));
       //X  imwrite("/home/ash/Desktop/Normalizer/"+QString::number(i).toStdString()+".png",Segmentation::characterNormalization(CharactersSet.at(i)));
    }

    TestingSet.create(sequanceVectorsSet.length(),sequanceVectorsSet.at(0).length(),CV_32FC1);
    for (int i = 0; i < sequanceVectorsSet.length(); ++i) {
        QList<float> list = sequanceVectorsSet.at(i);
        for (int j = 0; j < list.length(); ++j) {
            TestingSet.at<float>(i,j) = list.at(j);
        }
    }

    sequanceVectorsSet.clear();
    return TestingSet;
}

Mat  Recognition::getTrainingSet(QList<Mat> CharactersSet)
{
    Mat trainingSet;
    QList<QList<float> > sequanceVectorsSet;

    for (int i = 0; i < CharactersSet.length(); ++i) {
        sequanceVectorsSet<<extractFeaturesVector(CharactersSet.at(i));
    }
    // SWITCH from QT-LIST >> OPENCV Mat.
    trainingSet.create(sequanceVectorsSet.length(),sequanceVectorsSet.at(0).length(),CV_32FC1);
    for (int i = 0; i < sequanceVectorsSet.length(); ++i) {
        QList<float> list = sequanceVectorsSet.at(i);
        for (int j = 0; j < list.length(); ++j) {
            trainingSet.at<float>(i,j) = list.at(j);
        }
    }

    sequanceVectorsSet.clear();
    return trainingSet;
}

void Recognition::setSVMParameters(int kernalType,double C,double degree,double gamma)
{
    params.svm_type    = SVM::C_SVC;
    params.kernel_type = kernalType;
    params.C           = C;
    params.degree      = degree;
    params.gamma       = gamma;
    params.term_crit   = TermCriteria(CV_TERMCRIT_ITER, (int)1e7, 1e-6);
}

void Recognition::trainTheMachine(Mat trainingSet)
{
    // ------------------- 1. Preparing Data For Supervised Learning -----------------------------
    Mat labels(trainingSet.rows,1,CV_32FC1);
    for(int i=0; i< labels.rows;)
    {
        for (int j = 0; j <= trainingSet.rows; j++)
        {
            labels.at<float>(i,0) = j;
            i++;
            if(i >= labels.rows ) break;
        }
    }
    // ---------------------   2. Set Up Parametres SVM ------------------------------------------


    //------------------------ 3. Train The SVM --------------------------------------------------

    //qint64 start = QDateTime::currentMSecsSinceEpoch();
    svm.train(trainingSet, labels, Mat(), Mat(), params);
    // qint64 end = QDateTime::currentMSecsSinceEpoch();
    //int x = end-start;

}

void Recognition::loadTrainingFile(QString fileName)
{

    if(!fileName.isEmpty()) svm.load(fileName.toStdString().c_str());
}

void Recognition::saveTraining(QString fileName)
{
    if(!fileName.isEmpty()) svm.save(fileName.toStdString().c_str());
}

QString Recognition::recognize(Mat TestingSet)
{
    QStringList list = AllLettres.split(QRegExp("/"));               // Temperary solution [ for model Characters ].
    QString FinalResultToShow= "";
    charactersRecognized.create(TestingSet.rows,1,CV_32FC1);
    svm.predict(TestingSet,charactersRecognized);

    for (int i = -1; i < charactersRecognized.rows-1; i++) {
        int x = charactersRecognized.at<float>(i,1);
        FinalResultToShow=FinalResultToShow+list.at(x);
    }

    qDebug()<<FinalResultToShow;
    return FinalResultToShow;
}

QString Recognition::recognizeTest(Mat TestingSet)
{
    QStringList list = AllLettres.split(QRegExp("/"));         // Temperary solution [ for model Characters ].
    //for(int i=0; i< list.length();i++) qDebug()<<list.at(i)<<endl;
    QString FinalResultToShow= "";
    charactersRecognized.create(TestingSet.rows,1,CV_32FC1);
    svm.predict(TestingSet,charactersRecognized);

    //qDebug()<<"char rec : "<<charactersRecognized.rows<<"testing set : "<<TestingSet.rows<<endl;
    for (int i = -1; i < charactersRecognized.rows-1; i++) {
        int x = charactersRecognized.at<float>(i,1);

        FinalResultToShow=FinalResultToShow+" "+list.at(x);
    }
    qDebug()<<FinalResultToShow;
    return FinalResultToShow;

}

QList<float> Recognition::extractFeaturesVector(Mat image)
{
    QList<float> vector;
    vector.clear();

    // Ration width/height & Height/width.
    vector<<calculateWHRation(image);
    vector<<calculateHWRation(image);

    image = Segmentation::characterNormalization(image);

    float number=0;
    for (int i = 0; i < image.rows;) {
        int ii=i;
        i+=10;
        for (int j = 0; j < image.cols;) {
            int jj=j;
            j+=10;
            number = diagonalAverage(Segmentation::copyRect(image,ii,jj,i,j));
            vector<<number;
        }
    }

    // number CXX Feature
    vector<<countCXX(image);

    return vector;
}

int   Recognition::countCXX(const Mat &image)
{
    // Fill the label_image with the blobs
    // 0  - background
    // 1  - unlabelled foreground
    // 2+ - labelled foreground
    std::vector < std::vector<Point2i > > blobs;
    blobs.clear();

    threshold(image, image, 0.0, 1.0, THRESH_BINARY_INV);
    Mat output = Mat::zeros(image.size(), CV_8UC3);
    Mat label_image;

    image.convertTo(label_image, CV_32SC1);

    int label_count = 2; // starts at 2 because 0,1 are used already

    for(int y=0; y < label_image.rows; y++) {
        int *row = (int*)label_image.ptr(y);
        for(int x=0; x < label_image.cols; x++) {
            if(row[x] != 1) {
                continue;
            }

            Rect rect;
            floodFill(label_image, Point(x,y), label_count, &rect, 0, 0, 4);

            std::vector <Point2i> blob;

            for(int i=rect.y; i < (rect.y+rect.height); i++) {
                int *row2 = (int*)label_image.ptr(i);
                for(int j=rect.x; j < (rect.x+rect.width); j++) {
                    if(row2[j] != label_count) {
                        continue;
                    }

                    blob.push_back(Point2i(j,i));
                }
            }

            blobs.push_back(blob);

            label_count++;
        }
    }
    return blobs.size();
}

float Recognition::diagonalAverage(Mat mask)
{
    QList<int> sum;
    sum.clear();
    int x;
        int i=0;
        for (int j = 0; j < mask.cols; ++j) {
           int ii=i;
           int jj=j;
           x=0;
           while(ii <= j || jj >= i)
           {
               if(mask.at<uchar>(ii,jj) == 0) x++;
               ii++;
               jj--;
           }
           sum<<x;
        }


    int j=mask.cols-1;
    for (int i = 1; i < mask.rows; ++i) {
       int ii=i;
       int jj=j;
       x=0;
       while(ii <= j || jj >= i)
       {
           if(mask.at<uchar>(ii,jj) == 0) x++;
           ii++;
           jj--;
       }
       sum<<x;
    }
    x = 0;
    i = 0;
    while(i<sum.length())
    {
        x+=sum.at(i);
        i++;
    }
    return x/sum.length();
}

float Recognition::calculateWHRation(const Mat &image)
{
    return (float)image.cols/(float)image.rows;;
}

float Recognition::calculateHWRation(const Mat &image)
{
    return (float)image.rows/(float)image.cols;;
}
