package org.hipacc.example;

import android.graphics.Bitmap;

public class HIPAccFilters {

    static {
        System.loadLibrary("hipacc_filters");
    }

    public native int runRSBlur(Bitmap in, Bitmap out);
    public native int runFSBlur(Bitmap in, Bitmap out);
    public native int runRSGaussian(Bitmap in, Bitmap out);
    public native int runFSGaussian(Bitmap in, Bitmap out);
    public native int runRSLaplace(Bitmap in, Bitmap out);
    public native int runFSLaplace(Bitmap in, Bitmap out);
    public native int runRSSobel(Bitmap in, Bitmap out);
    public native int runFSSobel(Bitmap in, Bitmap out);
}
