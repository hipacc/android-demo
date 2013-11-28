package org.hipacc.example;

import android.os.Bundle;
import android.renderscript.Allocation;
import android.renderscript.RenderScript;
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
        
        mHipacc = new HIPAccFilters();
        
        mBtnNaive = (Button)findViewById(R.id.buttonNaive);
        mBtnHipacc = (Button)findViewById(R.id.buttonHIPAcc);
        
        mBitmapIn = BitmapFactory.decodeResource(this.getResources(), R.drawable.lenna);
        mBitmapOut = Bitmap.createBitmap(mBitmapIn.getWidth(), mBitmapIn.getHeight(), mBitmapIn.getConfig());
        
        mBtnNaive.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				RenderScript RS = RenderScript.create(MainActivity.this);

		        Allocation InAllocation = Allocation.createFromBitmap(RS, mBitmapIn,
				                            Allocation.MipmapControl.MIPMAP_NONE,
				                            Allocation.USAGE_SCRIPT);
		        Allocation OutAllocation = Allocation.createTyped(RS, InAllocation.getType());

		        ScriptC_blur Script = new ScriptC_blur(RS, getResources(), R.raw.blur);
		        Script.set_in(InAllocation);
		        Script.set_width(mBitmapIn.getWidth());
		        Script.set_height(mBitmapIn.getHeight());

		        RS.finish();
		        long time = System.nanoTime();
		        Script.forEach_root(OutAllocation);
		        RS.finish();
		        
		        time = System.nanoTime() - time;
		        Toast.makeText(MainActivity.this, Long.toString(time/1000000), Toast.LENGTH_LONG).show();

		        OutAllocation.copyTo(mBitmapOut);
		        
		        updateOutput();
			}
		});
        
        mBtnHipacc.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				Integer time = mHipacc.runBlur(mBitmapIn.getWidth(), mBitmapIn.getHeight(), mBitmapIn, mBitmapOut);
				
				Toast.makeText(MainActivity.this, time.toString(), Toast.LENGTH_LONG).show();
				
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
