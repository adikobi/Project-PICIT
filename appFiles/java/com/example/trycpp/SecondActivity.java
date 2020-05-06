package com.example.trycpp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.util.ArrayList;
import org.opencv.android.Utils;

public class SecondActivity extends AppCompatActivity {
    static int N;
    static Bitmap[] imageBitmaps;
    ImageView imageView2;
    Button next;
    int counter = 1;
    int numOfElement = 0;

    // array of point that every x in the even coord and the y in the odd
    ArrayList<Integer> points = new ArrayList<>();

    public void showIm(Bitmap im)
    {
        try{
            imageView2.setImageBitmap(im);
        }catch (Exception e){

        }
    }

    private View.OnTouchListener handleTouch = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View view, MotionEvent motionEvent) {
            int x = (int) motionEvent.getX();
            int y = (int) motionEvent.getY();
            // this add every point twice
            points.add(x);
            points.add(y);
            numOfElement += 2;
            return true;
        }
    };

    int[] convertArrayListToArray(ArrayList<Integer> src)
    {
        int[] dest = new int[src.size()];
        for(int i = 0; i < src.size(); ++i)
        {
            dest[i] = src.get(i).intValue();
        }
        return dest;
    }

    public void next_changeImageForTouch(View v){
        if (counter < N){
            try{
                points.set(counter, numOfElement);
                numOfElement = 0;
                Bitmap bmp = imageBitmaps[counter];
                imageView2.setImageBitmap(bmp);
                ++counter;
            }catch (Exception e){
                return;
            }
        }
        else {
            // need to move to other activity
            points.set(counter, numOfElement);
            points.set(0, counter);
            int[] po = convertArrayListToArray(points);
            Bitmap result = Bitmap.createBitmap(imageBitmaps[0].getWidth(), imageBitmaps[0].getHeight(), Bitmap.Config.ARGB_8888);
            Mat newMat = new Mat(imageBitmaps[0].getWidth(), imageBitmaps[0].getHeight(), CvType.CV_8UC3);
            run(po, newMat.getNativeObjAddr()); // do the algorithm
            Utils.matToBitmap(newMat, result);
            imageView2.setImageBitmap(result);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_second);
        imageView2 = (ImageView) findViewById(R.id.imageView2);
        next = (Button) findViewById(R.id.nextButton);
        imageBitmaps = MainActivity.imageBitMaps;
        N = MainActivity.counter;

        // save place to num of points in every image
        for (int i = 0; i <= N; ++i)
        {
            points.add(0);
        }

        for (Bitmap image : imageBitmaps)
        {
            ////Bitmap result = Bitmap.createBitmap(imageBitmaps[0].getWidth(), imageBitmaps[0].getHeight(), Bitmap.Config.ARGB_8888);
            Mat MatToSent = new Mat(image.getWidth(), image.getHeight(), CvType.CV_8UC3);
            //Mat getMat = new Mat(image.getWidth(), image.getHeight(), CvType.CV_8UC1);
            //Bitmap im = Bitmap.createBitmap(image.getWidth(), image.getHeight(), Bitmap.Config.ARGB_8888);
            Utils.bitmapToMat(image, MatToSent);
            // send the image to cpp as mat
            //convertNativeGray(MatToSent.getNativeObjAddr(), getMat.getNativeObjAddr());
            //Utils.matToBitmap(getMat, im);
            getImageFromAndroid(MatToSent.getNativeObjAddr());
            //showIm(imageBitmaps[0]);
//            imageView2.setImageBitmap(im);
//            int a = 6;
//            //getImage(image);
        }

        showIm(imageBitmaps[0]);
        imageView2.setOnTouchListener(handleTouch);


    }


    public native void getImageFromAndroid(long getImage);
    public native int convertNativeGray(long matAddrRgba, long matAddrGray);
    public native int run(int[] points, long addrGray);
    public native Bitmap getPoints(int[] points, Bitmap imageMat);
    public native Mat start(int a, Mat imageMat);
    public native Bitmap getImage(Bitmap imageMat);
}
