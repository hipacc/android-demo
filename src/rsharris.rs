#pragma version(1)
#pragma rs java_package_name(org.hipacc.example)

rs_allocation dx;
rs_allocation dy;
rs_allocation dxy;
float k;

void root(float* out, uint32_t x, uint32_t y) {
    float ix = rsGetElementAt_float(dx, x, y);
    float iy = rsGetElementAt_float(dy, x, y);
    float ixy = rsGetElementAt_float(dxy, x, y);
    *out = ((ix * iy) - (ixy * ixy)) -   /* det   */
            (k * (ix + iy) * (ix + iy)); /* trace */
}