//
// Copyright (c) 2013, University of Erlangen-Nuremberg
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <iostream>
#include <vector>
#include <numeric>

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "hipacc.hpp"

using namespace hipacc;

#define NO_SEP

#ifdef NO_SEP
class HarrisDeriv : public Kernel<float> {
  private:
    Accessor<uchar> &Input;
    Mask<float> &cMask;

  public:
    HarrisDeriv(IterationSpace<float> &IS,
                Accessor<uchar> &Input, Mask<float> &cMask)
          : Kernel(IS),
            Input(Input),
            cMask(cMask) {
      addAccessor(&Input);
    }

    void kernel() {
      float sum = 0.0f;
      sum += convolve(cMask, HipaccSUM, [&] () -> float {
          return Input(cMask) * cMask();
      });
      output() = sum;
    }
};
class Harris : public Kernel<float> {
  private:
    Accessor<float> &Dx;
    Accessor<float> &Dy;
    Domain &cDom;
    Mask<float> &cMask;
    float k;

  public:
    Harris(IterationSpace<float> &IS,
           Accessor<float> &Dx, Accessor<float> &Dy,
           Domain &cDom, Mask<float> &cMask,
           float k)
          : Kernel(IS),
            Dx(Dx),
            Dy(Dy),
            cDom(cDom),
            cMask(cMask),
            k(k) {
      addAccessor(&Dx);
      addAccessor(&Dy);
    }

    void kernel() {
      float sumX = 0.0f;
      float sumY = 0.0f;
      float sumXY = 0.0f;
      iterate(cDom, [&] () {
          float dx = Dx(cDom);
          float dy = Dy(cDom);
          sumX += cMask(cDom) * dx * dx;
          sumY += cMask(cDom) * dy * dy;
          sumXY += cMask(cDom) * dx * dy;
      });
      output() = ((sumX * sumY) - (sumXY * sumXY))      /* det   */
                 - (k * (sumX + sumY) * (sumX + sumY)); /* trace */
    }
};
#else
class HarrisDeriv1DCol : public Kernel<float> {
  private:
    Accessor<uchar> &Input;
    Mask<float> &cMask;
    const int size;

  public:
    HarrisDeriv1DCol(IterationSpace<float> &IS,
                     Accessor<uchar> &Input, Mask<float> &cMask,
                     const int size)
          : Kernel(IS),
            Input(Input),
            cMask(cMask),
            size(size) {
      addAccessor(&Input);
    }

    void kernel() {
      float sum = 0.0f;
      sum += convolve(cMask, HipaccSUM, [&] () -> float {
          return Input(cMask) * cMask();
      });
      output() = sum;
    }
};
class HarrisDeriv1DRow : public Kernel<float> {
  private:
    Accessor<float> &Input;
    Mask<float> &cMask;
    const int size;

  public:
    HarrisDeriv1DRow(IterationSpace<float> &IS,
                     Accessor<float> &Input, Mask<float> &cMask,
                     const int size)
          : Kernel(IS),
            Input(Input),
            cMask(cMask),
            size(size) {
      addAccessor(&Input);
    }

    void kernel() {
      float sum = 0.0f;
      sum += convolve(cMask, HipaccSUM, [&] () -> float {
          return Input(cMask) * cMask();
      });
      output() = sum*sum;
    }
};

class HarrisDeriv2D : public Kernel<float> {
  private:
    Accessor<uchar> &input;
    Domain &dom;
    Mask<float> &mask1;
    Mask<float> &mask2;

  public:
    HarrisDeriv2D(IterationSpace<float> &iter,
                  Accessor<uchar> &input, Domain &dom,
                  Mask<float> &mask1, Mask<float> &mask2)
          : Kernel(iter),
            input(input),
            dom(dom),
            mask1(mask1),
            mask2(mask2) {
      addAccessor(&input);
    }

    void kernel() {
      float sum1 = 0, sum2 = 0;
      iterate(dom, [&] () -> void {
        uchar val = input(dom);
        sum1 += val * mask1(dom);
        sum2 += val * mask2(dom);
      });
      output() = sum1 * sum2;
    }
};
class HarrisGaussianRow : public Kernel<float> {
  private:
    Accessor<float> &Input;
    Mask<float> &cMask;
    const int size;

  public:
    HarrisGaussianRow(IterationSpace<float> &IS, Accessor<float> &Input,
                      Mask<float> &cMask, const int size)
        : Kernel(IS),
          Input(Input),
          cMask(cMask),
          size(size) {
      addAccessor(&Input);
    }

    void kernel() {
      output() = convolve(cMask, HipaccSUM, [&] () -> float {
          return cMask() * Input(cMask);
      });
    }
};
class HarrisGaussianCol : public Kernel<float> {
  private:
    Accessor<float> &Input;
    Mask<float> &cMask;
    const int size;

  public:
    HarrisGaussianCol(IterationSpace<float> &IS,
                         Accessor<float> &Input, Mask<float> &cMask,
                         const int size)
        : Kernel(IS),
          Input(Input),
          cMask(cMask),
          size(size) {
      addAccessor(&Input);
    }

    void kernel() {
      output() = convolve(cMask, HipaccSUM, [&] () -> float {
          return cMask() * Input(cMask);
      }) + 0.5f;
    }
};
class Harris : public Kernel<float> {
  private:
    Accessor<float> &Dx;
    Accessor<float> &Dy;
    Accessor<float> &Dxy;
    float k;

  public:
    Harris(IterationSpace<float> &IS,
           Accessor<float> &Dx, Accessor<float> &Dy, Accessor<float> &Dxy,
           float k)
          : Kernel(IS),
            Dx(Dx),
            Dy(Dy),
            Dxy(Dxy),
            k(k) {
      addAccessor(&Dx);
      addAccessor(&Dy);
      addAccessor(&Dxy);
    }

    void kernel() {
      float x = Dx();
      float y = Dy();
      float xy = Dxy();
      output() = ((x * y) - (xy * xy)) /* det   */
                 - (k * (x + y) * (x + y)); /* trace */
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
int runRSHarris(int w, int h, uchar4 *in, uchar4 *out) {
#else
int runFSHarris(int w, int h, uchar4 *in, uchar4 *out) {
#endif
#endif
    float k = 0.04f;
    float threshold = 20000.0f;
    const int width = w;
    const int height = h;
    const int size_x = SIZE_X;
    const int size_y = SIZE_Y;
    float timing = 0.0f;

    // input and output image of width x height pixels
    Image<uchar> IN(width, height);
    Image<float> RES(width, height);
    Image<float> DX(width, height);
    Image<float> DY(width, height);
#ifndef NO_SEP
    Image<float> DXY(width, height);
    Image<float> TMP(width, height);
#endif

    // only filter kernel sizes 3x3, 5x5, and 7x7 implemented
    if (size_x != size_y || !(size_x == 3 || size_x == 5 || size_x == 7)) {
        fprintf(stderr, "Wrong filter kernel size. Currently supported values: 3x3, 5x5, and 7x7!\n");
        return -1;
    }

    uchar4 *host_in = in;
    uchar4 *host_out = out;
    uchar *filter_in = new uchar[width * height];
    float *result = new float[width * height];

    for (int i = 0; i < width * height; ++i) {
        filter_in[i] = .2126 * in[i].x + .7152 * in[i].y + .0722 * in[i].z;
    }

    // convolution filter mask
#ifdef NO_SEP
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
#else
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
#endif

    const float mask_x[9] = { -0.166666667f,          0.0f,  0.166666667f,
                              -0.166666667f,          0.0f,  0.166666667f,
                              -0.166666667f,          0.0f,  0.166666667f };
    const float mask_y[9] = { -0.166666667f, -0.166666667f, -0.166666667f,
                                       0.0f,          0.0f,          0.0f,
                               0.166666667f,  0.166666667f,  0.166666667f };
    Mask<float> MX(3, 3);
    Mask<float> MY(3, 3);
    MX = mask_x;
    MY = mask_y;

#ifndef NO_SEP
    const float mask_v[3] = {  0.166666667f,  0.166666667f,  0.166666667f };
    const float mask_m[3] = {         -1.0f,          0.0f,          1.0f };
    Mask<float> MXX(3, 1);
    Mask<float> MXY(1, 3);
    Mask<float> MYX(3, 1);
    Mask<float> MYY(1, 3);
    MXX = mask_m;
    MXY = mask_v;
    MYX = mask_v;
    MYY = mask_m;
#endif

#ifdef NO_SEP
    Domain D(size_x, size_y);
    Mask<float> G(size_x, size_y);
    G = filter_xy;
#else
    Mask<float> GX(size_x, 1);
    Mask<float> GY(1, size_y);
    GX = filter_x;
    GY = filter_y;
#endif

    IterationSpace<float> IsRes(RES);
    IterationSpace<float> IsDx(DX);
    IterationSpace<float> IsDy(DY);
#ifndef NO_SEP
    IterationSpace<float> IsDxy(DXY);
    IterationSpace<float> IsTmp(TMP);
#endif

    IN = filter_in;
    RES = result;

    BoundaryCondition<uchar> BcInClamp(IN, 3, 3, BOUNDARY_CLAMP);
    Accessor<uchar> AccInClamp(BcInClamp);

#ifdef NO_SEP
    HarrisDeriv dx(IsDx, AccInClamp, MX);
    dx.execute();
    timing = hipaccGetLastKernelTiming();

    HarrisDeriv dy(IsDy, AccInClamp, MY);
    dy.execute();
    timing += hipaccGetLastKernelTiming();

    BoundaryCondition<float> BcDxClamp(DX, size_x, size_y, BOUNDARY_CLAMP);
    Accessor<float> AccDx(BcDxClamp);
    BoundaryCondition<float> BcDyClamp(DY, size_x, size_y, BOUNDARY_CLAMP);
    Accessor<float> AccDy(BcDyClamp);
    Harris filter(IsRes, AccDx, AccDy, D, G, k);
    filter.execute();
    timing += hipaccGetLastKernelTiming();
#else
    BoundaryCondition<float> BcTmpDcClamp(TMP, 1, 3, BOUNDARY_CLAMP);
    Accessor<float> AccTmpDcClamp(BcTmpDcClamp);

    HarrisDeriv1DCol d1xc(IsTmp, AccInClamp, MXX, 3);
    d1xc.execute();
    timing = hipaccGetLastKernelTiming();

    HarrisDeriv1DRow d1xr(IsDx, AccTmpDcClamp, MXY, 3);
    d1xr.execute();
    timing += hipaccGetLastKernelTiming();

    HarrisDeriv1DCol d1yc(IsTmp, AccInClamp, MYX, 3);
    d1yc.execute();
    timing += hipaccGetLastKernelTiming();

    HarrisDeriv1DRow d1yr(IsDy, AccTmpDcClamp, MYY, 3);
    d1yr.execute();
    timing += hipaccGetLastKernelTiming();

    Domain dom(3, 3);
    HarrisDeriv2D d2(IsDxy, AccInClamp, dom, MX, MY);
    d2.execute();
    timing += hipaccGetLastKernelTiming();

    BoundaryCondition<float> BcTmpClamp(TMP, 1, size_y, BOUNDARY_CLAMP);
    Accessor<float> AccTmpClamp(BcTmpClamp);

    BoundaryCondition<float> BcInClampDx(DX, size_x, 1, BOUNDARY_CLAMP);
    Accessor<float> AccInClampDx(BcInClampDx);
    HarrisGaussianRow gxr(IsTmp, AccInClampDx, GX, size_x);
    HarrisGaussianCol gxc(IsDx, AccTmpClamp, GY, size_y);
    gxr.execute();
    timing += hipaccGetLastKernelTiming();
    gxc.execute();
    timing += hipaccGetLastKernelTiming();

    BoundaryCondition<float> BcInClampDy(DY, size_x, 1, BOUNDARY_CLAMP);
    Accessor<float> AccInClampDy(BcInClampDy);
    HarrisGaussianRow gyr(IsTmp, AccInClampDy, GX, size_x);
    HarrisGaussianCol gyc(IsDy, AccTmpClamp, GY, size_y);
    gyr.execute();
    timing += hipaccGetLastKernelTiming();
    gyc.execute();
    timing += hipaccGetLastKernelTiming();

    BoundaryCondition<float> BcInClampDxy(DXY, size_x, 1, BOUNDARY_CLAMP);
    Accessor<float> AccInClampDxy(BcInClampDxy);
    HarrisGaussianRow gxyr(IsTmp, AccInClampDxy, GX, size_x);
    HarrisGaussianCol gxyc(IsDxy, AccTmpClamp, GY, size_y);
    gxyr.execute();
    timing += hipaccGetLastKernelTiming();
    gxyc.execute();
    timing += hipaccGetLastKernelTiming();

    Accessor<float> AccDxy(DXY);
    Accessor<float> AccDx(DX);
    Accessor<float> AccDy(DY);

    Harris filter(IsRes, AccDx, AccDy, AccDxy, k);
    filter.execute();
    timing += hipaccGetLastKernelTiming();
#endif

    // get results
    result = RES.getData();

    // draw output
    memcpy(host_out, host_in, sizeof(uchar4) * width * height);

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; y++) {
            int pos = y*width+x;
            if (result[pos] > threshold) {
                for (int i = -10; i <= 10; ++i) {
                    if (x+i >= 0 && x+i < width)
                        host_out[pos+i].x =
                                host_out[pos+i].y =
                                host_out[pos+i].z = 255;
                }
                for (int i = -10; i <= 10; ++i) {
                    if (y+i > 0 && y+i < height)
                        host_out[pos+(i*width)].x =
                                host_out[pos+(i*width)].y =
                                host_out[pos+(i*width)].z = 255;
                }
            }
        }
    }

    delete[] result;
    delete[] filter_in;

    return timing;
}

