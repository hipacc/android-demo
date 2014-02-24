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
#include "filter_name.hpp"

using namespace hipacc;


// Kernel description in HIPAcc
class Harris : public Kernel<float> {
  private:
    Accessor<float> &inputX;
    Accessor<float> &inputY;
    Domain &dom;
    Mask<float> &mask;
    float k;

  public:
    Harris(IterationSpace<float> &iter,
           Accessor<float> &inputX, Accessor<float> &inputY,
           Domain &dom, Mask<float> &mask, float k)
              : Kernel(iter), inputX(inputX), inputY(inputY),
                dom(dom), mask(mask), k(k) {
        addAccessor(&inputX);
        addAccessor(&inputY);
    }

    void kernel() {
        float sumX = 0.0f;
        float sumY = 0.0f;
        float sumXY = 0.0f;

        iterate(dom, [&] () {
            float dx = inputX(dom);
            float dy = inputY(dom);
            sumX += mask(dom) * dx * dx;
            sumY += mask(dom) * dy * dy;
            sumXY += mask(dom) * dx * dy;
        });

        output() = ((sumX * sumY) - (sumXY * sumXY))      /* det   */
                   - (k * (sumX + sumY) * (sumX + sumY)); /* trace */
    }
};

class HarrisDeriv : public Kernel<float> {
  private:
    Accessor<uchar> &input;
    Domain &dom;
    Mask<float> &mask;

  public:
    HarrisDeriv(IterationSpace<float> &iter,
                Accessor<uchar> &input, Domain &dom, Mask<float> &mask)
            : Kernel(iter), input(input), dom(dom), mask(mask) {
        addAccessor(&input);
    }

    void kernel() {
        output() = reduce(dom, HipaccSUM, [&] () -> float {
                       return input(dom) * mask(dom);
                   });
    }
};


// Main function
FILTER_NAME(Harris) {
    float k = 0.04f;
    float threshold = 20000.0f;
    const int width = w;
    const int height = h;
    const int size_x = SIZE_X;
    const int size_y = SIZE_Y;
    float timing = 0.0f;

    // only filter kernel sizes 3x3, 5x5, and 7x7 implemented
    if (size_x != size_y || !(size_x == 3 || size_x == 5 || size_x == 7)) {
        return -1;
    }

    uchar *filter_in = new uchar[width * height];
    float *result = new float[width * height];

    for (int i = 0; i < width * height; ++i) {
        filter_in[i] = .2126 * pin[i].x + .7152 * pin[i].y + .0722 * pin[i].z;
    }

    // convolution filter masks
    const float mask_x[3][3] = {
        { -0.166666667f,          0.0f,  0.166666667f },
        { -0.166666667f,          0.0f,  0.166666667f },
        { -0.166666667f,          0.0f,  0.166666667f }
    };
    const float mask_y[3][3] = {
        { -0.166666667f, -0.166666667f, -0.166666667f },
        {          0.0f,          0.0f,          0.0f },
        {  0.166666667f,  0.166666667f,  0.166666667f }
    };
    const float gauss[size_y][size_x] = {
#if SIZE_X == 3
        { 0.057118f, 0.124758f, 0.057118f },
        { 0.124758f, 0.272496f, 0.124758f },
        { 0.057118f, 0.124758f, 0.057118f }
#endif
#if SIZE_X == 5
        { 0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f },
        { 0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f },
        { 0.026151f, 0.090339f, 0.136565f, 0.090339f, 0.026151f },
        { 0.017300f, 0.059761f, 0.090339f, 0.059761f, 0.017300f },
        { 0.005008f, 0.017300f, 0.026151f, 0.017300f, 0.005008f }
#endif
#if SIZE_X == 7
        { 0.000841, 0.003010, 0.006471, 0.008351, 0.006471, 0.003010, 0.000841 },
        { 0.003010, 0.010778, 0.023169, 0.029902, 0.023169, 0.010778, 0.003010 },
        { 0.006471, 0.023169, 0.049806, 0.064280, 0.049806, 0.023169, 0.006471 },
        { 0.008351, 0.029902, 0.064280, 0.082959, 0.064280, 0.029902, 0.008351 },
        { 0.006471, 0.023169, 0.049806, 0.064280, 0.049806, 0.023169, 0.006471 },
        { 0.003010, 0.010778, 0.023169, 0.029902, 0.023169, 0.010778, 0.003010 },
        { 0.000841, 0.003010, 0.006471, 0.008351, 0.006471, 0.003010, 0.000841 }
#endif
    };

    // input and output image of width x height pixels
    Image<uchar> In(width, height);
    Image<float> Res(width, height);
    Image<float> Dx(width, height);
    Image<float> Dy(width, height);

    In = filter_in;
    Res = result;

    Mask<float> G(gauss);
    Mask<float> MX(mask_x);
    Mask<float> MY(mask_y);

    Domain D(G);
    Domain DX(MX);
    Domain DY(MY);

    BoundaryCondition<uchar> BcInClamp(In, DX, BOUNDARY_CLAMP);
    Accessor<uchar> AccInClamp(BcInClamp);
    IterationSpace<float> IsDx(Dx);
    HarrisDeriv dx(IsDx, AccInClamp, DX, MX);

    dx.execute();
    timing = hipaccGetLastKernelTiming();

    IterationSpace<float> IsDy(Dy);
    HarrisDeriv dy(IsDy, AccInClamp, DY, MY);

    dy.execute();
    timing += hipaccGetLastKernelTiming();

    BoundaryCondition<float> BcDxClamp(Dx, G, BOUNDARY_CLAMP);
    Accessor<float> AccDx(BcDxClamp);
    BoundaryCondition<float> BcDyClamp(Dy, G, BOUNDARY_CLAMP);
    Accessor<float> AccDy(BcDyClamp);
    IterationSpace<float> IsRes(Res);
    Harris filter(IsRes, AccDx, AccDy, D, G, k);

    filter.execute();
    timing += hipaccGetLastKernelTiming();

    // get results
    result = Res.getData();

    // draw output
    memcpy(pout, pin, sizeof(uchar4) * width * height);

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; y++) {
            int pos = y*width+x;
            if (result[pos] > threshold) {
                for (int i = -10; i <= 10; ++i) {
                    if (x+i >= 0 && x+i < width) {
                        pout[pos+i].x =
                                pout[pos+i].y =
                                pout[pos+i].z = 255;
                    }
                }
                for (int i = -10; i <= 10; ++i) {
                    if (y+i > 0 && y+i < height) {
                        pout[pos+(i*width)].x =
                                pout[pos+(i*width)].y =
                                pout[pos+(i*width)].z = 255;
                    }
                }
            }
        }
    }

    delete[] result;
    delete[] filter_in;

    return timing;
}

