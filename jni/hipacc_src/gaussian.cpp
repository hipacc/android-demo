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


#ifdef NO_SEP
class Gaussian : public Kernel<uchar4> {
    private:
        Accessor<uchar4> &Input;
        Mask<float> &cMask;
        const int size_x, size_y;

    public:
        Gaussian(IterationSpace<uchar4> &IS,
                Accessor<uchar4> &Input, Mask<float> &cMask, const int
                size_x, const int size_y) :
            Kernel(IS),
            Input(Input),
            cMask(cMask),
            size_x(size_x),
            size_y(size_y)
        { addAccessor(&Input); }

        void kernel() {
            output() = convert_uchar4(convolve(cMask, HipaccSUM, [&] () -> float4 {
                    return cMask() * convert_float4(Input(cMask));
                    }) + 0.5f);
        }
};
#else
class GaussianRow : public Kernel<float4> {
    private:
        Accessor<uchar4> &Input;
        Mask<float> &cMask;
        const int size;

    public:
        GaussianRow(IterationSpace<float4> &IS, Accessor<uchar4>
                &Input, Mask<float> &cMask, const int size) :
            Kernel(IS),
            Input(Input),
            cMask(cMask),
            size(size)
        { addAccessor(&Input); }

        void kernel() {
            output() = convolve(cMask, HipaccSUM, [&] () -> float4 {
                    return cMask() * convert_float4(Input(cMask));
                    });
        }
};
class GaussianColumn : public Kernel<uchar4> {
    private:
        Accessor<float4> &Input;
        Mask<float> &cMask;
        const int size;

    public:
        GaussianColumn(IterationSpace<uchar4> &IS,
                Accessor<float4> &Input, Mask<float> &cMask, const int size) :
            Kernel(IS),
            Input(Input),
            cMask(cMask),
            size(size)
        { addAccessor(&Input); }

        void kernel() {
            output() = convert_uchar4(convolve(cMask, HipaccSUM, [&] () -> float4 {
                    return cMask() * convert_float4(Input(cMask));
                    }) + 0.5f);
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
int runRSGaussian(int w, int h, uchar4 *in, uchar4 *out) {
#else
int runFSGaussian(int w, int h, uchar4 *in, uchar4 *out) {
#endif
#endif
    const int width = w;
    const int height = h;
    const int size_x = SIZE_X;
    const int size_y = SIZE_Y;
    const int offset_x = size_x >> 1;
    const int offset_y = size_y >> 1;
    double timing;
    uchar4 *host_in = in;
    uchar4 *host_out = out;

    // only filter kernel sizes 3x3, 5x5, and 7x7 implemented
    if (size_x != size_y || !(size_x == 3 || size_x == 5 || size_x == 7)) {
        return -1;
    }

    // convolution filter mask
    const float filter_x[] = {
#if SIZE_X == 3
        0.238994f, 0.522011f, 0.238994f
#endif
#if SIZE_X == 5
        0.070766f, 0.244460f, 0.369546f, 0.244460f, 0.070766f
#endif
#if SIZE_X == 7
        0.028995f, 0.103818f, 0.223173f, 0.288026f, 0.223173f, 0.103818f, 0.028995f
#endif
    };
    const float filter_y[] = {
#if SIZE_X == 3
        0.238994f, 0.522011f, 0.238994f
#endif
#if SIZE_X == 5
        0.070766f, 0.244460f, 0.369546f, 0.244460f, 0.070766f
#endif
#if SIZE_X == 7
        0.028995f, 0.103818f, 0.223173f, 0.288026f, 0.223173f, 0.103818f, 0.028995f
#endif
    };
    const float filter_xy[] = {
#if SIZE_X == 3
        0.057118f, 0.124758f, 0.057118f,
        0.124758f, 0.272496f, 0.124758f,
        0.057118f, 0.124758f, 0.057118f
#endif
#if SIZE_X == 5
        0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f,
        0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
        0.026151f, 0.090339f, 0.136565f, 0.090339f, 0.026151f,
        0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f,
        0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f
#endif
#if SIZE_X == 7
        0.000841, 0.003010, 0.006471, 0.008351, 0.006471, 0.003010, 0.000841,
        0.003010, 0.010778, 0.023169, 0.029902, 0.023169, 0.010778, 0.003010,
        0.006471, 0.023169, 0.049806, 0.064280, 0.049806, 0.023169, 0.006471,
        0.008351, 0.029902, 0.064280, 0.082959, 0.064280, 0.029902, 0.008351,
        0.006471, 0.023169, 0.049806, 0.064280, 0.049806, 0.023169, 0.006471,
        0.003010, 0.010778, 0.023169, 0.029902, 0.023169, 0.010778, 0.003010,
        0.000841, 0.003010, 0.006471, 0.008351, 0.006471, 0.003010, 0.000841
#endif
    };

    // input and output image of width x height pixels
    Image<uchar4> IN(width, height);
    Image<uchar4> OUT(width, height);
    Image<float4> TMP(width, height);

    // filter mask
#ifdef NO_SEP
    Mask<float> M(size_x, size_y);
    M = filter_xy;
#else
    Mask<float> MX(size_x, 1);
    Mask<float> MY(1, size_y);
    MX = filter_x;
    MY = filter_y;
#endif

    IterationSpace<uchar4> IsOut(OUT);
    IterationSpace<float4> IsTmp(TMP);

    IN = host_in;
    OUT = host_out;


#ifdef NO_SEP
    BoundaryCondition<uchar4> BcInClamp2(IN, size_x, size_y, BOUNDARY_CLAMP);
    Accessor<uchar4> AccInClamp2(BcInClamp2);
    Gaussian filter(IsOut, AccInClamp2, M, size_x, size_y);

    filter.execute();
    timing = hipaccGetLastKernelTiming();
#else
    BoundaryCondition<uchar4> BcInClamp(IN, size_x, 1, BOUNDARY_CLAMP);
    Accessor<uchar4> AccInClamp(BcInClamp);
    GaussianRow filterRow(IsTmp, AccInClamp, MX, size_x);

    BoundaryCondition<float4> BcTmpClamp(TMP, 1, size_y, BOUNDARY_CLAMP);
    Accessor<float4> AccTmpClamp(BcTmpClamp);
    GaussianColumn filterCol(IsOut, AccTmpClamp, MY, size_y);

    filterRow.execute();
    timing = hipaccGetLastKernelTiming();
    filterCol.execute();
    timing += hipaccGetLastKernelTiming();
#endif

    // get results
    host_out = OUT.getData();

    return timing;
}

