#include "preprocess.h"

Preprocess::Preprocess(QObject *parent) : QObject(parent)
{
}

Mat Preprocess::binarizationOTSU(const Mat &image)
{
    Mat _imgInProcessus;
    Mat _imgBW;

    //! [ UNDER CONTROL ] 04/03/2015   19:59
    GaussianBlur(image, _imgInProcessus, Size(3, 3), 0, 0);
    cvtColor(_imgInProcessus, _imgBW, CV_RGB2GRAY);

    //!  OTSU Globale [ Suit the Printer ].
    threshold(_imgBW, _imgBW, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    return _imgBW;
}

Mat Preprocess::binarizationGlobal(const Mat &image, int Threshold)
{
    Mat _imgInProcessus;
    Mat _imgBW;

    //! [ UNDER CONTROL ] 04/03/2015   19:59
    GaussianBlur(image, _imgInProcessus, Size(3, 3), 0, 0);
    cvtColor(_imgInProcessus, _imgBW, CV_RGB2GRAY);

    //!  Glabale Threshold [ Suit the Printer ].
    threshold(_imgBW, _imgBW, Threshold, 255, CV_THRESH_BINARY);
    return _imgBW;
}

Mat Preprocess::convert2cxx(const Mat &image)
{
    // Fill the label_image with the blobs
    // 0  - background
    // 1  - unlabelled foreground
    // 2+ - labelled foreground
    std::vector<std::vector<Point2i>> blobs;
    blobs.clear();

    threshold(image, image, 0.0, 1.0, THRESH_BINARY_INV);
    Mat output = Mat::zeros(image.size(), CV_8UC3);
    Mat label_image;

    image.convertTo(label_image, CV_32SC1);

    int label_count = 2; // starts at 2 because 0,1 are used already

    for (int y = 0; y < label_image.rows; y++)
    {
        int *row = (int *)label_image.ptr(y);
        for (int x = 0; x < label_image.cols; x++)
        {
            if (row[x] != 1) continue;

            Rect rect;
            floodFill(label_image, Point(x, y), label_count, &rect, 0, 0, 4);

            std::vector<Point2i> blob;

            for (int i = rect.y; i < (rect.y + rect.height); i++)
            {
                int *row2 = (int *)label_image.ptr(i);
                for (int j = rect.x; j < (rect.x + rect.width); j++)
                {
                    if (row2[j] != label_count) continue;
                    blob.push_back(Point2i(j, i));
                }
            }

            blobs.push_back(blob);

            label_count++;
        }
    }

    // Randomy color the blobs
    for (size_t i = 0; i < blobs.size(); i++)
    {
        unsigned char r = 255 * (rand() / (1.0 + RAND_MAX));
        unsigned char g = 255 * (rand() / (1.0 + RAND_MAX));
        unsigned char b = 255 * (rand() / (1.0 + RAND_MAX));

        for (size_t j = 0; j < blobs[i].size(); j++)
        {
            int x = blobs[i][j].x;
            int y = blobs[i][j].y;

            output.at<Vec3b>(y, x)[0] = b;
            output.at<Vec3b>(y, x)[1] = g;
            output.at<Vec3b>(y, x)[2] = r;
        }
    }
    // imshow("CXX Colors",output);
    cvtColor(output, output, CV_BGR2GRAY);
    // Remove Black.
    for (int i = 0; i < output.rows; i++)
        for (int j = 0; j < output.cols; ++j)
        {
            if (output.at<uchar>(i, j) == 0)
                output.at<uchar>(i, j) = 255;
        }
    return output;
}

Mat Preprocess::copyRect(const Mat &image, int x1, int y1, int x2, int y2)
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

Mat Preprocess::computeSkewAndRotate(Mat image)
{
    // gray image.
    Size size = image.size();
    bitwise_not(image, image); // invert every bit.
    // cv::imshow("cccccccccc", image);
    vector<Vec4i> lines;
    HoughLinesP(image, lines, 1, CV_PI / 180, 100, 0, image.cols); //, size.width / 2.f, 20
    Mat disp_lines(size, CV_8UC1, Scalar(0, 0, 0));
    double angle = 0.;
    unsigned nb_lines = lines.size();
    for (unsigned i = 0; i < nb_lines; ++i)
    {
        line(disp_lines, Point(lines[i][0], lines[i][1]),
             Point(lines[i][2], lines[i][3]), Scalar(255, 0, 0));
        angle += atan2((double)lines[i][3] - lines[i][1],
                       (double)lines[i][2] - lines[i][0]);
    }
    angle /= nb_lines; // mean angle, in radians.
    // imshow("Angle equals : "+QString::number(angle).toStdString(), disp_lines);
    //  second Phase.
    int len = max(image.cols, image.rows);
    Point2f pt(len / 2., len / 2.);
    Mat r = getRotationMatrix2D(pt, angle * 180 / CV_PI, 1.0);
    warpAffine(image, image, r, image.size());
    GaussianBlur(image, image, Size(5, 3), 0, 0);
    // threshold(image,image,0,255,CV_THRESH_BINARY+CV_THRESH_OTSU);
    bitwise_not(image, image);

    return image;
}

Mat Preprocess::contour(const Mat &image)
{
    int x; // Parametre of dicision <point is contour or not>.
    int whiteColor = 255;
    pos point;
    QList<pos> contourPointsFound; // keeping contour points only.
    Mat contourMat = image.clone();
    for (int j = 1; j < image.cols - 1; j++)
    {
        for (int i = 1; i < image.rows - 1; i++)
        {
            contourPointsFound.clear();
            x = 0;
            if (image.at<uchar>(i, j) == whiteColor)
                x++;
            else
            {
                point.x = i;
                point.y = j;
                contourPointsFound << point;
            }

            if (image.at<uchar>(i + 1, j) == whiteColor)
                x++;
            else
            {
                point.x = i + 1;
                point.y = j;
                contourPointsFound << point;
            }

            if (image.at<uchar>(i, j + 1) == whiteColor)
                x++;
            else
            {
                point.x = i;
                point.y = j + 1;
                contourPointsFound << point;
            }

            if (image.at<uchar>(i + 1, j + 1) == whiteColor)
                x++;
            else
            {
                point.x = i + 1;
                point.y = j + 1;
                contourPointsFound << point;
            }

            if (x > 0)
            { //! Draw The ONLY Contour In New MATRIX.
                for (int k = 0; k < contourPointsFound.length(); k++)
                {
                    contourMat.at<uchar>((contourPointsFound.at(k)).x, (contourPointsFound.at(k)).y) = 87;
                }
            }
        }
    }
    // Remove white pixels, Only contour remain in white.
    for (int j = 1; j < contourMat.cols; j++)
        for (int i = 1; i < contourMat.rows; i++)
        {
            if (contourMat.at<uchar>(i, j) == whiteColor)
                contourMat.at<uchar>(i, j) = 0;
            if (contourMat.at<uchar>(i, j) == 87)
                contourMat.at<uchar>(i, j) = 255;
        }    
    return contourMat;
}

Mat Preprocess::Thinning(Mat image)
{
    //! [Under Controle]   02/03/2015 16:44
    threshold(image, image, 10, 255, CV_THRESH_BINARY_INV);
    thinning(image);
    threshold(image, image, 10, 255, CV_THRESH_BINARY_INV);
    return image;
}

void Preprocess::thinningIteration(Mat &im, int iter)
{
    Mat marker = Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows - 1; i++)
    {
        for (int j = 1; j < im.cols - 1; j++)
        {
            uchar p2 = im.at<uchar>(i - 1, j);
            uchar p3 = im.at<uchar>(i - 1, j + 1);
            uchar p4 = im.at<uchar>(i, j + 1);
            uchar p5 = im.at<uchar>(i + 1, j + 1);
            uchar p6 = im.at<uchar>(i + 1, j);
            uchar p7 = im.at<uchar>(i + 1, j - 1);
            uchar p8 = im.at<uchar>(i, j - 1);
            uchar p9 = im.at<uchar>(i - 1, j - 1);

            int A = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
                    (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
                    (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
                    (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
            int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
            int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
            int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                marker.at<uchar>(i, j) = 1;
        }
    }

    im &= ~marker;
}

void Preprocess::thinning(Mat &im)
{
    im /= 255;

    Mat prev = Mat::zeros(im.size(), CV_8UC1);
    Mat diff;

    do
    {
        thinningIteration(im, 0);
        thinningIteration(im, 1);
        absdiff(im, prev, diff);
        im.copyTo(prev);
    } while (countNonZero(diff) > 0);

    im *= 255;
}
