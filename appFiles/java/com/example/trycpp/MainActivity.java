package com.example.trycpp;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.drawable.RoundedBitmapDrawable;
import androidx.core.graphics.drawable.RoundedBitmapDrawableFactory;

public class MainActivity extends AppCompatActivity  implements View.OnClickListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("opencv_java3");

        System.loadLibrary("native-lib");
    }

    private static final int RESUALT_LOAD_IMAGE = 1;

    ImageView imageToUpload;
    TextView uploadAnother;
    ImageButton buttonToUpload;

    static int counter = 0;
    static Bitmap[] imageMats = new Bitmap[4];
    static Bitmap[] imageBitMaps;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setTheme(R.style.AppTheme);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imageToUpload = (ImageView) findViewById(R.id.imageToUpload);
        uploadAnother = (TextView) findViewById(R.id.textView4);
        buttonToUpload = (ImageButton) findViewById(R.id.bottonUpload);
        buttonToUpload.setVisibility(View.INVISIBLE);
        uploadAnother.setVisibility(View.INVISIBLE);

        imageToUpload.setOnClickListener(this);
        buttonToUpload.setOnClickListener(this);

    }


    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.imageToUpload:
                Intent galleryIntent = new Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI);

                startActivityForResult(galleryIntent, RESUALT_LOAD_IMAGE);

                break;
            case R.id.bottonUpload:
                nextActivity(v);
                break;
        }
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == RESUALT_LOAD_IMAGE && resultCode == RESULT_OK && data != null && data.getData() != null) {
            Uri selectedImage = data.getData();

            BitmapFactory.Options bmpFactoryOptions = new BitmapFactory.Options();
            bmpFactoryOptions.inPreferredConfig = Bitmap.Config.ARGB_8888;
            try{
                Bitmap bmp = MediaStore.Images.Media.getBitmap(this.getContentResolver(), selectedImage);
                imageMats[counter] = bmp;
                counter = (counter + 1) % 4;

                RoundedBitmapDrawable round = RoundedBitmapDrawableFactory.create(getResources(), bmp);
                round.setCornerRadius(200);
                imageToUpload.setImageDrawable(round);
                if (counter>0){
                    uploadAnother.setVisibility(View.VISIBLE);

                }
                if (counter > 1){
                    buttonToUpload.setVisibility(View.VISIBLE);
                }
            }catch (Exception e){
                return;
            }

        }
    }

    public void nextActivity(View v){
        imageBitMaps = new Bitmap[counter];
        for (int i = 0; i < counter; ++i)
        {
            imageBitMaps[i] = imageMats[i];
        }
        Intent i = new Intent(this, SecondActivity.class);
        startActivity(i);
    }

}