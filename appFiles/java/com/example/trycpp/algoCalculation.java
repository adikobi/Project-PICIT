package com.example.trycpp;

import android.Manifest;
import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.airbnb.lottie.LottieAnimationView;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class algoCalculation extends AppCompatActivity {
    static Bitmap result = Bitmap.createBitmap(SecondActivity.imageBitmaps[0].getWidth(), SecondActivity.imageBitmaps[0].getHeight(), Bitmap.Config.ARGB_8888);
    static ImageView resultImage ;
    static Mat newMat = new Mat(SecondActivity.imageBitmaps[0].getHeight(), SecondActivity.imageBitmaps[0].getWidth(), CvType.CV_8UC3);

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_algo_calculation);
        AsyncTask task = new RunAlgo();
        task.execute(/* optional params */);

    }


    @SuppressLint("StaticFieldLeak")
    public class RunAlgo extends AsyncTask {


        @Override
        protected Object doInBackground(Object[] objects) {
            int[] po =SecondActivity.po;

            run(po, newMat.getNativeObjAddr());
            onPostExecute(null);
            return null;
        }



        private void imagesavetomyphonegallery() {

            ImageView img = algoCalculation.resultImage;
            BitmapDrawable draw = (BitmapDrawable) img.getDrawable();
            Bitmap bitmap = draw.getBitmap();
            FileOutputStream outStream = null;
            File sdCard = Environment.getExternalStorageDirectory();
            File dir = new File(sdCard.getAbsolutePath() + "/SaveImages");
            dir.mkdirs();
            String fileName = String.format("%d.jpg", System.currentTimeMillis());
            File outFile = new File(dir, fileName);
            try {
                outStream = new FileOutputStream(outFile);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            bitmap.compress(Bitmap.CompressFormat.JPEG, 100, outStream);
            try {
                outStream.flush();
            } catch (IOException e) {
                Toast.makeText(algoCalculation.this, e.toString(), Toast.LENGTH_SHORT).show();

                e.printStackTrace();
            }
            try {
                outStream.close();
            } catch (IOException e) {

                e.printStackTrace();
            }
            MediaScannerConnection.scanFile(algoCalculation.this, new String[]{outFile.toString()}, null,
                    new MediaScannerConnection.OnScanCompletedListener() {
                        public void onScanCompleted(String path, Uri uri) {
                        }
                    });
            Toast.makeText(algoCalculation.this, "image saved successfully", Toast.LENGTH_SHORT).show();
        }

        protected void onPostExecute(Bitmap returnVal) {
            runOnUiThread(new Runnable() {

                @Override
                public void run() {
                    LottieAnimationView lottieanimationview;
                    ImageView imageView;
                    ImageButton save;
                    lottieanimationview = findViewById(R.id.animation_view);
                    lottieanimationview.setVisibility(LottieAnimationView.GONE);
                    TextView textView = findViewById(R.id.textView3);
                    TextView pleas_wait = findViewById(R.id.textView5);
                    pleas_wait.setVisibility(View.INVISIBLE);
                    textView.setVisibility(View.GONE);


                    algoCalculation.resultImage = (ImageView) findViewById(R.id.resultView);
                    Utils.matToBitmap(algoCalculation.newMat, algoCalculation.result);
                    algoCalculation.resultImage.setImageBitmap(result);

                    ImageButton downloadButton = (ImageButton) findViewById(R.id.downloadButton);
                    downloadButton.setVisibility(View.VISIBLE);


                    ActivityCompat.requestPermissions(algoCalculation.this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
                    save = (ImageButton) findViewById(R.id.downloadButton);
                    ActivityCompat.requestPermissions(algoCalculation.this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
                    save.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {

                            imagesavetomyphonegallery();
                        }


                    });

                }
            });
        }

        public native int run(int[] points, long addrGray);


    }
}