package org.hipacc.example;

import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MenuItem.OnMenuItemClickListener;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

public class MainActivity extends Activity {

    private enum FilterType {
        Blur,
        Gaussian
    };

    private FilterType mType;
    private boolean mRunFilterscript;
    private AlertDialog mTypeChooser;

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

        mType = FilterType.Blur;
        mRunFilterscript = false;

        mNaive = new NaiveFilters(this);
        mHipacc = new HIPAccFilters();

        mBtnNaive = (Button) findViewById(R.id.buttonNaive);
        mBtnHipacc = (Button) findViewById(R.id.buttonHIPAcc);

        mBitmapIn = BitmapFactory.decodeResource(this.getResources(),
                R.drawable.lenna);
        mBitmapOut = Bitmap.createBitmap(mBitmapIn.getWidth(),
                mBitmapIn.getHeight(), mBitmapIn.getConfig());

        mBtnNaive.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                Integer time;

                switch (mType) {
                case Blur:
                    time = mRunFilterscript ?
                            mNaive.runFSBlur(mBitmapIn, mBitmapOut) :
                            mNaive.runRSBlur(mBitmapIn, mBitmapOut);
                    break;
                case Gaussian:
                    time = mRunFilterscript ?
                            mNaive.runFSGaussian(mBitmapIn, mBitmapOut) :
                            mNaive.runRSGaussian(mBitmapIn, mBitmapOut);
                    break;
                default:
                    time = -1;
                    break;
                }

                Toast.makeText(MainActivity.this,
                        "Time: " + time.toString() + "ms", Toast.LENGTH_LONG)
                        .show();
                updateOutput();
            }
        });

        mBtnHipacc.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                Integer time;

                switch (mType) {
                case Blur:
                    time = mRunFilterscript ?
                            mHipacc.runFSBlur(mBitmapIn, mBitmapOut) :
                            mHipacc.runRSBlur(mBitmapIn, mBitmapOut);
                    break;
                case Gaussian:
                    time = mRunFilterscript ?
                            mHipacc.runFSGaussian(mBitmapIn, mBitmapOut) :
                            mHipacc.runRSGaussian(mBitmapIn, mBitmapOut);
                    break;
                default:
                    time = -1;
                    break;
                }

                Toast.makeText(MainActivity.this,
                        "Time: " + time.toString() + "ms", Toast.LENGTH_LONG)
                        .show();
                updateOutput();
            }
        });

        AlertDialog.Builder builder =
                new AlertDialog.Builder(MainActivity.this);
        builder.setTitle("Choose a filter application")
               .setItems(R.array.filters_array,
                         new DialogInterface.OnClickListener() {
                   public void onClick(DialogInterface dialog, int which) {
                   mType = FilterType.values()[which];
               }
        });
        mTypeChooser = builder.create();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);

        final MenuItem filterType = menu.findItem(R.id.filtertype);
        filterType.setOnMenuItemClickListener(new OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                mTypeChooser.show();
                return true;
            }
        });

        final MenuItem filterscript = menu.findItem(R.id.filterscript);
        filterscript.setOnMenuItemClickListener(new OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (filterscript.isChecked()) {
                    mRunFilterscript = false;
                    filterscript.setChecked(false);
                } else {
                    mRunFilterscript = true;
                    filterscript.setChecked(true);
                }
                return false;
            }
        });

        return true;
    }

}
