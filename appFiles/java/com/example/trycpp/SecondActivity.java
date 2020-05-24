package com.example.trycpp;

import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;

import com.airbnb.lottie.LottieAnimationView;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import java.util.ArrayList;


public class SecondActivity extends AppCompatActivity {
    static int N;
    static Bitmap[] imageBitmaps;
    ImageView imageView2;
    LottieAnimationView lottieanimationview;
    ImageButton next;
    public static int[] po;
    public static long address;

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

            float scale = Math.min((float)view.getWidth() / imageBitmaps[0].getWidth(), (float)view.getHeight() / imageBitmaps[0].getHeight());
            float offset_x = ((float)view.getWidth() - imageBitmaps[0].getWidth() * scale) / 2;
            float offset_y = ((float)view.getHeight() - imageBitmaps[0].getHeight() * scale) / 2;

            int x = (int) motionEvent.getX();
            int y = (int) motionEvent.getY();

            int update_x = Math.round((x - offset_x)/scale);
            int update_y = Math.round((y - offset_y)/scale);
            // this add every point twice
            points.add(update_x);
            points.add(update_y);
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
            po = convertArrayListToArray(points);
            Bitmap result = Bitmap.createBitmap(imageBitmaps[0].getWidth(), imageBitmaps[0].getHeight(), Bitmap.Config.ARGB_8888);
            Mat newMat = new Mat(imageBitmaps[0].getHeight(), imageBitmaps[0].getWidth(), CvType.CV_8UC3);


            Intent myIntent = new Intent(SecondActivity.this, algoCalculation.class);
            address = newMat.getNativeObjAddr();
            SecondActivity.this.startActivity(myIntent);

//            run(po, newMat.getNativeObjAddr()); // run the algorithm
//            Utils.matToBitmap(newMat, result);
//            imageView2.setImageBitmap(result);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_second);
        imageView2 = (ImageView) findViewById(R.id.imageView2);
        next = (ImageButton) findViewById(R.id.nextButton);
        imageBitmaps = MainActivity.imageBitMaps;
        lottieanimationview = findViewById(R.id.animation_view);

        N = MainActivity.counter;

        // save place to num of points in every image
        for (int i = 0; i <= N; ++i)
        {
            points.add(0);
        }

        for (Bitmap image : imageBitmaps)
        {
            Mat MatToSent = new Mat(image.getHeight(), image.getWidth(), CvType.CV_8UC3);
            Utils.bitmapToMat(image, MatToSent);

            // send the image to cpp as mat
            getImageFromAndroid(MatToSent.getNativeObjAddr());
        }
        showIm(imageBitmaps[0]);
        imageView2.setOnTouchListener(handleTouch);
    }



    public native void getImageFromAndroid(long getImage);
//    public static native int run(int[] points, long addrGray);

    //public native int convertNativeGray(long matAddrRgba, long matAddrGray);
    //public native Bitmap getPoints(int[] points, Bitmap imageMat);
    //public native Mat start(int a, Mat imageMat);
    //public native Bitmap getImage(Bitmap imageMat);
}
