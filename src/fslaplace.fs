#pragma version(1)
#pragma rs java_package_name(org.hipacc.example)

rs_allocation in;
rs_allocation mask;

uint32_t width;
uint32_t height;

uchar4 __attribute__((kernel)) root(uint32_t x, uint32_t y) {
    const int size_x = 5;
    const int size_y = 5;
    const int offset_x = size_x/2;
    const int offset_y = size_y/2;
    int4 sum = 0.0f;
    for (int yi = 0; yi < size_y; ++yi) {
        int yc = y - offset_y + yi;
        yc = min(max(yc, 0), (int)height-1);
        for (int xi = 0; xi < size_x; ++xi) {
            int xc = x - offset_x + xi;
            xc = min(max(xc, 0), (int)width-1);
            sum += convert_int4(rsGetElementAt_uchar4(in, xc, yc)) *
                           rsGetElementAt_int(mask, xi, yi);
        }
    }
    sum = min(sum, 255);
    sum = max(sum, 0);
    sum.w = 255;
    return convert_uchar4(sum);
}