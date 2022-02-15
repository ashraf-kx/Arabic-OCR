#include "segmentation.h"

Segmentation::Segmentation(QObject *parent) : QObject(parent)
{

}

QList<int> Segmentation::calculateBackProjection(Mat imageIn, HistogramType type)
{
    int x = 0;
    QList<int> list;
    list.clear();

    if (type == HORIZONTAL)
    {
        for (int i = 0; i < imageIn.rows; ++i)
        {
            x = 0;
            for (int j = 0; j < imageIn.cols; ++j)
            {
                if (imageIn.at<uchar>(i, j) == 255)
                    x++;
            }
            list << x;
        }
    }
    else
    {
        for (int i = 0; i < imageIn.cols; ++i)
        {
            x = 0;
            for (int j = 0; j < imageIn.rows; ++j)
            {
                if (imageIn.at<uchar>(j, i) == 255)
                    x++;
            }
            list << x;
        }
    }
    return list;
}

void Segmentation::getPositionCutWord(Mat imageLine, int lineIndex)
{
    // NOTE: Possible improvements suggest applying histogram smoothing.
    QList<int> VerticalHistogram = calculateBackProjection(imageLine, VERTICAL);

    // INFO: trying to find the right cut coordinates for the words in the current document line.
    bool lock = false;
    int i = 0;
    int tStartPosition = geoWords.length();
    while (i < VerticalHistogram.length())
    {
        if (VerticalHistogram.at(i) != 0)
        {
            word.y_start = i - 1;
            word.x_start = geoLines.at(lineIndex).start;
            lock = true;
            i++;

            while (lock)
            {
                if (VerticalHistogram.at(i) == 0)
                {
                    word.y_end = i + 1;
                    word.x_end = geoLines.at(lineIndex).end;
                    geoWords << word;
                    lock = false;
                }
                i++;
            }
        }
        else
            i++;
    }
    // For ARABIC SCRIPT [07_04_2015 20:00] (SAVED).
    int tEndPosition = geoWords.length();
    int Z = (tEndPosition - tStartPosition) / 2;

    for (int i = tStartPosition; i < tStartPosition + Z; i++)
    {
        tEndPosition--;
        geoWords.swapItemsAt(i, tEndPosition);
    }
}

void Segmentation::lineDetection(QList<int> histogramData)
{
    // No need for images here, only Histogram data
    int average = averageLineHeight(histogramData);

    int threshold = 0;
    bool lock = false;
    if (!histogramData.isEmpty())
    {
        int i = 0;
        while (i < histogramData.length())
        {
            int j = 0;
            if (histogramData.at(i) > threshold)
            {
                line.start = i - 1;
                lock = true;
                i++;
                j++;

                while (lock)
                {
                    if (histogramData.at(i) <= threshold)
                        if (j >= average)
                        {
                            line.end = i + 1;
                            geoLines << line; // Cricial: this is important.
                            lock = false;
                            j = -1;
                        }                    
                    i++; j++;
                }
            }
            else
                i++;
        }
    }
}

void Segmentation::wordsDetection()
{
    for (int i = 0; i < geoLines.length(); i++)
        this->getPositionCutWord(copyRect(binarizedMat, geoLines.at(i).start, 0, geoLines.at(i).end, binarizedMat.cols), i);
}

QList<Mat> Segmentation::segmentEntireDocument(Mat image)
{
    image.copyTo(binarizedMat);
    QList<Mat> wordsCollectionDetected;
    wordsCollectionDetected.clear();

    QList<int> horizontalHistogram = calculateBackProjection(binarizedMat, HORIZONTAL);

    int max = 0;
    for (int i = 0; i < horizontalHistogram.length(); i++)
        if (max < horizontalHistogram.at(i))
            max = horizontalHistogram.at(i);

    Mat paintedHistogram;
    paintedHistogram.create(binarizedMat.rows, max, CV_8UC1);

    for (int i = 0; i < paintedHistogram.rows; ++i)
        for (int j = 0; j < paintedHistogram.cols; ++j)
            paintedHistogram.at<uchar>(i, j) = 0;

    for (int i = 0; i < binarizedMat.rows; ++i)
        for (int j = 0; j < horizontalHistogram.at(i); ++j)
            paintedHistogram.at<uchar>(i, j) = 240;

    // imshow("H Histogram ", histImage);
    paintedHistogram = smoothingHistogram(paintedHistogram);
    imshow("Smoothed horizontal histogram", paintedHistogram);
    // END DRAWING.

    this->lineDetection(horizontalHistogram);
    this->wordsDetection();

    // Cut the Entier Image Into Small Pieces We call Them Words.
    for (int i = 0; i < geoWords.length(); ++i)
        wordsCollectionDetected << copyRect(binarizedMat, geoWords.at(i).x_start, geoWords.at(i).y_start, geoWords.at(i).x_end, geoWords.at(i).y_end);

    // Painting lines that seperate each text-line in the document.
    for (int i = 0; i < geoLines.length(); i++)
        for (int j = 0; j < binarizedMat.cols; ++j)
        {
            binarizedMat.at<uchar>((geoLines.at(i)).start, j) = 150;
            binarizedMat.at<uchar>((geoLines.at(i)).end, j) = 127;
        }

    // Painting the lines that seperate each word/alphabet for each text-line in the document.
    for (int i = 0; i < geoWords.length(); ++i)
        for (int j = geoWords.at(i).x_start; j < geoWords.at(i).x_end; ++j)
        {
            binarizedMat.at<uchar>(j, geoWords.at(i).y_start) = 200;
            binarizedMat.at<uchar>(j, (geoWords.at(i)).y_end) = 130;
        }

    geoLines.clear();
    geoWords.clear();

    binarizedMat.copyTo(image);
    return wordsCollectionDetected;
}

QList<Mat> Segmentation::getCollectionOfLinesDetected(Mat image)
{
    QList<Mat> collectionOfLinesDetected;
    collectionOfLinesDetected.clear();

    image.copyTo(binarizedMat);
    QList<int> horizontalHistogram = calculateBackProjection(binarizedMat, HORIZONTAL);

    this->lineDetection(horizontalHistogram);

    // Try detect and cut all text-lines within the current Document.
    for (int i = 0; i < geoLines.length(); ++i)    
        collectionOfLinesDetected << copyRect(binarizedMat, geoLines.at(i).start, 0, geoLines.at(i).end, binarizedMat.cols);

    // Visualise the tracing line that seperate documents text-lines.
    for (int i = 0; i < geoLines.length(); i++)    
        for (int j = 0; j < binarizedMat.cols; ++j)
        {
            binarizedMat.at<uchar>((geoLines.at(i)).start, j) = 150;
            binarizedMat.at<uchar>((geoLines.at(i)).end, j) = 127;
        }    
    binarizedMat.copyTo(image);
    return collectionOfLinesDetected;
}

int Segmentation::mostRedundantValue(QList<int> list)
{
    int value = 0;
    QList<Candidate> Condidate;
    Condidate.clear();
    while (list.length() != 0)
    {
        int x = list.at(0);
        int repeated = 0;

        for (int i = 0; i < list.length(); i++)
        {
            if (list.at(i) == x)
            {
                list.removeAt(i);
                i--;
                repeated++;
            }
        }
        C.timeRepeated = repeated;
        C.value = x;
        Condidate << C;
    }

    for (int i = 0; i < Condidate.length(); ++i)
    {
        if ((Condidate.at(i)).value == 0)
            Condidate.removeAt(i);
    }

    int max = 0;
    for (int i = 0; i < Condidate.length(); ++i)
    {
        if (max < (Condidate.at(i)).timeRepeated)
        {
            max = (Condidate.at(i)).timeRepeated;
            value = (Condidate.at(i)).value;
        }
    }

    return value;
}

QList<Mat> Segmentation::cutLine2Characters(QList<Mat> listImagesLines, int i)
{
    QList<Mat> listImagesCharacters;
    listImagesCharacters.clear();

    if (!listImagesLines.empty())
        listImagesCharacters = tryCutWord(listImagesLines.at(i));

    return listImagesCharacters;
}

QList<Mat> Segmentation::getCharactersInWord(Mat image, QList<int> posCuts)
{
    QList<Mat> listCharacters;
    int start = 0;
    int end = 0;
    listCharacters.clear();
    for (int i = 0; i < posCuts.length(); i++)
    {
        end = posCuts.at(i);
        listCharacters << copyRect(image, 0, start, image.rows, end);
        start = end;
    }
    listCharacters << copyRect(image, 0, start, image.rows, image.cols);
    return listCharacters;
}

QList<int> Segmentation::cutWhereWhite(QList<int> list)
{
    QList<int> listPosCuts;
    listPosCuts.clear();
    for (int i = 1; i < list.length(); i++)
    {
        if ((list.at(i) == 0) && (list.at(i - 1) > list.at(i)))
        {
            listPosCuts << i;
        }
    }
    return listPosCuts;
}

QList<Mat> Segmentation::tryCutWord(Mat imageIn)
{
    QList<Mat> listCharacters;
    listCharacters.clear();
    if (imageIn.rows > 5 && imageIn.cols > 5)
    {
        QList<int> HDataHist;
        QList<int> VDataHist;
        QList<int> Hist_3;
        QList<int> posCuts;
        HDataHist.clear();
        VDataHist.clear();
        Hist_3.clear();
        posCuts.clear();

        int x_Start = 0;
        int x_End = 0;
        int y_Start = 0;
        int y_End = 0;
        int PosBaseLine = 0;
        int max = 0;

        HDataHist = calculateBackProjection(imageIn, HORIZONTAL);
        // GET [x_Start-x_End] calculate THICKNESS [ 44% / 22%] >> [50% / 33%] >> Solution Inchallah [85% / 55%] >> Critic [20% / 20%]X
        // >> Emmm [85% / 70%]  >>
        int paramTop = 85;
        int paramButtom = 80;
        for (int i = 0; i < HDataHist.length(); i++)
        {
            if (HDataHist.at(i) != 0)
            {
                x_Start = i;
                break;
            }
        }
        for (int i = HDataHist.length() - 2; i >= 0; i--)
        {
            if (HDataHist.at(i) != 0)
            {
                x_End = i;
                break;
            }
        }

        // imageIn = contour(imageIn);
        VDataHist = calculateBackProjection(imageIn, VERTICAL);
        max = 0;
        for (int i = 0; i < VDataHist.length(); i++)
        {
            if (max < VDataHist.at(i))
            {
                max = VDataHist.at(i);
            }
        }

        // GET [y_Start-y_End]
        for (int i = 0; i < VDataHist.length(); i++)
        {
            if (VDataHist.at(i) != 0)
            {
                y_Start = i;
                i = VDataHist.length();
            }
        }
        for (int i = VDataHist.length() - 2; i >= 0; i--)
        {
            if (VDataHist.at(i) != 0)
            {
                y_End = i;
                i = 0;
            }
        }

        //  Entier New Pure Image.
        Mat part = copyRect(imageIn, x_Start, y_Start, x_End, y_End);
        // Getting the Right width of the zone BaseLine [ PROPOSED ].
        HDataHist = calculateBackProjection(part, HORIZONTAL);
        for (int i = 0; i < HDataHist.length(); i++)
        {
            if (max < HDataHist.at(i))
            {
                max = HDataHist.at(i);
                PosBaseLine = i;
            }
        }
        int Start = PosBaseLine - (((PosBaseLine - x_Start) * paramTop) / 100);
        int End = (((x_End - PosBaseLine) * paramButtom) / 100) + PosBaseLine;
        Mat clonedImageIn(part);
        Mat showImage(part);

        part = copyRect(imageIn, Start, y_Start, End, y_End);
        // imshow("Part Base Line",part);

        VDataHist = calculateBackProjection(part, VERTICAL);

        //!**********************************************************************************
        Visualization_V.create(max, part.cols, CV_8UC1);
        // convert to white
        for (int i = 0; i < Visualization_V.rows; ++i)
        {
            for (int j = 0; j < Visualization_V.cols; ++j)
            {
                Visualization_V.at<uchar>(i, j) = 255;
            }
        }
        // Draw Vertical hist From
        for (int i = 0; i < VDataHist.length(); ++i)
        {
            for (int j = 0; j < VDataHist.at(i); ++j)
            {
                Visualization_V.at<uchar>(j, i) = 0;
            }
        }

        // imshow("Before Vertical Projection",Visualization_V);
        GaussianBlur(Visualization_V, Visualization_V, Size(5, 5), 0, 0);
        cv::threshold(Visualization_V, Visualization_V, 127, 255, CV_THRESH_BINARY);
        Visualization_V = smoothingHistogram(Visualization_V);
        // imshow("After Vertical Projection",Visualization_V);

        //!>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><
        Hist_3 = calculateBackProjection(Visualization_V, VERTICAL);
        int threshold = mostRedundantValue(Hist_3);
        posCuts = cutWhereWhite(Hist_3);
        // Qdebug()<<"threshold of cutting based_On_Contour : "<<QString::number(threshold);
        int tracker = 0;
        // From Left To Right
        for (int i = 1; i < Hist_3.length() - 2; i++)
        {
            if ((Hist_3.at(i - 1) == Hist_3.at(i)) && (Hist_3.at(i) == Hist_3.at(i + 1)) && (Hist_3.at(i) == threshold))
            {
                tracker++;
                if (tracker > 2 && (Hist_3.at(i + 1) != Hist_3.at(i + 2)))
                {
                    posCuts << i - (tracker); // minus :  (tracker/2) OR (tracker/3) OR tracker
                    tracker = 0;
                }
            }
        }
        std::sort(posCuts.begin(), posCuts.end()); // sorted with the default operator< [Solved]
        for (int j = 0; j < posCuts.length(); ++j) // temporary Visualization [ SHOW RESULT ].
        {
            // Qdebug()<<"Cut >_"<<QString::number(posCuts.at(j));
            for (int i = 0; i < showImage.rows; i++)
            {
                showImage.at<uchar>(i, posCuts.at(j)) = 255;
            }
            for (int i = 0; i < Visualization_V.rows; i++)
            {
                Visualization_V.at<uchar>(i, posCuts.at(j)) = 255;
            }
        }
        for (int i = 0; i < showImage.cols; i++)
        {
            showImage.at<uchar>(PosBaseLine, i) = 127;
        }
        // imshow("Back Projection cuts",Visualization_V);
        // imshow("Base Line",showImage);

        //! Display the Image Resulted.
        imageUnderProcess = QImage((uchar *)showImage.data, showImage.cols, showImage.rows, showImage.step, QImage::Format_Indexed8);
        imageUnderProcess.bits();
        // display(imgInProcessus,"Cut Words");
        showImage.copyTo(imageIn);
        listCharacters = getCharactersInWord(clonedImageIn, posCuts);
        // POST PROCESSING NEEDED ONLY [ later inchallah].
        return listCharacters;
    }
    else
        return listCharacters;
}

Mat Segmentation::smoothingHistogram(Mat image)
{
    int x;
    int black = 0; // select BLACK color.
    int white = 255;

    for (int i = 1; i < image.rows - 2; ++i)
    {
        for (int j = 1; j < image.cols - 2; ++j)
        {
            x = 0;

            if (image.at<uchar>(i, j) == white)
            {
                if (image.at<uchar>(i + 1, j + 1) == black)
                    x++;
                if (image.at<uchar>(i - 1, j - 1) == black)
                    x++;
                if (image.at<uchar>(i, j + 1) == black)
                    x++;
                if (image.at<uchar>(i, j - 1) == black)
                    x++;
                if (image.at<uchar>(i + 1, j) == black)
                    x++;
                if (image.at<uchar>(i - 1, j) == black)
                    x++;
                if (image.at<uchar>(i - 1, j + 1) == black)
                    x++;
                if (image.at<uchar>(i + 1, j - 1) == black)
                    x++;
            }
            if (x >= 6)
                image.at<uchar>(i, j) = black;
        }
    }

    for (int i = 1; i < image.rows - 2; ++i)
    {
        for (int j = 1; j < image.cols - 2; ++j)
        {
            x = 0;
            if (image.at<uchar>(i, j) == black)
            {
                if (image.at<uchar>(i + 1, j + 1) == white)
                    x++;
                if (image.at<uchar>(i - 1, j - 1) == white)
                    x++;
                if (image.at<uchar>(i, j + 1) == white)
                    x++;
                if (image.at<uchar>(i, j - 1) == white)
                    x++;
                if (image.at<uchar>(i + 1, j) == white)
                    x++;
                if (image.at<uchar>(i - 1, j) == white)
                    x++;
                if (image.at<uchar>(i - 1, j + 1) == white)
                    x++;
                if (image.at<uchar>(i + 1, j - 1) == white)
                    x++;
            }
            if (x >= 4)
                image.at<uchar>(i, j) = white;
        }
    }
    return image;
}

Mat Segmentation::copyRect(Mat image, int x1, int y1, int x2, int y2)
{

    int ii = 0;
    int jj = 0;
    int rows = x2 - x1;
    int cols = y2 - y1;
    // Qdebug()<<QString::number(rows)+"X"+QString::number(cols);
    Mat partImage(rows, cols, CV_8UC1, Scalar(0));
    for (int i = x1; i < x2; i++)
    {
        jj = 0;
        for (int j = y1; j < y2; j++)
        {
            partImage.at<uchar>(ii, jj) = image.at<uchar>(i, j);
            jj++;
        }
        ii++;
    }
    return partImage;
}

Mat Segmentation::characterNormalization(Mat image)
{
    if (!image.empty() && image.cols > 0 && image.rows > 0)
    {
        int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
        int x = 0;
        QList<int> VHist; // = calculateBackProjection(image,V_Hist);
        VHist.clear();
        for (int i = 0; i < image.cols; ++i)
        {
            x = 0;
            for (int j = 0; j < image.rows; ++j)
            {
                if (image.at<uchar>(j, i) == 0)
                    x++;
            }
            VHist << x;
        }

        for (int i = 0; i < VHist.length(); i++)
        {
            if (VHist.at(i) != 0)
            {
                y1 = i;
                break;
            }
        }
        for (int i = VHist.length() - 2; i >= 0; i--)
        {
            if (VHist.at(i) != 0)
            {
                y2 = i;
                break;
            }
        }

        QList<int> HHist;
        HHist.clear(); // calculateBackProjection(image,H_Hist);

        for (int i = 0; i < image.rows; ++i)
        {
            x = 0;
            for (int j = 0; j < image.cols; ++j)
            {
                if (image.at<uchar>(i, j) == 0)
                    x++;
            }
            HHist << x;
        }

        for (int i = 0; i < HHist.length(); i++)
        {
            if (HHist.at(i) != 0)
            {
                x1 = i;
                break;
            }
        }
        for (int i = HHist.length() - 2; i >= 0; i--)
        {
            if (HHist.at(i) != 0)
            {
                x2 = i;
                break;
            }
        }
        VHist.clear();
        HHist.clear();

        image = copyRect(image, x1, y1, x2, y2);
        cv::resize(image, image, Size(100, 100), 0, 0, INTER_CUBIC);
        threshold(image, image, 127, 255, CV_THRESH_BINARY);
        // imwrite(QString::number(w).toStdString()+".png",image);   w++;   // For Demonstration Only.
    }
    return image;
}

int Segmentation::averageLineHeight(QList<int> histogramData)
{
    QList<int> linesHeight;
    linesHeight.clear();

    bool lock = false;
    if (!histogramData.isEmpty())
    {
        int i = 0;
        while (i < histogramData.length())
        {
            int j = 0;
            if (histogramData.at(i) > 0)
            {
                lock = true;
                j++; i++;

                while (lock)
                {
                    if (histogramData.at(i) <= 0)
                    {
                        linesHeight << j;
                        lock = false;
                        j = -1;
                    }

                    j++; i++;
                }
            }
            else
                i++;
        }
    }

    int average = 0;
    for (int i = 0; i < linesHeight.length(); i++) average += linesHeight.at(i);
    average = (int)((average / linesHeight.length()) / 1.5);

    return average;
}
