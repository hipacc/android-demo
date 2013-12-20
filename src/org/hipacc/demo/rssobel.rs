#pragma version(1)
#pragma rs java_package_name(org.hipacc.demo)

rs_allocation input;
rs_allocation maskX;
rs_allocation maskY;

uint32_t width;
uint32_t height;

void root(uchar4* out, uint32_t x, uint32_t y) {
    const int size_x = 5;
    const int size_y = 5;
    const int offset_x = size_x/2;
    const int offset_y = size_y/2;
    float4 sumX = 0.0f;
    float4 sumY = 0.0f;
    for (int yi = 0; yi < size_y; ++yi) {
        int yc = y - offset_y + yi;
        yc = min(max(yc, 0), (int)height-1);
        for (int xi = 0; xi < size_x; ++xi) {
            int xc = x - offset_x + xi;
            xc = min(max(xc, 0), (int)width-1);
            float4 in = convert_float4(rsGetElementAt_uchar4(input, xc, yc));
            sumX += (float)rsGetElementAt_int(maskX, xi, yi) * in;
            sumY += (float)rsGetElementAt_int(maskY, xi, yi) * in;
        }
    }

    int4 output = convert_int4(sqrt((sumX * sumX) + (sumY * sumY)));
    output = min(output, 255);
    output = max(output, 0);
    output.w = 255;

    *out = convert_uchar4(output);
}