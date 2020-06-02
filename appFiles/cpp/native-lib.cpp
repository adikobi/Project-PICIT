#include <jni.h>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include "maxflow/graph.h"
#include "collage.h"
#include "data.h"
#ifdef __ANDROID__
#include "android/bitmap.h"

#include <vector>
#include <map>


using namespace std;
using namespace cv;

const string winName = "Image";
vector<Mat> imageMats;
map<int, vector<int>> pointsPerImage;
void static computeGradient(const Mat &I, Mat *Gx, Mat *Gy);
int numOfImages = 0;

extern "C"{

    void initData(Data& D){
    D.sources = vector<cv::Mat>(numOfImages);
    D.gradientYSources = vector<cv::Mat>(numOfImages);
    D.gradientXSources = vector<cv::Mat>(numOfImages);
    D.offsets = vector<Vec2d>(numOfImages);

    for (int i = 0; i < numOfImages; ++i){
        D.sources[i] = imageMats[i];
    }
    /* Compute gradient images for all sources */
    for (int i = 0; i < numOfImages; i ++) {
        Mat I = D.sources[i];
        int m = I.rows, n = I.cols;
        Mat Gx(m, n, CV_64FC3);
        Mat Gy(m, n, CV_64FC3);
        computeGradient(I, &Gx, &Gy);
        D.gradientXSources[i] = Gx;
        D.gradientYSources[i] = Gy;
    }

    /* Define offsets for all sources */
    for (int i = 0; i < numOfImages; i++) {
        Vec2d offset = Vec2d(0, 0);
        D.offsets[i] = offset;
    }


    /* Define result image and constraints associated */
    D.height = D.sources[0].rows;
    D.width = D.sources[0].cols;
    D.SourceConstraints = cv::Mat(D.height, D.width, CV_8UC1);
    for (int i = 0; i < D.height; i++) {
        for (int j = 0; j < D.width; j++) {
            D.SourceConstraints.at<uchar>(i, j) = 255;
        }
    }
    D.selectSource = 0;
    D.Draw = D.sources[0].clone();
}


void getElements(int start, int end, int* srcArray, vector<int>& dest)
{
    int i = start;
    while (i < end - 1){
        dest.push_back(srcArray[i]);
        dest.push_back(srcArray[i + 1]);
        i += 4;
    }
}


void divideArrayToSepareteArrays(int* srcArray)
{
    numOfImages = srcArray[0];
    int start = numOfImages + 1;
    int end = 0;
    for(int i = 0; i < numOfImages; ++i)
    {
        int numOfPoints = srcArray[i + 1];
        end = start + numOfPoints;
        vector<int> point;
        getElements(start, end, srcArray, point);
        start = end;
        pointsPerImage.insert(pair<int, vector<int>>(i, point));
    }
}

void updateImage(vector<int> points, Data* D)
{
    int length = points.size();
    int i = 0;
    int x, y;
    while (i < length - 1){
        x = points.at(i);
        y = points.at(i + 1);
        Point p0(x, y);
        circle(D->Draw, p0, 30, D->colors[D->selectSource], 30);
        circle(D->SourceConstraints, p0, 30, D->selectSource, 30);
        i += 2;
    }
    D->selectSource++;

    if (D->selectSource < D->sources.size()) {
        Vec2d offset = D->offsets[D->selectSource];
        D->Draw = D->sources[0].clone();


        for (int k = 0; k < D->width; k++) {
            for (int j = 0; j < D->height; j++) {
                if (Collage::isInImage(j, k, offset[0], offset[1], D->sources[D->selectSource])) {

                    unsigned char * p = (D->Draw).ptr(j, k); // Y first, X after
                    unsigned char * pp = (D->sources[D->selectSource]).ptr(j, k); // Y first, X after

                    p[0] = pp[0];   // B
                    p[1] = pp[1];   // G
                    p[2] = pp[2]; // R

                }
                if (D->SourceConstraints.at<uchar>(j, k) != 255) {
                    unsigned char * p1 = (D->Draw).ptr(j, k); // Y first, X after
                    unsigned char * pp1 = (D->SourceConstraints).ptr(j, k); // Y first, X after

                    p1[0] = pp1[0];   // B
                    p1[1] = pp1[1];   // G
                    p1[2] = pp1[2]; // R

                }
            }
        }
    }
}

Mat computePhotomontage(Data* D) {
    Collage C(D);

    Mat* resultsImage = new Mat (D->height, D->width, CV_8UC3);
    *resultsImage = C.computePhotomontage(DESIGNATED_SOURCE, COLORS_AND_GRADIENTS);
    return *resultsImage;
}

Mat static createFinalImage()
{
    Data D;
    initData(D);
    for (int i = 0; i < numOfImages; ++i)
    {
        vector<int> points = pointsPerImage.at(i);
        updateImage(points, &D);
    }
    Mat* resultsImage = new Mat (D.height, D.width, CV_8UC3);
    *resultsImage = computePhotomontage(&D);

    return *resultsImage;
}


static void computeGradient(const Mat &I, Mat *Gx, Mat *Gy) {
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


JNIEXPORT void JNICALL Java_com_example_trycpp_SecondActivity_getImageFromAndroid__J(JNIEnv*, jobject, jlong getImage) {
    Mat& gIm = *(Mat*)getImage;
    imageMats.push_back(gIm);
}

}

#endif //__ANDROID__
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_trycpp_algoCalculation_00024RunAlgo_run(JNIEnv* env, jobject, jintArray  inJNIArray, jlong addrGray) {
    Mat& mGray = *(Mat*)addrGray;
    jint *inCArray = (env)->GetIntArrayElements(inJNIArray, NULL);
    divideArrayToSepareteArrays(inCArray);
    (env)->ReleaseIntArrayElements(inJNIArray, inCArray, 0);// release resources
    Mat* resultsImage = new Mat(imageMats[0].rows, imageMats[0].cols, CV_8UC3);

    *resultsImage = createFinalImage();

    mGray = *resultsImage;
    return (jint)0;
}