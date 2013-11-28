package org.hipacc.example;

import android.graphics.Bitmap;

public class HIPAccFilters {
	static {
        System.loadLibrary("hipacc_filters");
	}
	
	public native int runBlur(int width, int height, Bitmap in, Bitmap out);
}
