#pragma version(1)
#pragma rs java_package_name(org.hipacc.demo)

rs_allocation input;

uint32_t width;
uint32_t height;

void root(uchar4* out, uint32_t x, uint32_t y) {
    const int size_x = 5;
    const int size_y = 5;
    const int offset_x = size_x/2;
    const int offset_y = size_y/2;

    uint4 sum = 0;
    for (int yi = (int)y-offset_y; yi <= y+offset_y; ++yi) {
        int yc = min(max(yi, 0), (int)height-1);
        for (int xi = (int)x-offset_x; xi <= x+offset_x; ++xi) {
            int xc = min(max(xi, 0), (int)width-1);
            sum += convert_uint4(rsGetElementAt_uchar4(input, xc, yc));
        }
    }

    *out = convert_uchar4(sum/(size_x*size_y));
}