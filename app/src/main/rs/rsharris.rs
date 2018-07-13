#pragma version(1)
#pragma rs java_package_name(org.hipacc.demo)

rs_allocation derivX;
rs_allocation derivY;
rs_allocation mask;

float k;
uint32_t width;
uint32_t height;

void root(float* out, uint32_t x, uint32_t y) {
    const int size_x = 5;
    const int size_y = 5;
    const int offset_x = size_x/2;
    const int offset_y = size_y/2;
    
    float sumX = 0.0f;
    float sumY = 0.0f;
    float sumXY = 0.0f;
    for (int yi = 0; yi < size_y; ++yi) {
        int yc = y - offset_y + yi;
        yc = min(max(yc, 0), (int)height-1);
        for (int xi = 0; xi < size_x; ++xi) {
            int xc = x - offset_x + xi;
            xc = min(max(xc, 0), (int)width-1);
            float dx = rsGetElementAt_float(derivX, xc, yc);
            float dy = rsGetElementAt_float(derivY, xc, yc);
            float maskVal = rsGetElementAt_float(mask, xi, yi);
            sumX += maskVal * dx * dx;
            sumY += maskVal * dy * dy;
            sumXY += maskVal * dx * dy;
        }
    }
    
    *out = ((sumX * sumY) - (sumXY * sumXY)) -   /* det   */
            (k * (sumX + sumY) * (sumX + sumY)); /* trace */
}