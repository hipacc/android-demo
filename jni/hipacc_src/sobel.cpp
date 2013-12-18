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
#include <vector>

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "hipacc.hpp"

#define NO_SEP

using namespace hipacc;
using namespace hipacc::math;

#if SIZE_X==3
#  define MAX_DERIV (255*4)
#elif SIZE_X==5
#  define MAX_DERIV (255*48)
#elif SIZE_X==7
#  define MAX_DERIV (255*640)
#endif

#ifdef NO_SEP
class Sobel : public Kernel<uchar4> {
    private:
        Accessor<uchar4> &Input;
        Domain &cDom;
        Mask<int> &cMaskX;
        Mask<int> &cMaskY;
        const int size;

    public:
        Sobel(IterationSpace<uchar4> &IS, Accessor<uchar4>
                &Input, Domain &cDom, Mask<int> &cMaskX, Mask<int> &cMaskY, const int size) :
            Kernel(IS),
            Input(Input),
            cDom(cDom),
            cMaskX(cMaskX),
            cMaskY(cMaskY),
            size(size)
        { addAccessor(&Input); }

        void kernel() {
            float4 sumX = 0.0f;
            float4 sumY = 0.0f;
            iterate(cDom, [&] () {
                float4 in = convert_float4(Input(cDom));
                sumX += (float)cMaskX(cDom) * in;
                sumY += (float)cMaskY(cDom) * in;
            });

            int4 out = convert_int4(sqrtf((sumX * sumX) + (sumY * sumY)));
            out = min(out, 255);
            out = max(out, 0);
            out.w = 255;
            output() = convert_uchar4(out);
        }
};
#else
class SobelRow : public Kernel<short4> {
    private:
        Accessor<uchar4> &Input;
        Mask<int> &cMask;
        const int size;

    public:
        SobelRow(IterationSpace<short4> &IS, Accessor<uchar4>
                &Input, Mask<int> &cMask, const int size):
            Kernel(IS),
            Input(Input),
            cMask(cMask),
            size(size)
        { addAccessor(&Input); }

        void kernel() {
            int4 sum = convolve(cMask, HipaccSUM, [&] () -> int4 {
                    return cMask() * convert_int4(Input(cMask));
                    });
            output() = convert_short4(sum);
        }
};
class SobelColumn : public Kernel<short4> {
    private:
        Accessor<short4> &Input;
        Mask<int> &cMask;
        const int size;

    public:
        SobelColumn(IterationSpace<short4> &IS, Accessor<short4>
                &Input, Mask<int> &cMask, const int size):
            Kernel(IS),
            Input(Input),
            cMask(cMask),
            size(size)
        { addAccessor(&Input); }

        void kernel() {
            int4 sum = convolve(cMask, HipaccSUM, [&] () -> int4 {
                    return cMask() * convert_int4(Input(cMask));
                    });
            output() = convert_short4(sum);
        }
};
class SobelCombine : public Kernel<uchar4> {
    private:
        Accessor<short4> &Input1;
        Accessor<short4> &Input2;

    public:
        SobelCombine(IterationSpace<uchar4> &IS, Accessor<short4>
                &Input1, Accessor<short4> &Input2):
            Kernel(IS),
            Input1(Input1),
            Input2(Input2)
        { addAccessor(&Input1); addAccessor(&Input2); }

        void kernel() {
            // combine
            float4 in1 = convert_float4(Input1());
            float4 in2 = convert_float4(Input2());
            int4 out = convert_int4(sqrtf((in1 * in1) + (in2 * in2)));
            out = min(out, 255);
            out = max(out, 0);

            // debug: just gather and normalize input 1
            //int4 out = (convert_int4(Input1()) + MAX_DERIV) * 255 / (2 * MAX_DERIV);

            // debug: just gather and normalize input 2
            //int4 out = (convert_int4(Input2()) + MAX_DERIV) * 255 / (2 * MAX_DERIV);

            out.w = 255;
            output() = convert_uchar4(out);
        }
};
#endif

// Main function
#ifdef HIPACC
int w = 1024;
int h = 1024;
uchar4 *in;
uchar4 *out;
int main(int argc, const char **argv) {
#else
#ifndef FILTERSCRIPT
int runRSSobel(int w, int h, uchar4 *in, uchar4 *out) {
#else
int runFSSobel(int w, int h, uchar4 *in, uchar4 *out) {
#endif
#endif
    const int width = w;
    const int height = h;
    const int size_x = SIZE_X;
    const int size_y = SIZE_Y;
    const int offset_x = size_x >> 1;
    const int offset_y = size_y >> 1;
    uchar4 *host_in = in;
    uchar4 *host_out = out;
    float timing = 0.0f;

    // only filter kernel sizes 3x3, 5x5, and 7x7 implemented
    if (size_x != size_y || !(size_x == 3 || size_x == 5 || size_x == 7)) {
        fprintf(stderr, "Wrong filter kernel size. Currently supported values: 3x3, 5x5, and 7x7!\n");
        return -1;
    }

    // filter coefficients
    const int mask_x[] = {
#if SIZE_X==3
        -1, 0,  1,
        -2, 0,  2,
        -1, 0,  1
#endif
#if SIZE_X==5
        -1, -2,  0,  2,  1,
        -4, -8,  0,  8,  4,
        -6, -12, 0,  12, 6,
        -4, -8,  0,  8,  4,
        -1, -2,  0,  2,  1
#endif
#if SIZE_X==7
        -1,  -4,  -5,   0, 5,   4,  1,
        -6,  -24, -30,  0, 30,  24,  6,
        -15, -60, -75,  0, 75,  60, 15,
        -20, -80, -100, 0, 100, 80, 20,
        -15, -60, -75,  0, 75,  60, 15,
        -6,  -24, -30,  0, 30,  24,  6,
        -1,  -4,  -5,   0, 5,   4,  1
#endif
    };
    const int mask_xx[] = {
#if SIZE_X==3
        -1, 0, +1
#endif
#if SIZE_X==5
        -1, -2, 0, +2, +1
#endif
#if SIZE_X==7
        -1, -4, -5, 0, 5, 4, 1
#endif
    };
    const int mask_xy[] = {
#if SIZE_X==3
        1, 2, 1,
#endif
#if SIZE_X==5
        1, 4, 6, 4, 1
#endif
#if SIZE_X==7
        1, 6, 15, 20, 15, 6, 1
#endif
    };

    const int mask_y[] = {
#if SIZE_X==3
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
#endif
#if SIZE_X==5
        -1, -4, -6,  -4, -1,
        -2, -8, -12, -8, -2,
         0,  0,  0,   0,  0,
         2,  8,  12,  8,  2,
         1,  4,  6,   4,  1
#endif
#if SIZE_X==7
        -1, -6,  -15, -20,  -15, -6,  -1,
        -4, -24, -60, -80,  -60, -24, -4,
        -5, -30, -75, -100, -75, -30, -5,
         0,  0,   0,   0,    0,   0,   0,
         5,  30,  75,  100,  75,  30,  5,
         4,  24,  60,  80,   60,  24,  4,
         1,  6,   15,  20,   15,  6,   1
#endif
    };
    const int mask_yx[] = {
#if SIZE_X==3
        1, 2, 1
#endif
#if SIZE_X==5
        1, 4, 6, 4, 1
#endif
#if SIZE_X==7
        1, 6, 15, 20, 15, 6, 1
#endif
    };
    const int mask_yy[] = {
#if SIZE_X==3
        -1, 0, +1
#endif
#if SIZE_X==5
        -1, -2, 0, +2, +1
#endif
#if SIZE_X==7
        -1, -4, -5, 0, 5, 4, 1
#endif
    };

    // input and output image of width x height pixels
    Image<uchar4> IN(width, height);
    Image<uchar4> OUT(width, height);
#ifndef NO_SEP
    Image<short4> DERIV1(width, height);
    Image<short4> DERIV2(width, height);
    Image<short4> TMP(width, height);
#endif

    // filter mask
    Mask<int> MX(size_x, size_y);
    Mask<int> MY(size_x, size_y);
    MX = mask_x;
    MY = mask_y;
#ifndef NO_SEP
    Mask<int> MXX(size_x, 1);
    Mask<int> MXY(1, size_y);
    Mask<int> MYX(size_x, 1);
    Mask<int> MYY(1, size_y);
    MXX = mask_xx;
    MXY = mask_xy;
    MYX = mask_yx;
    MYY = mask_yy;
#else
    Domain D(size_x, size_y);
#endif

    IterationSpace<uchar4> IsOut(OUT);
#ifndef NO_SEP
    IterationSpace<short4> IsDeriv1(DERIV1);
    IterationSpace<short4> IsDeriv2(DERIV2);
    IterationSpace<short4> IsTmp(TMP);
#endif

    IN = host_in;
    OUT = host_out;

#ifdef NO_SEP
    BoundaryCondition<uchar4> BcInClamp(IN, size_x, size_y, BOUNDARY_CLAMP);
    Accessor<uchar4> AccInClamp(BcInClamp);
    Sobel filter(IsOut, AccInClamp, D, MX, MY, size_x);

    filter.execute();
    timing = hipaccGetLastKernelTiming();
#else
    BoundaryCondition<uchar4> BcInClamp(IN, size_x, 1, BOUNDARY_CLAMP);
    Accessor<uchar4> AccInClamp(BcInClamp);
    SobelRow derivx1(IsTmp, AccInClamp, MXX, size_x);

    BoundaryCondition<short4> BcTmpClamp(TMP, 1, size_y, BOUNDARY_CLAMP);
    Accessor<short4> AccTmpClamp(BcTmpClamp);
    SobelColumn derivx2(IsDeriv1, AccTmpClamp, MXY, size_y);

    SobelRow derivy1(IsTmp, AccInClamp, MYX, size_x);

    SobelColumn derivy2(IsDeriv2, AccTmpClamp, MYY, size_y);

    derivx1.execute();
    timing = hipaccGetLastKernelTiming();
    derivx2.execute();
    timing += hipaccGetLastKernelTiming();
    derivy1.execute();
    timing += hipaccGetLastKernelTiming();
    derivy2.execute();
    timing += hipaccGetLastKernelTiming();

    Accessor<short4> AccDeriv1(DERIV1);
    Accessor<short4> AccDeriv2(DERIV2);
    SobelCombine comb(IsOut, AccDeriv1, AccDeriv2);

    comb.execute();
    timing += hipaccGetLastKernelTiming();
#endif

    // get results
    host_out = OUT.getData();

    return timing;
}

