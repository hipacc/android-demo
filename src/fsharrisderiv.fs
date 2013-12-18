#pragma version(1)
#pragma rs java_package_name(org.hipacc.example)

rs_allocation input;
rs_allocation mask;

uint32_t width;
uint32_t height;

float __attribute__((kernel)) root(uint32_t x, uint32_t y) {
    const int size_x = 3;
    const int size_y = 3;
    const int offset_x = size_x/2;
    const int offset_y = size_y/2;
    float sum = 0.0f;
    for (int yi = 0; yi < size_y; ++yi) {
        int yc = y - offset_y + yi;
        yc = min(max(yc, 0), (int)height-1);
        for (int xi = 0; xi < size_x; ++xi) {
            int xc = x - offset_x + xi;
            xc = min(max(xc, 0), (int)width-1);
            sum += rsGetElementAt_float(input, xc, yc) *
                    rsGetElementAt_float(mask, xi, yi);
        }
    }
    return sum;
}