#pragma version(1)
#pragma rs java_package_name(org.hipacc.example)

rs_allocation in;

uint32_t width;
uint32_t height;

void root(uchar4* out, uint32_t x, uint32_t y) {
    uint4 sum = 0;
    uint count = 0;
    for (int yi = y-2; yi <= y+2; ++yi) {
        for (int xi = x-2; xi <= x+2; ++xi) {
            if (xi >= 0 && xi < width && yi >= 0 && yi < height) {
                sum += convert_uint4(rsGetElementAt_uchar4(in, xi, yi));
                ++count;
            }
        }
    }
    *out = convert_uchar4(sum/count/2);
}