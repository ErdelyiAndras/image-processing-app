__kernel void gaussian_blur(
    __global const float* input,
    __global float* output,
    __global const float* kernel_data,
    int rows,
    int cols,
    int kernel_size
) {
    const int idx = get_global_id(0);
    const int i = idx / cols;
    const int j = idx % cols;

    if (i >= rows || j >= cols) {
        return;
    }

    int half_size = kernel_size / 2;
    float val = 0.0f;

    for (int ki = -half_size; ki <= half_size; ++ki) {
        for (int kj = -half_size; kj <= half_size; ++kj) {
            int ni = clamp(i + ki, 0, rows - 1);
            int nj = clamp(j + kj, 0, cols - 1);
            val += input[ni * cols + nj] * kernel_data[(ki + half_size) * kernel_size + (kj + half_size)];
        }
    }

    output[idx] = val;
}
