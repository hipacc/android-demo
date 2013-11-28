#include "hipacc_rs.hpp"

#include "ScriptC_blurfilterfilter.h"
//
// Copyright (c) 2013, University of Erlangen-Nuremberg
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <iostream>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/*************************************************************************
 * Main function                                                         *
 *************************************************************************/
#ifdef HIPACC
uchar4* pin;
uchar4* pout;
const int w = WIDTH;
const int h = HEIGHT;
int main(int argc, const char **argv) {
#else
int runBlur(int w, int h, uchar4 *pin, uchar4 *pout) {
#endif
    hipaccInitRenderScript(19);
    ScriptC_blurfilterfilter rsblurfilterfilterKernel = hipaccInitScript<ScriptC_blurfilterfilter>();
    
    const int width = w;
    const int height = h;
    const int size_x = SIZE_X;
    const int size_y = SIZE_Y;
    const int offset_x = size_x >> 1;
    const int offset_y = size_y >> 1;
    float timing = 0.0f;
    uchar4 *host_in = pin;
    uchar4 *host_out = pout;

    // only filter kernel sizes 3x3 and 5x5 implemented
    if (size_x != size_y && (size_x != 3 || size_x != 5)) {
        fprintf(stderr, "Wrong filter kernel size. Currently supported values: 3x3 and 5x5!\n");
        return 0;
    }

    // define Domain for blur filter
    
    const uchar domain[] = { 
        #if SIZE_X==3
        1, 1, 1,
        1, 1, 1,
        1, 1, 1
        #endif
        #if SIZE_X==5
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1
        #endif
    };
    

    // input and output image of width x height pixels
    HipaccImage in = hipaccCreateAllocation((uchar4*)NULL, width, height, 512);
    HipaccImage out = hipaccCreateAllocation((uchar4*)NULL, width, height, 512);
    
    HipaccAccessor acc(in, width - 2 * offset_x, height - 2 * offset_y, offset_x, offset_y);
    HipaccAccessor iter(out, width - 2 * offset_x, height - 2 * offset_y, offset_x, offset_y);

    hipaccWriteMemory(in, host_in);
    hipaccWriteMemory(out, host_out);

    hipacc_launch_info filter_info0(0, 0, iter, 1, 1);
    size_t work_size0[2];
    work_size0[0] = 128;
    work_size0[1] = 1;
    hipaccPrepareKernelLaunch(filter_info0, work_size0);

    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_in, sp<Allocation>(((Allocation *)acc.img.mem)));
    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_in_offset_x, acc.offset_x);
    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_in_offset_y, acc.offset_y);
    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_size_x, size_x);
    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_size_y, size_y);
    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_is_stride, iter.img.stride);
    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_is_width, iter.width);
    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_is_height, iter.height);
    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_is_offset_x, iter.offset_x);
    hipaccSetScriptArg(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::set_is_offset_y, iter.offset_y);
    
    hipaccLaunchScriptKernel(&rsblurfilterfilterKernel, &ScriptC_blurfilterfilter::forEach_rsBlurFilterfilterKernel, out, work_size0);
    timing = hipaccGetLastKernelTiming();

    hipaccReadMemory(host_out, out);

    return timing;
}

