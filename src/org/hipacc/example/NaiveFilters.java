package org.hipacc.example;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;

import android.content.Context;
import android.graphics.Bitmap;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.Type;

public class NaiveFilters {

    private Context mCtx;
    private RenderScript mRS;
    private Allocation mInAllocation;
    private Allocation mOutAllocation;

    public NaiveFilters(Context ctx) {
        mCtx = ctx;
        mRS = RenderScript.create(ctx);
    }

    private boolean init(Bitmap in, Bitmap out, boolean alloc) {
        // Checking dimension
        if (in.getWidth() != out.getWidth() ||
            in.getHeight() != out.getHeight()) {
            return false;
        }

        // Checking format
        if (in.getConfig() != Bitmap.Config.ARGB_8888 ||
            out.getConfig() != Bitmap.Config.ARGB_8888) {
            return false;
        }

        if (alloc) {
            mInAllocation = Allocation.createFromBitmap(mRS, in,
                    Allocation.MipmapControl.MIPMAP_NONE,
                    Allocation.USAGE_SCRIPT);
            mOutAllocation = Allocation.createTyped(mRS,
                    mInAllocation.getType());
        }

        return true;
    }

    public int runRSBlur(Bitmap in, Bitmap out) {
        long time = -1;

        if (init(in, out, true)) {
            ScriptC_rsblur script = new ScriptC_rsblur(mRS, mCtx.getResources(),
                    R.raw.rsblur);
            script.set_in(mInAllocation);
            script.set_width(in.getWidth());
            script.set_height(in.getHeight());

            mRS.finish();
            time = System.nanoTime();
            script.forEach_root(mOutAllocation);

            mRS.finish();
            time = (System.nanoTime() - time) / 1000000;

            mOutAllocation.copyTo(out);
            mRS.finish();
        }

        return (int)time;
    }
    
    public int runFSBlur(Bitmap in, Bitmap out) {
        long time = -1;

        if (init(in, out, true)) {
            ScriptC_fsblur script = new ScriptC_fsblur(mRS, mCtx.getResources(),
                    R.raw.fsblur);
            script.set_in(mInAllocation);
            script.set_width(in.getWidth());
            script.set_height(in.getHeight());

            mRS.finish();
            time = System.nanoTime();
            script.forEach_root(mOutAllocation);

            mRS.finish();
            time = (System.nanoTime() - time) / 1000000;

            mOutAllocation.copyTo(out);
            mRS.finish();
        }

        return (int)time;
    }

    public int runRSGaussian(Bitmap in, Bitmap out) {
        long time = -1;

        if (init(in, out, true)) {
            Type.Builder maskType = new Type.Builder(mRS, Element.F32(mRS));
            maskType.setX(5);
            maskType.setY(5);

            Allocation maskAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskAllocation.copyFrom(new float[]{
                    0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f,
                    0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
                    0.026151f, 0.090339f, 0.136565f, 0.090339f, 0.026151f,
                    0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
                    0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f
            });

            ScriptC_rsgaussian script = new ScriptC_rsgaussian(mRS,
                    mCtx.getResources(), R.raw.rsgaussian);
            script.set_in(mInAllocation);
            script.set_mask(maskAllocation);
            script.set_width(in.getWidth());
            script.set_height(in.getHeight());

            mRS.finish();
            time = System.nanoTime();
            script.forEach_root(mOutAllocation);

            mRS.finish();
            time = (System.nanoTime() - time) / 1000000;

            mOutAllocation.copyTo(out);
            mRS.finish();
        }

        return (int)time;
    }
    
    public int runFSGaussian(Bitmap in, Bitmap out) {
        long time = -1;

        if (init(in, out, true)) {
            Type.Builder maskType = new Type.Builder(mRS, Element.F32(mRS));
            maskType.setX(5);
            maskType.setY(5);

            Allocation maskAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskAllocation.copyFrom(new float[]{
                    0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f,
                    0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
                    0.026151f, 0.090339f, 0.136565f, 0.090339f, 0.026151f,
                    0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
                    0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f
            });

            ScriptC_fsgaussian script = new ScriptC_fsgaussian(mRS,
                    mCtx.getResources(), R.raw.fsgaussian);
            script.set_in(mInAllocation);
            script.set_mask(maskAllocation);
            script.set_width(in.getWidth());
            script.set_height(in.getHeight());

            mRS.finish();
            time = System.nanoTime();
            script.forEach_root(mOutAllocation);

            mRS.finish();
            time = (System.nanoTime() - time) / 1000000;

            mOutAllocation.copyTo(out);
            mRS.finish();
        }

        return (int)time;
    }

    public int runRSLaplace(Bitmap in, Bitmap out) {
        long time = -1;

        if (init(in, out, true)) {
            Type.Builder maskType = new Type.Builder(mRS, Element.I32(mRS));
            maskType.setX(5);
            maskType.setY(5);

            Allocation maskAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskAllocation.copyFrom(new int[]{
                    1,   1,   1,   1,   1,
                    1,   1,   1,   1,   1,
                    1,   1, -24,   1,   1,
                    1,   1,   1,   1,   1,
                    1,   1,   1,   1,   1
            });

            ScriptC_rslaplace script = new ScriptC_rslaplace(mRS,
                    mCtx.getResources(), R.raw.rslaplace);
            script.set_in(mInAllocation);
            script.set_mask(maskAllocation);
            script.set_width(in.getWidth());
            script.set_height(in.getHeight());

            mRS.finish();
            time = System.nanoTime();
            script.forEach_root(mOutAllocation);

            mRS.finish();
            time = (System.nanoTime() - time) / 1000000;

            mOutAllocation.copyTo(out);
            mRS.finish();
        }

        return (int)time;
    }

    public int runFSLaplace(Bitmap in, Bitmap out) {
        long time = -1;

        if (init(in, out, true)) {
            Type.Builder maskType = new Type.Builder(mRS, Element.I32(mRS));
            maskType.setX(5);
            maskType.setY(5);

            Allocation maskAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskAllocation.copyFrom(new int[]{
                    1,   1,   1,   1,   1,
                    1,   1,   1,   1,   1,
                    1,   1, -24,   1,   1,
                    1,   1,   1,   1,   1,
                    1,   1,   1,   1,   1
            });

            ScriptC_fslaplace script = new ScriptC_fslaplace(mRS,
                    mCtx.getResources(), R.raw.fslaplace);
            script.set_in(mInAllocation);
            script.set_mask(maskAllocation);
            script.set_width(in.getWidth());
            script.set_height(in.getHeight());

            mRS.finish();
            time = System.nanoTime();
            script.forEach_root(mOutAllocation);

            mRS.finish();
            time = (System.nanoTime() - time) / 1000000;

            mOutAllocation.copyTo(out);
            mRS.finish();
        }

        return (int)time;
    }

    public int runRSSobel(Bitmap in, Bitmap out) {
        long time = -1;
        long timeKernel;

        if (init(in, out, true)) {
            Type.Builder maskType = new Type.Builder(mRS, Element.I32(mRS));
            maskType.setX(5);
            maskType.setY(5);

            Type.Builder derivType = new Type.Builder(mRS, Element.I16_4(mRS));
            derivType.setX(in.getWidth());
            derivType.setY(in.getHeight());

            Allocation maskXAllocation =
                    Allocation.createTyped(mRS, maskType.create());
            Allocation maskYAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskXAllocation.copyFrom(new int[]{
                    -1, -2,  0,  2,  1,
                    -4, -8,  0,  8,  4,
                    -6, -12, 0,  12, 6,
                    -4, -8,  0,  8,  4,
                    -1, -2,  0,  2,  1
            });

            maskYAllocation.copyFrom(new int[]{
                    -1, -4, -6,  -4, -1,
                    -2, -8, -12, -8, -2,
                     0,  0,  0,   0,  0,
                     2,  8,  12,  8,  2,
                     1,  4,  6,   4,  1
            });

            Allocation derivXAllocation =
                    Allocation.createTyped(mRS, derivType.create());
            Allocation derivYAllocation =
                    Allocation.createTyped(mRS, derivType.create());

            ScriptC_rssobelderiv deriv = new ScriptC_rssobelderiv(mRS,
                    mCtx.getResources(), R.raw.rssobelderiv);
            deriv.set_in(mInAllocation);
            deriv.set_width(in.getWidth());
            deriv.set_height(in.getHeight());

            ScriptC_rssobelcombine combine = new ScriptC_rssobelcombine(mRS,
                    mCtx.getResources(), R.raw.rssobelcombine);
            combine.set_in1(derivXAllocation);
            combine.set_in2(derivYAllocation);

            // First run: Derivate X
            deriv.set_mask(maskXAllocation);
            
            mRS.finish();
            timeKernel = System.nanoTime();
            deriv.forEach_root(derivXAllocation);

            mRS.finish();
            time = (System.nanoTime() - timeKernel) / 1000000;

            // Second run: Derivate Y
            deriv.set_mask(maskYAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            deriv.forEach_root(derivYAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Third run: Combine
            mRS.finish();
            timeKernel = System.nanoTime();
            combine.forEach_root(mOutAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            mOutAllocation.copyTo(out);
            mRS.finish();
        }

        return (int)time;
    }

    public int runFSSobel(Bitmap in, Bitmap out) {
        long time = -1;
        long timeKernel;

        if (init(in, out, true)) {
            Type.Builder maskType = new Type.Builder(mRS, Element.I32(mRS));
            maskType.setX(5);
            maskType.setY(5);

            Type.Builder derivType = new Type.Builder(mRS, Element.I16_4(mRS));
            derivType.setX(in.getWidth());
            derivType.setY(in.getHeight());

            Allocation maskXAllocation =
                    Allocation.createTyped(mRS, maskType.create());
            Allocation maskYAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskXAllocation.copyFrom(new int[]{
                    -1, -2,  0,  2,  1,
                    -4, -8,  0,  8,  4,
                    -6, -12, 0,  12, 6,
                    -4, -8,  0,  8,  4,
                    -1, -2,  0,  2,  1
            });

            maskYAllocation.copyFrom(new int[]{
                    -1, -4, -6,  -4, -1,
                    -2, -8, -12, -8, -2,
                     0,  0,  0,   0,  0,
                     2,  8,  12,  8,  2,
                     1,  4,  6,   4,  1
            });

            Allocation derivXAllocation =
                    Allocation.createTyped(mRS, derivType.create());
            Allocation derivYAllocation =
                    Allocation.createTyped(mRS, derivType.create());

            ScriptC_fssobelderiv deriv = new ScriptC_fssobelderiv(mRS,
                    mCtx.getResources(), R.raw.fssobelderiv);
            deriv.set_in(mInAllocation);
            deriv.set_width(in.getWidth());
            deriv.set_height(in.getHeight());

            ScriptC_fssobelcombine combine = new ScriptC_fssobelcombine(mRS,
                    mCtx.getResources(), R.raw.fssobelcombine);
            combine.set_in1(derivXAllocation);
            combine.set_in2(derivYAllocation);

            // First run: Derivate X
            deriv.set_mask(maskXAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            deriv.forEach_root(derivXAllocation);

            mRS.finish();
            time = (System.nanoTime() - timeKernel) / 1000000;

            // Second run: Derivate Y
            deriv.set_mask(maskYAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            deriv.forEach_root(derivYAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Third run: Combine
            mRS.finish();
            timeKernel = System.nanoTime();
            combine.forEach_root(mOutAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            mOutAllocation.copyTo(out);
            mRS.finish();
        }

        return (int)time;
    }
    
    private float ucharToFloat(byte in) {
        return in < 0 ? (float)in + 256 : in;
    }

    public int runRSHarris(Bitmap in, Bitmap out) {
        long time = -1;
        long timeKernel;

        if (init(in, out, false)) {
            float k = 0.04f;
            float threshold = 20000.0f;

            ByteBuffer bufRGB = ByteBuffer.allocate(in.getWidth() * in.getHeight() * 4);
            FloatBuffer bufGray = FloatBuffer.allocate(in.getWidth() * in.getHeight());

            in.copyPixelsToBuffer(bufRGB);
            
            bufRGB.rewind();

            for (int i = 0; i < in.getHeight() * in.getWidth(); ++i) {
                bufGray.put(i, .2126f * ucharToFloat(bufRGB.get(4*i)) +
                               .7152f * ucharToFloat(bufRGB.get(4*i + 1)) +
                               .0722f * ucharToFloat(bufRGB.get(4*i + 2)));
            }

            Type.Builder derivType = new Type.Builder(mRS, Element.F32(mRS));
            derivType.setX(in.getWidth());
            derivType.setY(in.getHeight());

            Type.Builder maskType = new Type.Builder(mRS, Element.F32(mRS));
            maskType.setX(3);
            maskType.setY(3);

            Allocation grayAllocation =
                    Allocation.createTyped(mRS, derivType.create());

            Allocation derivXAllocation =
                    Allocation.createTyped(mRS, derivType.create());
            Allocation derivYAllocation =
                    Allocation.createTyped(mRS, derivType.create());
            Allocation derivXYAllocation =
                    Allocation.createTyped(mRS, derivType.create());
            Allocation derivTmpAllocation =
                    Allocation.createTyped(mRS, derivType.create());

            Allocation maskDerivXAllocation =
                    Allocation.createTyped(mRS, maskType.create());
            Allocation maskDerivYAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskDerivXAllocation.copyFrom(new float[]{
                    -0.166666667f,          0.0f,  0.166666667f,
                    -0.166666667f,          0.0f,  0.166666667f,
                    -0.166666667f,          0.0f,  0.166666667f
            });
            
            maskDerivYAllocation.copyFrom(new float[]{
                    -0.166666667f, -0.166666667f, -0.166666667f,
                             0.0f,          0.0f,          0.0f,
                     0.166666667f,  0.166666667f,  0.166666667f
            });

            maskType.setX(5);
            maskType.setY(5);

            Allocation maskGaussAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskGaussAllocation.copyFrom(new float[]{
                    0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f,
                    0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
                    0.026151f, 0.090339f, 0.136565f, 0.090339f, 0.026151f,
                    0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
                    0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f
            });

            grayAllocation.copy1DRangeFrom(0, in.getWidth() * in.getHeight(),
                    bufGray.array());

            ScriptC_rsharrisderiv1d deriv1d = new ScriptC_rsharrisderiv1d(mRS,
                    mCtx.getResources(), R.raw.rsharrisderiv1d);
            deriv1d.set_in(grayAllocation);
            deriv1d.set_width(in.getWidth());
            deriv1d.set_height(in.getHeight());

            ScriptC_rsharrisderiv2d deriv2d = new ScriptC_rsharrisderiv2d(mRS,
                    mCtx.getResources(), R.raw.rsharrisderiv2d);
            deriv2d.set_in(grayAllocation);
            deriv2d.set_width(in.getWidth());
            deriv2d.set_height(in.getHeight());
            deriv2d.set_mask1(maskDerivXAllocation);
            deriv2d.set_mask2(maskDerivYAllocation);

            ScriptC_rsharrisgaussian gaussian = new ScriptC_rsharrisgaussian(mRS,
                    mCtx.getResources(), R.raw.rsharrisgaussian);
            gaussian.set_width(in.getWidth());
            gaussian.set_height(in.getHeight());
            gaussian.set_mask(maskGaussAllocation);

            ScriptC_rsharris harris = new ScriptC_rsharris(mRS,
                    mCtx.getResources(), R.raw.rsharris);
            harris.set_dx(derivTmpAllocation);
            harris.set_dy(derivXAllocation);
            harris.set_dxy(derivYAllocation);
            harris.set_k(k);

            // First run
            deriv1d.set_mask(maskDerivXAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            deriv1d.forEach_root(derivXAllocation);

            mRS.finish();
            time = (System.nanoTime() - timeKernel) / 1000000;
            
            // Second run
            deriv1d.set_mask(maskDerivYAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            deriv1d.forEach_root(derivYAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Third run
            mRS.finish();
            timeKernel = System.nanoTime();
            deriv2d.forEach_root(derivXYAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Forth run
            gaussian.set_in(derivXAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            gaussian.forEach_root(derivTmpAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Fifth run
            gaussian.set_in(derivYAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            gaussian.forEach_root(derivXAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Sixth run
            gaussian.set_in(derivXYAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            gaussian.forEach_root(derivYAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Seventh run
            mRS.finish();
            timeKernel = System.nanoTime();
            harris.forEach_root(grayAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            grayAllocation.copyTo(bufGray.array());

            // Draw output
            for (int x = 0; x < in.getWidth(); ++x) {
                for (int y = 0; y < in.getHeight(); y++) {
                    int pos = y*in.getWidth() + x;
                    if (bufGray.get(pos) > threshold) {
                        for (int i = -10; i <= 10; ++i) {
                            if (x+i >= 0 && x+i < in.getWidth()) {
                                bufRGB.put((pos + i)*4, (byte)255);
                                bufRGB.put((pos + i)*4 + 1, (byte)255);
                                bufRGB.put((pos + i)*4 + 2, (byte)255);
                            }
                        }
                        for (int i = -10; i <= 10; ++i) {
                            if (y+i >= 0 && y+i < in.getHeight()) {
                                bufRGB.put((pos + i*in.getWidth())*4, (byte)255);
                                bufRGB.put((pos + i*in.getWidth())*4 + 1, (byte)255);
                                bufRGB.put((pos + i*in.getWidth())*4 + 2, (byte)255);
                            }
                        }
                    }
                }
            }

            out.copyPixelsFromBuffer(bufRGB);
        }

        return (int)time;
    }
    
    public int runFSHarris(Bitmap in, Bitmap out) {
        long time = -1;
        long timeKernel;

        if (init(in, out, false)) {
            float k = 0.04f;
            float threshold = 20000.0f;

            ByteBuffer bufRGB = ByteBuffer.allocate(in.getWidth() * in.getHeight() * 4);
            FloatBuffer bufGray = FloatBuffer.allocate(in.getWidth() * in.getHeight());

            in.copyPixelsToBuffer(bufRGB);
            
            bufRGB.rewind();

            for (int i = 0; i < in.getHeight() * in.getWidth(); ++i) {
                bufGray.put(i, .2126f * ucharToFloat(bufRGB.get(4*i)) +
                               .7152f * ucharToFloat(bufRGB.get(4*i + 1)) +
                               .0722f * ucharToFloat(bufRGB.get(4*i + 2)));
            }

            Type.Builder derivType = new Type.Builder(mRS, Element.F32(mRS));
            derivType.setX(in.getWidth());
            derivType.setY(in.getHeight());

            Type.Builder maskType = new Type.Builder(mRS, Element.F32(mRS));
            maskType.setX(3);
            maskType.setY(3);

            Allocation grayAllocation =
                    Allocation.createTyped(mRS, derivType.create());

            Allocation derivXAllocation =
                    Allocation.createTyped(mRS, derivType.create());
            Allocation derivYAllocation =
                    Allocation.createTyped(mRS, derivType.create());
            Allocation derivXYAllocation =
                    Allocation.createTyped(mRS, derivType.create());
            Allocation derivTmpAllocation =
                    Allocation.createTyped(mRS, derivType.create());

            Allocation maskDerivXAllocation =
                    Allocation.createTyped(mRS, maskType.create());
            Allocation maskDerivYAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskDerivXAllocation.copyFrom(new float[]{
                    -0.166666667f,          0.0f,  0.166666667f,
                    -0.166666667f,          0.0f,  0.166666667f,
                    -0.166666667f,          0.0f,  0.166666667f
            });
            
            maskDerivYAllocation.copyFrom(new float[]{
                    -0.166666667f, -0.166666667f, -0.166666667f,
                             0.0f,          0.0f,          0.0f,
                     0.166666667f,  0.166666667f,  0.166666667f
            });

            maskType.setX(5);
            maskType.setY(5);

            Allocation maskGaussAllocation =
                    Allocation.createTyped(mRS, maskType.create());

            maskGaussAllocation.copyFrom(new float[]{
                    0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f,
                    0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
                    0.026151f, 0.090339f, 0.136565f, 0.090339f, 0.026151f,
                    0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
                    0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f
            });

            grayAllocation.copy1DRangeFrom(0, in.getWidth() * in.getHeight(),
                    bufGray.array());

            ScriptC_fsharrisderiv1d deriv1d = new ScriptC_fsharrisderiv1d(mRS,
                    mCtx.getResources(), R.raw.fsharrisderiv1d);
            deriv1d.set_in(grayAllocation);
            deriv1d.set_width(in.getWidth());
            deriv1d.set_height(in.getHeight());

            ScriptC_fsharrisderiv2d deriv2d = new ScriptC_fsharrisderiv2d(mRS,
                    mCtx.getResources(), R.raw.fsharrisderiv2d);
            deriv2d.set_in(grayAllocation);
            deriv2d.set_width(in.getWidth());
            deriv2d.set_height(in.getHeight());
            deriv2d.set_mask1(maskDerivXAllocation);
            deriv2d.set_mask2(maskDerivYAllocation);

            ScriptC_fsharrisgaussian gaussian = new ScriptC_fsharrisgaussian(mRS,
                    mCtx.getResources(), R.raw.fsharrisgaussian);
            gaussian.set_width(in.getWidth());
            gaussian.set_height(in.getHeight());
            gaussian.set_mask(maskGaussAllocation);

            ScriptC_fsharris harris = new ScriptC_fsharris(mRS,
                    mCtx.getResources(), R.raw.fsharris);
            harris.set_dx(derivTmpAllocation);
            harris.set_dy(derivXAllocation);
            harris.set_dxy(derivYAllocation);
            harris.set_k(k);

            // First run
            deriv1d.set_mask(maskDerivXAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            deriv1d.forEach_root(derivXAllocation);

            mRS.finish();
            time = (System.nanoTime() - timeKernel) / 1000000;
            
            // Second run
            deriv1d.set_mask(maskDerivYAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            deriv1d.forEach_root(derivYAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Third run
            mRS.finish();
            timeKernel = System.nanoTime();
            deriv2d.forEach_root(derivXYAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Forth run
            gaussian.set_in(derivXAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            gaussian.forEach_root(derivTmpAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Fifth run
            gaussian.set_in(derivYAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            gaussian.forEach_root(derivXAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Sixth run
            gaussian.set_in(derivXYAllocation);

            mRS.finish();
            timeKernel = System.nanoTime();
            gaussian.forEach_root(derivYAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            // Seventh run
            mRS.finish();
            timeKernel = System.nanoTime();
            harris.forEach_root(grayAllocation);

            mRS.finish();
            time += (System.nanoTime() - timeKernel) / 1000000;

            grayAllocation.copyTo(bufGray.array());

            // Draw output
            for (int x = 0; x < in.getWidth(); ++x) {
                for (int y = 0; y < in.getHeight(); y++) {
                    int pos = y*in.getWidth() + x;
                    if (bufGray.get(pos) > threshold) {
                        for (int i = -10; i <= 10; ++i) {
                            if (x+i >= 0 && x+i < in.getWidth()) {
                                bufRGB.put((pos + i)*4, (byte)255);
                                bufRGB.put((pos + i)*4 + 1, (byte)255);
                                bufRGB.put((pos + i)*4 + 2, (byte)255);
                            }
                        }
                        for (int i = -10; i <= 10; ++i) {
                            if (y+i >= 0 && y+i < in.getHeight()) {
                                bufRGB.put((pos + i*in.getWidth())*4, (byte)255);
                                bufRGB.put((pos + i*in.getWidth())*4 + 1, (byte)255);
                                bufRGB.put((pos + i*in.getWidth())*4 + 2, (byte)255);
                            }
                        }
                    }
                }
            }

            out.copyPixelsFromBuffer(bufRGB);
        }

        return (int)time;
    }
}
