package org.hipacc.example;

import android.graphics.Bitmap;

public class HIPAccFilters {
    static {
        System.loadLibrary("hipacc_filters");
    }

    public native int runBlur(Bitmap in, Bitmap out);
    public native int runGaussian(Bitmap in, Bitmap out);
}
