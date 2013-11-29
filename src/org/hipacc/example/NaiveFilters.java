package org.hipacc.example;

import android.content.Context;
import android.graphics.Bitmap;
import android.renderscript.Allocation;
import android.renderscript.RenderScript;

public class NaiveFilters {
    private Context mCtx;
    private RenderScript mRS;
    
    public NaiveFilters(Context ctx) {
        mCtx = ctx;
        mRS = RenderScript.create(ctx);
    }

    public int runBlur(Bitmap in, Bitmap out) {
        Allocation InAllocation = Allocation.createFromBitmap(mRS, in,
                Allocation.MipmapControl.MIPMAP_NONE, Allocation.USAGE_SCRIPT);
        Allocation OutAllocation = Allocation.createTyped(mRS,
                InAllocation.getType());

        ScriptC_blur Script = new ScriptC_blur(mRS, mCtx.getResources(),
                R.raw.blur);
        Script.set_in(InAllocation);
        Script.set_width(in.getWidth());
        Script.set_height(in.getHeight());

        mRS.finish();
        long time = System.nanoTime();
        Script.forEach_root(OutAllocation);

        mRS.finish();
        time = System.nanoTime() - time;

        OutAllocation.copyTo(out);

        return (int)(time/1000000);
    }
}
