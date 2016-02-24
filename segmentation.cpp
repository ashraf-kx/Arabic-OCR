#include "segmentation.h"

Segmentation::Segmentation(QObject *parent) :
    QObject(parent)
{

}

QList<int> Segmentation::calculateBackProjection(Mat imageIn,HistType type)
{
    int x = 0;
    QList<int> list;
    list.clear();

    if(type == H_Hist){
        for (int i = 0; i < imageIn.rows; ++i){
            x=0;
            for (int j = 0; j < imageIn.cols; ++j){
                if(imageIn.at<uchar>(i, j) == 0) x++;
            }
            list<<x;
        }
    }else{
        for (int i = 0; i < imageIn.cols; ++i){
            x=0;
            for (int j = 0; j < imageIn.rows; ++j)
            {
                if(imageIn.at<uchar>(j, i) == 0) x++;
            }
            list<<x;
        }
    }
    return list;
}

void Segmentation::getPosCutWord(Mat imageLine, int lineNbr)
{
    // Vertical Histogram.
    QList<int> HistLineData = calculateBackProjection(imageLine,V_Hist);

    // word Calculate position of cuts.
    bool lock = false;
    int i=0;
    int t_S_Pos = geoWords.length();
    while(i<HistLineData.length())
    {
        if(HistLineData.at(i) != 0)
        {
            W.y_start = i-1;
            W.x_start = geoLines.at(lineNbr).start;
            lock = true;
            i++;

            while (lock)
            {
                if(HistLineData.at(i) == 0)
                {
                    W.y_end = i+1;
                    W.x_end = geoLines.at(lineNbr).end;
                    geoWords<<W;
                    lock=false;
                }
                i++;
            }
        }
        else i++;
    }
    // For ARABIC SCRIPT [07_04_2015 20:00] (SAVED).
    int t_E_Pos = geoWords.length();
    int Z       = (t_E_Pos-t_S_Pos)/2;
    for(int i=t_S_Pos; i< t_S_Pos+Z; i++)
    {
        t_E_Pos--;
        geoWords.swap(i,t_E_Pos);

    }
}

void Segmentation::lineDetection(QList<int> HistDATA)
{
    // No need for images hear, only Histograms informations of them used !!
    int average = averageLineHeight(HistDATA);

    int threshold = 0;
    bool lock = false;
    if(!HistDATA.isEmpty())
    {
        int i=0;
        while(i<HistDATA.length())
        {
            int j=0;
            if(HistDATA.at(i) > threshold)
            {
                L.start = i-1;
                lock = true;
                i++;
                j++;

                while (lock)
                {
                    if(HistDATA.at(i) <= threshold)
                    {
                        if(j >= average ){
                            L.end = i+1;
                            geoLines<<L; // this is it <<<<----- [ Cricial ].
                            lock=false;
                            j=-1;
                        }
                    }
                    i++;
                    j++;
                }
            }
            else i++;
        }
    }
}

void Segmentation::wordsDetection()
{
    for(int i=0; i< geoLines.length();i++)
    {
        // complicated, but under Control [ Histogram Smoothing needed MAYBE ].
        this->getPosCutWord(copyRect(_imgBW,(geoLines.at(i)).start,0,(geoLines.at(i)).end,_imgBW.cols),i);
    }
}

QList<Mat> Segmentation::segmenteEntierDocument(Mat image)
{
        image.copyTo(_imgBW);
        QList<Mat> allWordsImages;
        allWordsImages.clear();

        // Vertical Histogram.
        QList<int> H_HistDATA = calculateBackProjection(_imgBW,H_Hist);
        int max = 0;
        for(int i=0; i< H_HistDATA.length();i++)
        {
            if(max < H_HistDATA.at(i) ){ max = H_HistDATA.at(i); }
        }

        // Drawing the Histogram.
        Mat histImage;
        histImage.create(_imgBW.rows,max,CV_8UC1);

        for (int i = 0; i < histImage.rows; ++i) {
            for (int j = 0; j < histImage.cols; ++j)
            {
                histImage.at<uchar>(i, j) = 255;
            }
        }

        for (int i = 0; i < _imgBW.rows; ++i) {
            for (int j = 0; j < H_HistDATA.at(i); ++j) {
                histImage.at<uchar>(i, j) = 0;
            }
        }
        //imshow("H Histogramme ",histImage);
        histImage = smoothingHistogramme(histImage);
        //imshow("H Histogramme S",histImage);
        // END DRAWING.

        //! [ 03/03/2015  21:49] CALL FOR METHODES <? Wandring ?>
        this->lineDetection(H_HistDATA);
        this->wordsDetection();

        // Cut the Entier Image Into Small Pieces We call Them Words.
        for(int i = 0; i<geoWords.length(); ++i){  //JAVAFX
            allWordsImages<<copyRect(_imgBW,geoWords.at(i).x_start,geoWords.at(i).y_start,geoWords.at(i).x_end,geoWords.at(i).y_end);
        }

        // Visualisation the Detection lines.
        for(int i =0; i< geoLines.length(); i++)
        {
            for (int j = 0; j < _imgBW.cols; ++j)
            {
                _imgBW.at<uchar>((geoLines.at(i)).start,j) = 150;
                _imgBW.at<uchar>((geoLines.at(i)).end,j)   = 127;
            }
        }

        // Visualisation the Detection words.
        for(int i =0; i< geoWords.length(); ++i){

            for (int j = geoWords.at(i).x_start; j < geoWords.at(i).x_end; ++j)
            {
                _imgBW.at<uchar>(j,geoWords.at(i).y_start)  = 200;
                _imgBW.at<uchar>(j,(geoWords.at(i)).y_end)  = 130;
            }
        }

        geoLines.clear();
        geoWords.clear();

        _imgBW.copyTo(image);
        return allWordsImages;
}

QList<Mat> Segmentation::getAllImagesLines(Mat image)
{
    QList<Mat> allLinesImages;
    allLinesImages.clear();

    image.copyTo(_imgBW);
    // Vertical Histogram.
    QList<int> H_HistDATA = calculateBackProjection(_imgBW,H_Hist);

    //! [ 03/03/2015  21:49] CALL FOR METHODES <? Wandring ?>
    this->lineDetection(H_HistDATA);

    // Cut the Entier Image Into Small Pieces We call Them images  Lines.
    for(int i = 0; i<geoLines.length(); ++i){ //JAVAFX
        allLinesImages<<copyRect(_imgBW,geoLines.at(i).start,0,geoLines.at(i).end,_imgBW.cols);
    }

    // Visualisation the Detection lines.
    for(int i =0; i< geoLines.length(); i++)
    {
        for (int j = 0; j < _imgBW.cols; ++j)
        {
            _imgBW.at<uchar>((geoLines.at(i)).start,j) = 150;
            _imgBW.at<uchar>((geoLines.at(i)).end,j)   = 127;
        }
    }
    _imgBW.copyTo(image);
    return allLinesImages;
}

int Segmentation::mostRedundantValue(QList<int> list)
{
   int value=0;
   QList<candidate> Condidate;
   Condidate.clear();
   while(list.length() != 0){
           int x = list.at(0);
           int repeated=0;

           for(int i=0;i<list.length();i++){
               if(list.at(i) == x ){
                   list.removeAt(i);
                   i--;
                   repeated++;
               }
           }
           C.timeRepeated = repeated;
           C.value        = x;
           Condidate<<C;

   }

   for (int i = 0; i < Condidate.length(); ++i) {
       if((Condidate.at(i)).value == 0 )  Condidate.removeAt(i);
   }

   int max=0;
   for (int i = 0; i < Condidate.length(); ++i) {
       if(max < (Condidate.at(i)).timeRepeated ) {
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

    if(!listImagesLines.empty())
        listImagesCharacters = tryCutWord(listImagesLines.at(i));

    return listImagesCharacters;
}

QList<Mat> Segmentation::getCharactersInWord(Mat image,QList<int> posCuts)
{
    QList<Mat> listCharacters;
    int start = 0;
    int end=0;
    listCharacters.clear();
    for(int i=0; i< posCuts.length();i++)
    {
        end = posCuts.at(i);
        listCharacters<< copyRect(image,0,start,image.rows,end);
        start = end;
    }
    listCharacters<< copyRect(image,0,start,image.rows,image.cols);
    return listCharacters;
}

QList<int> Segmentation::cutWhereWhite(QList<int> list)
{
    QList<int> listPosCuts;
    listPosCuts.clear();
    for(int i=1;i<list.length();i++)
    {
        if((list.at(i) == 0) && (list.at(i-1) > list.at(i) ))
        {
            listPosCuts<< i;
        }
    }
    return listPosCuts;
}

QList<Mat> Segmentation::tryCutWord(Mat imageIn)
{
    QList<Mat> listCharacters;
    listCharacters.clear();
    if(imageIn.rows> 5 && imageIn.cols>5)
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
    int x_End   = 0;
    int y_Start = 0;
    int y_End   = 0;
    int PosBaseLine = 0;
    int max = 0;

    HDataHist = calculateBackProjection(imageIn,H_Hist);
    // GET [x_Start-x_End] calculate THICKNESS [ 44% / 22%] >> [50% / 33%] >> Solution Inchallah [85% / 55%] >> Critic [20% / 20%]X
    // >> Emmm [85% / 70%]  >>
    int paramTop = 85; int paramButtom = 80;
    for(int i=0; i<HDataHist.length();i++)
    {
        if(HDataHist.at(i) != 0) { x_Start = i; break; }
    }
    for(int i=HDataHist.length()-2; i >= 0; i--)
    {
        if(HDataHist.at(i) != 0) { x_End = i; break; }
    }

    //imageIn = contour(imageIn);
    VDataHist = calculateBackProjection(imageIn,V_Hist);
    max=0;
    for(int i=0; i<VDataHist.length();i++)
    {
        if(max < VDataHist.at(i) ){ max = VDataHist.at(i); }
    }

    // GET [y_Start-y_End]
    for(int i=0; i<VDataHist.length();i++)
    {
        if(VDataHist.at(i) != 0) { y_Start = i; i=VDataHist.length(); }
    }
    for(int i=VDataHist.length()-2; i >= 0; i--)
    {
        if(VDataHist.at(i) != 0) { y_End = i; i=0; }
    }

    //  Entier New Pure Image.
    Mat part = copyRect(imageIn,x_Start,y_Start,x_End,y_End);
    // Getting the Right width of the zone BaseLine [ PROPOSED ].
    HDataHist = calculateBackProjection(part,H_Hist);
    for(int i=0; i<HDataHist.length();i++)
    {
        if(max < HDataHist.at(i) ){ max = HDataHist.at(i); PosBaseLine = i; }
    }
    int Start = PosBaseLine-(((PosBaseLine-x_Start)*paramTop)/100);
    int End   = (((x_End-PosBaseLine)*paramButtom)/100)+PosBaseLine;
    Mat clonedImageIn(part);
    Mat showImage(part);

    part = copyRect(imageIn,Start,y_Start,End,y_End);
    //imshow("Part Base Line",part);

    VDataHist = calculateBackProjection(part,V_Hist);

    //!**********************************************************************************
        Visualization_V.create(max,part.cols,CV_8UC1);
        // convert to white
        for (int i = 0; i < Visualization_V.rows; ++i) {
            for (int j = 0; j < Visualization_V.cols; ++j)
            {
                Visualization_V.at<uchar>(i, j) = 255;
            }
        }
        // Draw Vertical hist From
        for (int i = 0; i < VDataHist.length(); ++i) {
            for (int j = 0; j < VDataHist.at(i); ++j)
            {
                Visualization_V.at<uchar>(j, i) = 0;
            }
        }

        //imshow("Before Vertical Projection",Visualization_V);
        GaussianBlur(Visualization_V,Visualization_V,Size(5,5),0,0);
        cv::threshold(Visualization_V,Visualization_V,127,255,CV_THRESH_BINARY);
        Visualization_V = smoothingHistogramme(Visualization_V);
        //imshow("After Vertical Projection",Visualization_V);

    //!>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><
    Hist_3 = calculateBackProjection(Visualization_V,V_Hist);
    int threshold = mostRedundantValue(Hist_3);
    posCuts = cutWhereWhite(Hist_3);
    //Qdebug()<<"threshold of cutting based_On_Contour : "<<QString::number(threshold);
    int tracker=0;
    // From Left To Right
    for(int i=1; i<Hist_3.length()-2; i++)
    {
        if((Hist_3.at(i-1) == Hist_3.at(i)) && (Hist_3.at(i) == Hist_3.at(i+1)) && (Hist_3.at(i) == threshold) )
        {
               tracker++;
               if(tracker > 2 && (Hist_3.at(i+1) != Hist_3.at(i+2)))
               {
                   posCuts<< i-(tracker);  // minus :  (tracker/2) OR (tracker/3) OR tracker
                   tracker=0;
               }
        }
    }
    qSort(posCuts.begin(), posCuts.end(), qLess<int>());  // [Solved]
    for (int j = 0; j < posCuts.length(); ++j)   // temporary Visualization [ SHOW RESULT ].
    {
        //Qdebug()<<"Cut >_"<<QString::number(posCuts.at(j));
        for(int i=0; i< showImage.rows; i++)
        {
            showImage.at<uchar>(i, posCuts.at(j)) = 255;

        }
        for(int i=0; i< Visualization_V.rows; i++)
        {
            Visualization_V.at<uchar>(i, posCuts.at(j)) =  255;
        }
    }
    for(int i=0; i< showImage.cols;i++){ showImage.at<uchar>(PosBaseLine,i) = 127; }
    //imshow("Back Projection cuts",Visualization_V);
    //imshow("Base Line",showImage);

    //! Display the Image Resulted.
    imgInProcessus = QImage((uchar*) showImage.data, showImage.cols, showImage.rows, showImage.step, QImage::Format_Indexed8);
    imgInProcessus.bits();
    //display(imgInProcessus,"Cut Words");
    showImage.copyTo(imageIn);
    listCharacters = getCharactersInWord(clonedImageIn,posCuts);
    // POST PROCESSING NEEDED ONLY [ later inchallah].
    return listCharacters;
    }
    else
    return listCharacters;
}

Mat Segmentation::smoothingHistogramme(Mat image)
{
    int x;
    int black = 0;  // select BLACK color.
    int white = 255;

    for (int i = 1; i < image.rows-2; ++i) {
        for (int j = 1; j < image.cols-2; ++j) {
            x = 0;

            if(image.at<uchar>(i, j) == white)
            {
                if(image.at<uchar>(i+1, j+1) == black) x++;
                if(image.at<uchar>(i-1, j-1) == black) x++;
                if(image.at<uchar>(i, j+1) == black) x++;
                if(image.at<uchar>(i, j-1) == black) x++;
                if(image.at<uchar>(i+1, j) == black) x++;
                if(image.at<uchar>(i-1, j) == black) x++;
                if(image.at<uchar>(i-1, j+1) == black) x++;
                if(image.at<uchar>(i+1, j-1) == black) x++;
            }
            if(x >= 6) image.at<uchar>(i,j) = black;
        }
    }

        for (int i = 1; i < image.rows-2; ++i) {
            for (int j = 1; j < image.cols-2; ++j) {
                x = 0;
                if(image.at<uchar>(i, j) == black)
                {
                    if(image.at<uchar>(i+1, j+1) == white) x++;
                    if(image.at<uchar>(i-1, j-1) == white) x++;
                    if(image.at<uchar>(i, j+1) == white) x++;
                    if(image.at<uchar>(i, j-1) == white) x++;
                    if(image.at<uchar>(i+1, j) == white) x++;
                    if(image.at<uchar>(i-1, j) == white) x++;
                    if(image.at<uchar>(i-1, j+1) == white) x++;
                    if(image.at<uchar>(i+1, j-1) == white) x++;
                }
                if(x >= 4) image.at<uchar>(i,j) = white;
            }
        }
        return image;
}

Mat Segmentation::copyRect(Mat image, int x1, int y1, int x2, int y2)
{

    int ii=0;
    int jj=0;
    int rows = x2-x1;
    int cols = y2-y1;
    //Qdebug()<<QString::number(rows)+"X"+QString::number(cols);
    Mat partImage(rows,cols, CV_8UC1, Scalar(0));
    for(int i=x1; i<x2; i++)
    {
        jj=0;
        for(int j=y1; j<y2;j++)
        {
            partImage.at<uchar>(ii,jj) = image.at<uchar>(i,j);
            jj++;
        }
        ii++;
    }
    return partImage;
}

Mat Segmentation::characterNormalization(Mat image)
{
    if(!image.empty() && image.cols>0 && image.rows>0)
    {
        int x1=0,y1=0,x2=0,y2=0;
        int x = 0;
        QList<int> VHist; // = calculateBackProjection(image,V_Hist);
        VHist.clear();
        for (int i = 0; i < image.cols; ++i){
            x=0;
            for (int j = 0; j < image.rows; ++j)
            {
                if(image.at<uchar>(j, i) == 0) x++;
            }
            VHist<<x;
        }

        for(int i=0; i<VHist.length();i++)
        {
            if(VHist.at(i) != 0) { y1 = i; break; }

        }
        for(int i=VHist.length()-2; i >= 0; i--)
        {
            if(VHist.at(i) != 0) { y2 = i; break; }
        }

        QList<int> HHist;
        HHist.clear();  // calculateBackProjection(image,H_Hist);

        for (int i = 0; i < image.rows; ++i){
            x=0;
            for (int j = 0; j < image.cols; ++j){
                if(image.at<uchar>(i, j) == 0) x++;
            }
            HHist<<x;
        }

        for(int i=0; i<HHist.length();i++)
        {
            if(HHist.at(i) != 0) { x1 = i; break; }
        }
        for(int i=HHist.length()-2; i >= 0; i--)
        {
            if(HHist.at(i) != 0) { x2 = i; break;}
        }
        VHist.clear();
        HHist.clear();

        image = copyRect(image,x1,y1,x2,y2);
        cv::resize(image,image,Size(100,100),0,0,INTER_CUBIC);
        threshold(image,image,127,255,CV_THRESH_BINARY);
       // imwrite(QString::number(w).toStdString()+".png",image);   w++;   // For Demonstration Only.
    }
    return image;
}

int Segmentation::averageLineHeight(QList<int> list)
{
    QList<int> linesHeight;
    linesHeight.clear();

    bool lock = false;
    if(!list.isEmpty())
    {
        int i=0;
        while(i<list.length())
        {
            int j=0;
            if(list.at(i) > 0)
            {
                lock = true;

                j++;
                i++;

                while (lock)
                {
                    if(list.at(i) <= 0)
                    {
                        linesHeight<<j;
                        lock=false;
                        j=-1;
                    }

                    j++;
                    i++;
                }
            }
            else i++;
        }
    }

    int average=0;
    for(int i=0; i<linesHeight.length(); i++)
    {
        average+=linesHeight.at(i);
    }
    average = (int)((average/linesHeight.length())/1.5);

    return average;

}
