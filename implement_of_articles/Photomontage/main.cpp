//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <iostream>
//#include "maxflow/graph.h"
//#include "collage.h"
//#include "data.h"
//#include <map>
//
//using namespace std;
//using namespace cv;
//
//const string winName = "Image";
//void static computeGradient(const Mat &I, Mat *Gx, Mat *Gy);
//vector<int> points;
//int counter = 1;
//int numOfElement = 0;
//int numOfImages = 0;
//map<int, vector<int> > pointsPerImage;
//
////void computePhotomontage(Data* D) {
////    Collage C(D);
////    cout << "Computing photo montage...";
////    C.computePhotomontage(DESIGNATED_SOURCE, COLORS_AND_GRADIENTS);
////    cout << "Done.";
////}
//
//Mat computePhotomontage1(Data* D) {
//    Collage C(D);
//    Mat resultsImage = C.computePhotomontage(DESIGNATED_SOURCE, COLORS_AND_GRADIENTS);
//    return resultsImage;
//}
//
//void updateImage(vector<int> points, Data* D)
//{
//    int length = points.size();
//    int i = 0;
//    int x, y;
//    while (i < length - 1){
//        x = points.at(i);
//        y = points.at(i + 1);
//        Point p0(x, y);
//        circle(D->Draw, p0, 2, D->colors[D->selectSource], 2);
//        circle(D->SourceConstraints, p0, 2, D->selectSource, 2);
//        i += 2;
//    }
//    imshow(winName, D->Draw);
//    D->selectSource++;
//    if (D->selectSource < D->sources.size()) {
//        Vec2d offset = D->offsets[D->selectSource];
//        D->Draw = D->sources[0].clone();
//        for (int i = 0; i < D->height; i++) {
//            for (int j = 0; j < D->width; j++) {
//                if (Collage::isInImage(i, j, offset[0], offset[1], D->sources[D->selectSource])) {
//                    D->Draw.at<Vec3b>(i, j) = D->sources[D->selectSource].at<Vec3b>(i - offset[0], j - offset[1]);
//                }
//                if (D->SourceConstraints.at<uchar>(i,j) != 255) {
//                    D->Draw.at<Vec3b>(i, j) = D->colors[D->SourceConstraints.at<uchar>(i, j)];
//                }
//            }
//        }
//    }
////    else {
////        computePhotomontage(D);
////        cout << "Photomontage computed." << endl;
////    }
//
//}
//
//
//void getElements(int start, int end, vector<int>* srcArray, vector<int>& dest)
//{
//    int i = start;
//    while (i < end - 1){
//        dest.push_back((*srcArray)[i]);
//        dest.push_back((*srcArray)[i + 1]);
//        i += 2;
//    }
//}
//
//void divideArrayToSepareteArrays(vector<int>* srcArray)
//{
//    numOfImages = (*srcArray)[0];
//    int start = numOfImages + 1;
//    int end = 0;
//    for(int i = 0; i < numOfImages; ++i)
//    {
//        int numOfPoints = (*srcArray)[i + 1];
//        end = start + numOfPoints;
//        vector<int> point;
//        getElements(start, end, srcArray, point);
//        start = end;
//        //pair<int, vector<int> > p = make_pair(i, point);
//        pointsPerImage.insert(make_pair(i, point));
//    }
//}
//
//void static onMouse(int event, int x, int y, int flag, void* p) {
//    Data* D = (Data*)p;
//    if (event == EVENT_LBUTTONDOWN) {
//        points.push_back(x);
//        points.push_back(y);
//        numOfElement += 2;
//    }
//    else if (event == EVENT_RBUTTONDOWN) {
//        D->selectSource++;
//        if (D->selectSource < D->sources.size()) {
//            points.at(counter) = numOfElement;
//            counter += 1;
//            numOfElement = 0;
//            D->Draw = D->sources[D->selectSource].clone();
//            imshow(winName, D->Draw);
//        }
//        else{
//            points.at(counter) = numOfElement;
//            points.at(0) = counter;
//            D->selectSource = D->selectSource - counter;
//            divideArrayToSepareteArrays(&points);
//            for (int i = 0; i < counter; ++i)
//            {
//                vector<int> point = pointsPerImage.at(i);
//                updateImage(point, D);
//            }
//            //computePhotomontage(D);
//
//            Mat resultImage = computePhotomontage1(D);
//            imshow("Photomontage", resultImage);
//            //computePhotomontage(D);
//        }
//    }
//}
//
//
//
//
//void static onMouse2(int event, int x, int y, int flag, void* p) {
//    Data* D = (Data*)p;
//    if (event == EVENT_LBUTTONDOWN) {
//        Point p0(x, y);
//        circle(D->Draw, p0, 2, D->colors[D->selectSource], 2);
//        circle(D->SourceConstraints, p0, 2, D->selectSource, 2);
//        imshow(winName, D->Draw);
//    }
//    else if (event == EVENT_RBUTTONDOWN) {
//        D->selectSource++;
//        if (D->selectSource < D->sources.size()) {
//            Vec2d offset = D->offsets[D->selectSource];
//            D->Draw = D->sources[0].clone();
//            for (int i = 0; i < D->height; i++) {
//                for (int j = 0; j < D->width; j++) {
//                    if (Collage::isInImage(i, j, offset[0], offset[1], D->sources[D->selectSource])) {
//                        D->Draw.at<Vec3b>(i, j) = D->sources[D->selectSource].at<Vec3b>(i - offset[0], j - offset[1]);
//                    }
//                    if (D->SourceConstraints.at<uchar>(i,j) != 255) {
//                        D->Draw.at<Vec3b>(i, j) = D->colors[D->SourceConstraints.at<uchar>(i, j)];
//                    }
//                }
//            }
//            imshow(winName, D->Draw);
//        }
//        else {
//            setMouseCallback(winName, NULL, NULL);
//            computePhotomontage(D);
//            cout << "Photomontage computed." << endl;
//        }
//    }
//}
//
//int main() {
//    namedWindow(winName);
//    Data D;
//    int N = 2;
//    D.sources = vector<Mat>(N);
//    D.gradientYSources = vector<Mat>(N);
//    D.gradientXSources = vector<Mat>(N);
//    D.offsets = vector<Vec2d>(N);
//
//    /*int N = 3;
//    D.sources[0] = imread("../riviere.jpg");
//    D.sources[1] = imread("../maison.jpg");
//    D.sources[2] = imread("../cascade.jpg");
//    */
//
//    // todo change to the real directory
//    D.sources[0] = imread("/Users/chana/Desktop/pic22.jpeg");
//    D.sources[1] = imread("/Users/chana/Desktop/pic11.jpeg");
////    D.sources[2] = imread("/Users/chana/Desktop/pic3.jpeg");
////	D.sources[3] = imread("/cs/usr/adi.kobi/Desktop/PhotoMontage-master (2)/images/famille4.jpg");
//
//    /* Compute gradient images for all sources */
//    for (int i = 0; i < N; i ++) {
//        Mat I = D.sources[i];
//        int m = I.rows, n = I.cols;
//        Mat Gx(m, n, CV_64FC3);
//        Mat Gy(m, n, CV_64FC3);
//        computeGradient(I, &Gx, &Gy);
//        D.gradientXSources[i] = Gx;
//        D.gradientYSources[i] = Gy;
//    }
//
//    /* Define offsets for all sources */
//    for (int i = 0; i < N; i++) {
//        Vec2d offset = Vec2d(0, 0);
//        D.offsets[i] = offset;
//    }
//
//
//    /* Define result image and constraints associated */
//    D.height = D.sources[0].rows;
//    D.width = D.sources[0].cols;
//    D.SourceConstraints = Mat(D.height, D.width, CV_8UC1);
//    for (int i = 0; i < D.height; i++) {
//        for (int j = 0; j < D.width; j++) {
//            D.SourceConstraints.at<uchar>(i, j) = 255;
//        }
//    }
//
//    D.selectSource = 0;
//    D.Draw = D.sources[0].clone();
//    for (int i = 0; i < N + 1; ++i)
//    {
//        points.push_back(0);
//    }
//    imshow(winName, D.Draw);
//    setMouseCallback(winName, onMouse, &D);
//    waitKey();
//}
//
//void static computeGradient(const Mat &I, Mat *Gx, Mat *Gy) {
//    int m = I.rows, n = I.cols;
//    for (int i = 0; i < m; i++) {
//        for (int j = 0; j < n; j++) {
//            Vec3d gx, gy;
//            if (i == 0 || i == m-1) {
//                gy = Vec3d(0, 0, 0);
//            } else {
//                gy = ((Vec3d)I.at<Vec3b>(i + 1, j) - (Vec3d)I.at<Vec3b>(i - 1, j)) / 2;
//            }
//
//            if (j == 0 || j == n - 1) {
//                gx = Vec3d(0, 0, 0);
//            } else {
//                gx = ((Vec3d)I.at<Vec3b>(i,j+1)- (Vec3d)I.at<Vec3b>(i,j-1)) / 2;
//            }
//            Gx->at<Vec3d>(i, j) = gx;
//            Gy->at<Vec3d>(i, j) = gy;
//        }
//    }
//}

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "maxflow/graph.h"
#include "collage.h"
#include "data.h"

using namespace std;
using namespace cv;

const string winName = "Image";
void static computeGradient(const Mat &I, Mat *Gx, Mat *Gy);



void computePhotomontage(Data* D) {
    Collage C(D);
    cout << "Computing photo montage...";
    C.computePhotomontage(DESIGNATED_SOURCE, COLORS_AND_GRADIENTS);
    cout << "Done.";
}


void static onMouse(int event, int x, int y, int flag, void* p) {
    Data* D = (Data*)p;
    if (event == EVENT_LBUTTONDOWN) {
        Point p0(x, y);
        circle(D->Draw, p0, 2, D->colors[D->selectSource], 2);
        circle(D->SourceConstraints, p0, 2, D->selectSource, 2);
        imshow(winName, D->Draw);
    }
    else if (event == EVENT_RBUTTONDOWN) {
        D->selectSource++;
        if (D->selectSource < D->sources.size()) {
            Vec2d offset = D->offsets[D->selectSource];
            D->Draw = D->sources[0].clone();
            for (int i = 0; i < D->height; i++) {
                for (int j = 0; j < D->width; j++) {
                    if (Collage::isInImage(i, j, offset[0], offset[1], D->sources[D->selectSource])) {
                        D->Draw.at<Vec3b>(i, j) = D->sources[D->selectSource].at<Vec3b>(i - offset[0], j - offset[1]);
                    }
                    if (D->SourceConstraints.at<uchar>(i,j) != 255) {
                        D->Draw.at<Vec3b>(i, j) = D->colors[D->SourceConstraints.at<uchar>(i, j)];
                    }
                }
            }
            imshow(winName, D->Draw);
        }
        else {
            setMouseCallback(winName, NULL, NULL);
            computePhotomontage(D);
            cout << "Photomontage computed." << endl;
        }
    }
}

int main() {
    namedWindow(winName);
    Data D;
    int N = 3;
    // define for each image grad by y, by x and offsets
    D.sources = vector<Mat>(N);
    D.gradientYSources = vector<Mat>(N);
    D.gradientXSources = vector<Mat>(N);
    D.offsets = vector<Vec2d>(N);

/*int N = 3;
D.sources[0] = imread("../riviere.jpg");
D.sources[1] = imread("../maison.jpg");
D.sources[2] = imread("../cascade.jpg");
*/

// todo change to the real directory
    D.sources[0] = imread("/Users/chana/Desktop/fam1.jpeg");
    D.sources[1] = imread("/Users/chana/Desktop/fam2.jpeg");
    D.sources[2] = imread("/Users/chana/Desktop/fam3.jpeg");
//	D.sources[3] = imread("/cs/usr/adi.kobi/Desktop/PhotoMontage-master (2)/images/famille4.jpg");

/* Compute gradient images for all sources */
    for (int i = 0; i < N; i ++) {
        Mat I = D.sources[i];
        int m = I.rows, n = I.cols;
        Mat Gx(m, n, CV_64FC3);
        Mat Gy(m, n, CV_64FC3);
        computeGradient(I, &Gx, &Gy);
        D.gradientXSources[i] = Gx;
        D.gradientYSources[i] = Gy;
    }

/* Define offsets for all sources */
    for (int i = 0; i < N; i++) {
        Vec2d offset = Vec2d(0, 0);
        D.offsets[i] = offset;
    }


/* Define result image and constraints associated */
    D.height = D.sources[0].rows;
    D.width = D.sources[0].cols;
    D.SourceConstraints = Mat(D.height, D.width, CV_8UC1);
    for (int i = 0; i < D.height; i++) {
        for (int j = 0; j < D.width; j++) {
            D.SourceConstraints.at<uchar>(i, j) = 255;
        }
    }

    D.selectSource = 0;
    D.Draw = D.sources[0].clone();
    imshow(winName, D.Draw);
    setMouseCallback(winName, onMouse, &D);
    waitKey();
}

void static computeGradient(const Mat &I, Mat *Gx, Mat *Gy) {
    int m = I.rows, n = I.cols;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            Vec3d gx, gy;
            if (i == 0 || i == m-1) {
                gy = Vec3d(0, 0, 0);
            } else {
                gy = ((Vec3d)I.at<Vec3b>(i + 1, j) - (Vec3d)I.at<Vec3b>(i - 1, j)) / 2;
            }

            if (j == 0 || j == n - 1) {
                gx = Vec3d(0, 0, 0);
            } else {
                gx = ((Vec3d)I.at<Vec3b>(i,j+1)- (Vec3d)I.at<Vec3b>(i,j-1)) / 2;
            }
            Gx->at<Vec3d>(i, j) = gx;
            Gy->at<Vec3d>(i, j) = gy;
        }
    }
}
