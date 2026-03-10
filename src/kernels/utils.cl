__kernel void sum_int(__global int* data, int offset)
{
    int idx = get_global_id(0);
    int right_idx = idx + offset;
    data[idx] += data[right_idx];
}

__kernel void sum_float(__global float* data, int offset)
{
    int idx = get_global_id(0);
    int right_idx = idx + offset;
    data[idx] += data[right_idx];
}
