package org.hipacc.example;

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

    private boolean init(Bitmap in, Bitmap out) {
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

        mInAllocation = Allocation.createFromBitmap(mRS, in,
                Allocation.MipmapControl.MIPMAP_NONE, Allocation.USAGE_SCRIPT);
        mOutAllocation = Allocation.createTyped(mRS, mInAllocation.getType());

        return true;
    }

    public int runRSBlur(Bitmap in, Bitmap out) {
        long time = -1;

        if (init(in, out)) {
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

        if (init(in, out)) {
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

        if (init(in, out)) {
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

        if (init(in, out)) {
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

        if (init(in, out)) {
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

        if (init(in, out)) {
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

        if (init(in, out)) {
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

        if (init(in, out)) {
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
}
