//
// Copyright (c) 2012, University of Erlangen-Nuremberg
// Copyright (c) 2012, Siemens AG
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
#include <vector>

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "hipacc.hpp"

using namespace hipacc;
using namespace hipacc::math;

class Laplace : public Kernel<uchar4> {
    private:
        Accessor<uchar4> &Input;
        Mask<int> &cMask;
        const int size;

    public:
        Laplace(IterationSpace<uchar4> &IS, Accessor<uchar4>
                &Input, Mask<int> &cMask, const int size) :
            Kernel(IS),
            Input(Input),
            cMask(cMask),
            size(size)
        { addAccessor(&Input); }

        void kernel() {
            int4 sum = convolve(cMask, HipaccSUM, [&] () -> int4 {
                    return cMask() * convert_int4(Input(cMask));
                    });
            sum = min(sum, 255);
            sum = max(sum, 0);
            sum.w = 255;
            output() = convert_uchar4(sum);
        }
};

// Main function
#ifdef HIPACC
int w = 1024;
int h = 1024;
uchar4 *in;
uchar4 *out;
int main(int argc, const char **argv) {
#else
#ifndef FILTERSCRIPT
int runRSLaplace(int w, int h, uchar4 *in, uchar4 *out) {
#else
int runFSLaplace(int w, int h, uchar4 *in, uchar4 *out) {
#endif
#endif
    const int width = w;
    const int height = h;
    const int size_x = SIZE_X;
    const int size_y = SIZE_Y;
    const int offset_x = size_x >> 1;
    const int offset_y = size_y >> 1;
    float timing = 0.0f;
    uchar4 *host_in = in;
    uchar4 *host_out = out;

    // only filter kernel sizes 3x3 and 5x5 supported
    if (size_x != size_y || !(size_x == 3 || size_x == 5)) {
        fprintf(stderr, "Wrong filter kernel size. Currently supported values: 3x3 and 5x5!\n");
        return -1;
    }

    // convolution filter mask
    const int mask[] = {
#if SIZE_X==1
        0,  1,  0,
        1, -4,  1,
        0,  1,  0
#endif
#if SIZE_X==3
        2,  0,  2,
        0, -8,  0,
        2,  0,  2
#endif
#if SIZE_X==5
        1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,
        1,   1, -24,   1,   1,
        1,   1,   1,   1,   1,
        1,   1,   1,   1,   1
#endif
    };

    // input and output image of width x height pixels
    Image<uchar4> IN(width, height);
    Image<uchar4> OUT(width, height);

    // filter mask
    Mask<int> M(size_x, size_y);
    M = mask;

    IterationSpace<uchar4> IsOut(OUT);

    IN = host_in;
    OUT = host_out;

    BoundaryCondition<uchar4> BcInClamp(IN, size_x, BOUNDARY_CLAMP);
    Accessor<uchar4> AccInClamp(BcInClamp);
    Laplace filter(IsOut, AccInClamp, M, size_x);

    filter.execute();
    timing = hipaccGetLastKernelTiming();

    // get results
    host_out = OUT.getData();

    return timing;
}

