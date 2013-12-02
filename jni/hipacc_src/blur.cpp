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

#include "hipacc.hpp"

using namespace hipacc;
using namespace hipacc::math;


// Kernel description in HIPAcc
class BlurFilter : public Kernel<uchar4> {
    private:
        Accessor<uchar4> &in;
        Domain &dom;
        int size_x, size_y;

    public:
        BlurFilter(IterationSpace<uchar4> &iter, Accessor<uchar4> &in,
        		   Domain &dom, int size_x, int size_y)
            : Kernel(iter), in(in), dom(dom), size_x(size_x), size_y(size_y) {
          addAccessor(&in);
        }

        void kernel() {
            output() = convert_uchar4( (1/(float)(size_x*size_y)) *
                    convert_float4(reduce(dom, HipaccSUM, [&] () -> int4 {
                    return convert_int4(in(dom));
                    })));
        }
};


// Main function
#ifdef HIPACC
uchar4* pin;
uchar4* pout;
const int w = 1024;
const int h = 1024;
int main(int argc, const char **argv) {
#else
int runBlur(int w, int h, uchar4 *pin, uchar4 *pout) {
#endif
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
        return -1;
    }

    // define Domain for blur filter
    Domain dom(size_x, size_y);
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
    dom = domain;

    // input and output image of width x height pixels
    Image<uchar4> in(width, height);
    Image<uchar4> out(width, height);

    BoundaryCondition<uchar4> bound(in, size_x, size_y, BOUNDARY_CLAMP);
    Accessor<uchar4> acc(bound, width-2*offset_x, height-2*offset_y, offset_x, offset_y);

    IterationSpace<uchar4> iter(out, width-2*offset_x, height-2*offset_y, offset_x, offset_y);
    BlurFilter filter(iter, acc, dom, size_x, size_y);

    in = host_in;
    out = host_out;

    filter.execute();
    timing = hipaccGetLastKernelTiming();

    // get results
    host_out = out.getData();

    return timing;
}

