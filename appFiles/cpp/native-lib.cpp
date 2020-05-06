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
//#include <opencv2/android/Utils.>
#define LOG_TAG "org.opencv.android.Utils"
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

//jstring //TODO - how to return
//Java_com_example_trycpp_MainActivity_validate(JNIEnv* env, jobject /* this */, jlong addrGrey, jlong addrRgba) {
//    cv :: Rect();
//    cv :: Mat();
//    std::string hellow2 = "Hellow from validate";
//    return env->NewStringUTF(hellow2.c_str());
//}


void initData(Data* P){
    Data D;
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
    *P = D;
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
        circle(D->Draw, p0, 2, D->colors[D->selectSource], 2);
        circle(D->SourceConstraints, p0, 2, D->selectSource, 2);
        i += 2;
    }
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
    }
//    else {
//        computePhotomontage(D);
//        cout << "Photomontage computed." << endl;
//    }

}

Mat computePhotomontage(Data* D) {
    Collage C(D);
    Mat* resultsImage = new Mat(D->sources[0].rows, D->sources[0].cols, CV_8UC3);
    *resultsImage = C.computePhotomontage(DESIGNATED_SOURCE, COLORS_AND_GRADIENTS).clone();
    return *resultsImage;
}


//TODO // maybe we need to pass the array of points and not let it be static
Mat static createFinalImage()
{
    Data D;
    initData(&D);
    for (int i = 0; i < numOfImages; ++i)
    {
        vector<int> points = pointsPerImage.at(i);
        updateImage(points, &D);
    }
    Mat* resultsImage = new Mat(D.sources[0].rows, D.sources[0].cols, CV_8UC3);
    *resultsImage = computePhotomontage(&D);
    return *resultsImage;
}


jobject MatToBitmap(JNIEnv * env, Mat isrc, jobject bitmap)
{
    AndroidBitmapInfo  info;
    void*              pixels = 0;
    Mat&               src = isrc;

    try {
        CV_Assert( AndroidBitmap_getInfo(env, bitmap, &info) >= 0 );
        CV_Assert( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                   info.format == ANDROID_BITMAP_FORMAT_RGB_565 );
        CV_Assert( src.dims == 2 && info.height == (uint32_t)src.rows && info.width == (uint32_t)src.cols );
        CV_Assert( src.type() == CV_8UC1 || src.type() == CV_8UC3 || src.type() == CV_8UC4 );
        CV_Assert( AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0 );
        CV_Assert( pixels );
        if( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 )
        {
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if(src.type() == CV_8UC1)
            {
                cvtColor(src, tmp, COLOR_GRAY2RGBA);
            } else if(src.type() == CV_8UC3){
                cvtColor(src, tmp, COLOR_RGB2RGBA);
            } else if(src.type() == CV_8UC4){
                src.copyTo(tmp);
            }
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if(src.type() == CV_8UC1)
            {
                cvtColor(src, tmp, COLOR_GRAY2BGR565);
            } else if(src.type() == CV_8UC3){
                cvtColor(src, tmp, COLOR_RGB2BGR565);
            } else if(src.type() == CV_8UC4){
                cvtColor(src, tmp, COLOR_RGBA2BGR565);
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return bitmap;
    } catch(const cv::Exception& e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("org/opencv/core/CvException");
        if(!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return bitmap;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nMatToBitmap}");
        return bitmap;
    }
}



//JNIEXPORT jdoubleArray JNICALL
JNIEXPORT jobject JNICALL
Java_com_example_trycpp_SecondActivity_getPoints(JNIEnv* env, jobject /* this */, jintArray  inJNIArray, jobject bitmap) {
    jint *inCArray = (env)->GetIntArrayElements(inJNIArray, NULL);
    if (NULL == inCArray) return bitmap;
    jsize length = (env)->GetArrayLength(inJNIArray);
    divideArrayToSepareteArrays(inCArray);
    (env)->ReleaseIntArrayElements(inJNIArray, inCArray, 0);// release resources
    Mat* resultsImage = new Mat(imageMats[0].rows, imageMats[0].cols, CV_8UC3);
    *resultsImage = createFinalImage();

//    //get source bitmap's config
//    jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap");
//    jmethodID mid = env->GetMethodID(java_bitmap_class, "getConfig", "()Landroid/graphics/Bitmap$Config;");
//    jobject bitmap_config = env->CallObjectMethod(bitmap, mid);
    jobject _bitMap = MatToBitmap(env, *resultsImage, bitmap);
    return _bitMap;
    //AndroidBitmap_unlockPixels(env, bitmap);

//    //and finally you can get bitmap for android
//    return _bitmap;
}

//jint
//java_com_example_trycpp_MainActivity_onTouch()
//{
//
//}



//void static onMouse(int event, int x, int y, int flag, void* p) {
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



JNIEXPORT jobject JNICALL
Java_com_example_trycpp_SecondActivity_getImage(::JNIEnv* env, jobject thisobject, jobject bitmap){
    AndroidBitmapInfo  info;
    void*              pixels = 0;
    Mat&               dst = *(new Mat);

    try {
        CV_Assert( AndroidBitmap_getInfo(env, bitmap, &info) >= 0 );
        CV_Assert( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                   info.format == ANDROID_BITMAP_FORMAT_RGB_565 );
        CV_Assert( AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0 );
        CV_Assert( pixels );
        dst.create(info.height, info.width, CV_8UC4);
        if( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 )
        {
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            tmp.copyTo(dst);
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            cvtColor(tmp, dst, COLOR_BGR5652RGBA);
        }
        imageMats.push_back(dst);
        return bitmap;
//        jobject _bitMap = MatToBitmap(env, dst, bitmap2);
        AndroidBitmap_unlockPixels(env, bitmap);
        return bitmap;
    } catch(const cv::Exception& e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("org/opencv/core/CvException");
        if(!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return bitmap;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nBitmapToMat}");
        return bitmap;
    }
}



JNIEXPORT Mat JNICALL
Java_com_example_trycpp_SecondActivity_start(::JNIEnv* env, jobject thisobject, jint numOfImage, jobject matImage){

    //namedWindow(winName);

    Data D;
    D.sources = vector<cv::Mat>(numOfImage);
    D.gradientYSources = vector<cv::Mat>(numOfImage);
    D.gradientXSources = vector<cv::Mat>(numOfImage);
    D.offsets = vector<Vec2d>(numOfImage);

    /*int N = 3;
    D.sources[0] = imread("../riviere.jpg");
    D.sources[1] = imread("../maison.jpg");
    D.sources[2] = imread("../cascade.jpg");
    */

    // todo change to the real directory
    for (int i = 0; i < numOfImage; ++i)
    {
        //D.sources[i] = *(cv::Mat*)env->CallLongMethod(env->GetObjectArrayElement(bufimgsArray, i), getPtrMethod);
        //Mat& input = *(Mat*)imageMats;
        //D.sources[i] = input.clone();
    }

    /* Compute gradient images for all sources */
    for (int i = 0; i < numOfImage; i ++) {
        Mat I = D.sources[i];
        int m = I.rows, n = I.cols;
        Mat Gx(m, n, CV_64FC3);
        Mat Gy(m, n, CV_64FC3);
        computeGradient(I, &Gx, &Gy);
        D.gradientXSources[i] = Gx;
        D.gradientYSources[i] = Gy;
    }

    /* Define offsets for all sources */
    for (int i = 0; i < numOfImage; i++) {
        Vec2d offset = Vec2d(0, 0);
        D.offsets[i] = offset;
    }


    /* Define result image and constraints associated */
    D.height = D.sources[0].rows;
    D.width = D.sources[0].cols;
    D.SourceConstraints = cv::Mat(D.height, D.width, CV_8UC3);
    for (int i = 0; i < D.height; i++) {
        for (int j = 0; j < D.width; j++) {
            D.SourceConstraints.at<uchar>(i, j) = 255;
        }
    }

    D.selectSource = 0;
    D.Draw = D.sources[0].clone(); //todo
    //return D.Draw;
    return D.Draw;
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


int toGray(Mat img, Mat& gray)
{
    cvtColor(img, gray, CV_RGBA2GRAY); // Assuming RGBA input
    //cvtColor(gray, gray, CV_GRAY2RGB); // Assuming RGBA input

    if (gray.rows == img.rows && gray.cols == img.cols)
    {
        return (1);
    }
    return(0);
////    gray = img;
////    Mat im = new Mat()
////    //mageMats.push_back(img);
//    cvtColor(imageMats[0], gray, CV_RGB2GRAY); // Assuming RGBA input
//    cvtColor(gray, gray, CV_GRAY2RGB); // Assuming RGBA input
//
//
//    if (gray.rows == img.rows && gray.cols == img.cols)
//    {
//        return (1);
//    }
//    return(0);
}

JNIEXPORT jint JNICALL Java_com_example_trycpp_SecondActivity_run(JNIEnv* env, jobject, jintArray  inJNIArray, jlong addrGray) {
    // run the all program, the algorithm
    Mat& mGray = *(Mat*)addrGray;
    jint *inCArray = (env)->GetIntArrayElements(inJNIArray, NULL);
    //if (NULL == inCArray) return bitmap;
    jsize length = (env)->GetArrayLength(inJNIArray);
    divideArrayToSepareteArrays(inCArray);
    (env)->ReleaseIntArrayElements(inJNIArray, inCArray, 0);// release resources
    Mat* resultsImage = new Mat(imageMats[0].rows, imageMats[0].cols, CV_8UC3);
    *resultsImage = createFinalImage();
    mGray = *resultsImage;
    return (jint)0;

}

JNIEXPORT void JNICALL Java_com_example_trycpp_SecondActivity_getImageFromAndroid(JNIEnv*, jobject, jlong getImage) {

    Mat& gIm = *(Mat*)getImage;
    imageMats.push_back(gIm);

}


JNIEXPORT jint JNICALL Java_com_example_trycpp_SecondActivity_convertNativeGray(JNIEnv*, jobject, jlong addrRgba, jlong addrGray) {

    Mat& mRgb = *(Mat*)addrRgba;
    Mat& mGray = *(Mat*)addrGray;

    int conv;
    jint retVal;

    conv = toGray(mRgb, mGray);
    retVal = (jint)conv;

    return retVal;

}

}




////JNIEXPORT jdoubleArray JNICALL
//JNIEXPORT void JNICALL
//Java_com_example_trycpp_SecondActivity_getPoints(JNIEnv* env, jobject /* this */, jintArray  inJNIArray) {
//    jint *inCArray = (env)->GetIntArrayElements(inJNIArray, NULL);
//    if (NULL == inCArray) return;
//    jsize length = (env)->GetArrayLength(inJNIArray);
//
//    // Step 2: Perform its intended operations
////    jint sum = 0;
////    int i;
////    for (i = 0; i < length; i++) {
////        sum += inCArray[i];
////    }
////    jdouble average = (jdouble)sum / length;
//    divideArrayToSepareteArrays(inCArray);
//    (env)->ReleaseIntArrayElements(inJNIArray, inCArray, 0); // release resources
//
////    //the values we want to return - we changed it to 0 stam
////    jdouble outCArray[] = {0, 0};
////
//////    // Step 3: Convert the C's Native jdouble[] to JNI jdoublearray, and return
//////    jdoubleArray outJNIArray = (env)->NewDoubleArray(2);  // allocate
//////    if (NULL == outJNIArray) return NULL;
//////    (env)->SetDoubleArrayRegion(outJNIArray, 0 , 2, outCArray);  // copy
//////    return outJNIArray;
//}


#endif //__ANDROID__