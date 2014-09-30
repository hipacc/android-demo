#include <iostream>
#include <vector>

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "hipacc.hpp"
#include "filter_name.hpp"

using namespace hipacc;
using namespace hipacc::math;


// Kernel description in HIPAcc
class Laplace : public Kernel<uchar4> {
  private:
    Accessor<uchar4> &input;
    Domain &dom;
    Mask<int> &mask;

  public:
    Laplace(IterationSpace<uchar4> &iter, Accessor<uchar4> &input,
            Domain &dom, Mask<int> &mask)
            : Kernel(iter), input(input), dom(dom), mask(mask) {
        addAccessor(&input);
    }

    void kernel() {
        int4 sum = reduce(dom, Reduce::SUM, [&] () -> int4 {
                       return mask(dom) * convert_int4(input(dom));
                   });
        sum = min(sum, 255);
        sum = max(sum, 0);
        sum.w = 255;
        output() = convert_uchar4(sum);
    }
};


// Main function
FILTER_NAME(Laplace) {
    const int width = w;
    const int height = h;
    const int size_x = SIZE_X;
    const int size_y = SIZE_Y;
    const int offset_x = size_x >> 1;
    const int offset_y = size_y >> 1;
    float timing = 0.0f;

    // only filter kernel sizes 3x3 and 5x5 supported
    if (size_x != size_y || !(size_x == 3 || size_x == 5)) {
        return -1;
    }

    // convolution filter mask
    const int mask[size_y][size_x] = {
#if SIZE_X==1
        { 0,  1,  0 },
        { 1, -4,  1 },
        { 0,  1,  0 }
#endif
#if SIZE_X==3
        { 2,  0,  2 },
        { 0, -8,  0 },
        { 2,  0,  2 }
#endif
#if SIZE_X==5
        { 1,   1,   1,   1,   1 },
        { 1,   1,   1,   1,   1 },
        { 1,   1, -24,   1,   1 },
        { 1,   1,   1,   1,   1 },
        { 1,   1,   1,   1,   1 }
#endif
    };

    // input and output image of width x height pixels
    Image<uchar4> In(width, height);
    Image<uchar4> Out(width, height);

    In = pin;
    Out = pout;

    // filter mask
    Mask<int> M(mask);

    Domain D(M);

    BoundaryCondition<uchar4> BcInClamp(In, D, Boundary::CLAMP);
    Accessor<uchar4> AccInClamp(BcInClamp);
    IterationSpace<uchar4> IsOut(Out);
    Laplace filter(IsOut, AccInClamp, D, M);

    filter.execute();
    timing = hipaccGetLastKernelTiming();

    // get results
    pout = Out.getData();

    return timing;
}

