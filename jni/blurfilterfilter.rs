#ifndef _BLURFILTERFILTER_RS_
#define _BLURFILTERFILTER_RS_

#pragma version(1)
#pragma rs java_package_name(org.hipacc.example)

rs_allocation in;
int in_offset_x;
int in_offset_y;
int size_x;
int size_y;
int is_stride;
int is_width;
int is_height;
int is_offset_x;
int is_offset_y;

void rsBlurFilterfilterKernel(uchar4 *_IS, uint32_t x, uint32_t y) {
    const int gid_x = x;
    const int gid_y = y;
    {
        if (gid_x >= is_offset_x && gid_x < is_width + is_offset_x && gid_y >= is_offset_y && gid_y < is_height + is_offset_y) {
            {
                int4 _tmp0 = { 0, 0, 0, 0 };
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -2 - is_offset_x + in_offset_x, gid_y + -2 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -1 - is_offset_x + in_offset_x, gid_y + -2 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 0 - is_offset_x + in_offset_x, gid_y + -2 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 1 - is_offset_x + in_offset_x, gid_y + -2 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 2 - is_offset_x + in_offset_x, gid_y + -2 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -2 - is_offset_x + in_offset_x, gid_y + -1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -1 - is_offset_x + in_offset_x, gid_y + -1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 0 - is_offset_x + in_offset_x, gid_y + -1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 1 - is_offset_x + in_offset_x, gid_y + -1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 2 - is_offset_x + in_offset_x, gid_y + -1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -2 - is_offset_x + in_offset_x, gid_y + 0 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -1 - is_offset_x + in_offset_x, gid_y + 0 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 0 - is_offset_x + in_offset_x, gid_y + 0 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 1 - is_offset_x + in_offset_x, gid_y + 0 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 2 - is_offset_x + in_offset_x, gid_y + 0 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -2 - is_offset_x + in_offset_x, gid_y + 1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -1 - is_offset_x + in_offset_x, gid_y + 1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 0 - is_offset_x + in_offset_x, gid_y + 1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 1 - is_offset_x + in_offset_x, gid_y + 1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 2 - is_offset_x + in_offset_x, gid_y + 1 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -2 - is_offset_x + in_offset_x, gid_y + 2 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + -1 - is_offset_x + in_offset_x, gid_y + 2 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 0 - is_offset_x + in_offset_x, gid_y + 2 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 1 - is_offset_x + in_offset_x, gid_y + 2 - is_offset_y + in_offset_y));
                }
                {
                    _tmp0 += convert_int4(rsGetElementAt_uchar4(in, gid_x + 2 - is_offset_x + in_offset_x, gid_y + 2 - is_offset_y + in_offset_y));
                }
                *_IS = convert_uchar4((1 / (float)(size_x * size_y) /2) * convert_float4(_tmp0));
            }
        }
    }
}

#endif //_BLURFILTERFILTER_RS_

