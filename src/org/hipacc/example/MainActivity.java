package org.hipacc.example;

import android.os.Bundle;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

public class MainActivity extends Activity {

    private NaiveFilters mNaive;
    private HIPAccFilters mHipacc;
    private Button mBtnNaive;
    private Button mBtnHipacc;
    private Bitmap mBitmapIn;
    private Bitmap mBitmapOut;

    private void updateOutput() {
        ImageView imgView = (ImageView)findViewById(R.id.imageView2);
        imgView.setImageBitmap(mBitmapOut);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mNaive = new NaiveFilters(this);
        mHipacc = new HIPAccFilters();

        mBtnNaive = (Button) findViewById(R.id.buttonNaive);
        mBtnHipacc = (Button) findViewById(R.id.buttonHIPAcc);

        mBitmapIn = BitmapFactory.decodeResource(this.getResources(),
                R.drawable.landscape);
        mBitmapOut = Bitmap.createBitmap(mBitmapIn.getWidth(),
                mBitmapIn.getHeight(), mBitmapIn.getConfig());

        mBtnNaive.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                Integer time = mNaive.runGaussian(mBitmapIn, mBitmapOut);
                Toast.makeText(MainActivity.this,
                        "Time: " + time.toString() + "ms", Toast.LENGTH_LONG)
                        .show();
                updateOutput();
            }
        });

        mBtnHipacc.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                Integer time = mHipacc.runGaussian(mBitmapIn, mBitmapOut);
                Toast.makeText(MainActivity.this,
                        "Time: " + time.toString() + "ms", Toast.LENGTH_LONG)
                        .show();
                updateOutput();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

}
