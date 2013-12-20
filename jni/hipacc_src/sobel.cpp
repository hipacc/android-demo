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
class Sobel : public Kernel<uchar4> {
  private:
    Accessor<uchar4> &input;
    Domain &dom;
    Mask<int> &maskX;
    Mask<int> &maskY;

  public:
    Sobel(IterationSpace<uchar4> &iter, Accessor<uchar4> &input,
          Domain &dom, Mask<int> &maskX, Mask<int> &maskY)
        : Kernel(iter), input(input), dom(dom), maskX(maskX), maskY(maskY) {
      addAccessor(&input);
    }

    void kernel() {
      float4 sumX = 0.0f;
      float4 sumY = 0.0f;

      iterate(dom, [&] () {
        float4 in = convert_float4(input(dom));
        sumX += (float)maskX(dom) * in;
        sumY += (float)maskY(dom) * in;
      });

      // combine
      int4 out = convert_int4(sqrtf((sumX * sumX) + (sumY * sumY)));
      out = min(out, 255);
      out = max(out, 0);
      out.w = 255;
      output() = convert_uchar4(out);
    }
};


// Main function
FILTER_NAME(Sobel) {
  const int width = w;
  const int height = h;
  const int size_x = SIZE_X;
  const int size_y = SIZE_Y;
  const int offset_x = size_x >> 1;
  const int offset_y = size_y >> 1;
  float timing = 0.0f;

  // only filter kernel sizes 3x3, 5x5, and 7x7 implemented
  if (size_x != size_y || !(size_x == 3 || size_x == 5 || size_x == 7)) {
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

  // input and output image of width x height pixels
  Image<uchar4> IN(width, height);
  Image<uchar4> OUT(width, height);

  IN = pin;
  OUT = pout;

  Domain D(size_x, size_y);

  // filter mask
  Mask<int> MX(size_x, size_y);
  Mask<int> MY(size_x, size_y);
  MX = mask_x;
  MY = mask_y;

  BoundaryCondition<uchar4> BcInClamp(IN, size_x, size_y, BOUNDARY_CLAMP);
  Accessor<uchar4> AccInClamp(BcInClamp);
  IterationSpace<uchar4> IsOut(OUT);
  Sobel filter(IsOut, AccInClamp, D, MX, MY);

  filter.execute();
  timing = hipaccGetLastKernelTiming();

  // get results
  pout = OUT.getData();

  return timing;
}

