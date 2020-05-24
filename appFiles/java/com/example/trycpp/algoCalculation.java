package com.example.trycpp;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.drawable.RoundedBitmapDrawable;
import androidx.core.graphics.drawable.RoundedBitmapDrawableFactory;

import com.airbnb.lottie.LottieAnimationView;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;


public class algoCalculation extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_algo_calculation);
        AsyncTask task = new RunAlgo();
        task.execute(/* optional params */);


    }


    @SuppressLint("StaticFieldLeak")
    public class RunAlgo extends AsyncTask {
        ImageView resultImage ;
        Mat newMat = new Mat(SecondActivity.imageBitmaps[0].getHeight(), SecondActivity.imageBitmaps[0].getWidth(), CvType.CV_8UC3);

        @Override
        protected Object doInBackground(Object[] objects) {
            int[] po =SecondActivity.po;

            run(po, newMat.getNativeObjAddr());
            onPostExecute(null);
            return null;
        }

        protected void onPostExecute(Bitmap returnVal) {
            runOnUiThread(new Runnable() {

                @Override
                public void run() {
                    Bitmap result = Bitmap.createBitmap(SecondActivity.imageBitmaps[0].getWidth(), SecondActivity.imageBitmaps[0].getHeight(), Bitmap.Config.ARGB_8888);
                    resultImage =  (ImageView) findViewById(R.id.resultView);

                    Utils.matToBitmap(newMat, result);
                    RoundedBitmapDrawable round = RoundedBitmapDrawableFactory.create(getResources(), result);
                    round.setCornerRadius(200);
                    resultImage.setImageDrawable(round);

                    LottieAnimationView lottieanimationview;
                    lottieanimationview = findViewById(R.id.animation_view);
                    lottieanimationview.setVisibility(LottieAnimationView.GONE);
                    TextView textView = findViewById(R.id.textView3);
                    TextView pleas_wait = findViewById(R.id.textView5);
                    pleas_wait.setVisibility(View.INVISIBLE);
                    textView.setVisibility(View.GONE);
                }
            });

        }

        public native int run(int[] points, long addrGray);


    }
}