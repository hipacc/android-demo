#pragma version(1)
#pragma rs java_package_name(org.hipacc.example)

rs_allocation in1;
rs_allocation in2;

uchar4 __attribute__((kernel)) root(uint32_t x, uint32_t y) {
    float4 input1 = convert_float4(rsGetElementAt_short4(in1, x, y));
    float4 input2 = convert_float4(rsGetElementAt_short4(in2, x, y));

    int4 output = convert_int4(sqrt((input1 * input1) + (input2 * input2)));
    output = min(output, 255);
    output = max(output, 0);
    output.w = 255;
    
    return convert_uchar4(output);
}