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
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "hipacc.hpp"
#include "filter_name.hpp"

using namespace hipacc;
using namespace hipacc::math;


// Kernel description in HIPAcc
class Blur : public Kernel<uchar4> {
  private:
    Accessor<uchar4> &input;
    Domain &dom;
    const int size_x, size_y;

  public:
    Blur(IterationSpace<uchar4> &iter, Accessor<uchar4> &input, Domain &dom,
         const int size_x, const int size_y)
        : Kernel(iter), input(input), dom(dom), size_x(size_x), size_y(size_y) {
      addAccessor(&input);
    }

    void kernel() {
      uint4 sum = reduce(dom, HipaccSUM, [&] () -> uint4 {
                    return convert_uint4(input(dom));
                  });
      output() = convert_uchar4(convert_float4(sum)/(size_x*size_y));
    }
};


// Main function
FILTER_NAME(Blur) {
  const int width = w;
  const int height = h;
  const int size_x = SIZE_X;
  const int size_y = SIZE_Y;
  const int offset_x = size_x >> 1;
  const int offset_y = size_y >> 1;
  float timing = 0.0f;

  // define Domain for blur filter
  Domain dom(size_x, size_y);

  // input and output image of width x height pixels
  Image<uchar4> in(width, height);
  Image<uchar4> out(width, height);

  BoundaryCondition<uchar4> bound(in, size_x, size_y, BOUNDARY_CLAMP);
  Accessor<uchar4> acc(bound);

  IterationSpace<uchar4> iter(out);
  Blur filter(iter, acc, dom, size_x, size_y);

  in = pin;
  out = pout;

  filter.execute();
  timing = hipaccGetLastKernelTiming();

  // get results
  pout = out.getData();

  return timing;
}

