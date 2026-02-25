__kernel void sobel_edge_detection(
    __global const float* img,
    __global float* output,
    int rows,
    int cols
) {
    const int idx = get_global_id(0);
    const int i = idx / cols;
    const int j = idx % cols;

    if (i < 1 || i >= rows - 1 || j < 1 || j >= cols - 1) {
        return;
    }

    const int idx_tl = (i - 1) * cols + (j - 1);  // top-left
    const int idx_tc = (i - 1) * cols +  j;       // top-center
    const int idx_tr = (i - 1) * cols + (j + 1);  // top-right
    const int idx_ml =  i      * cols + (j - 1);  // middle-left
    const int idx_mr =  i      * cols + (j + 1);  // middle-right
    const int idx_bl = (i + 1) * cols + (j - 1);  // bottom-left
    const int idx_bc = (i + 1) * cols +  j;       // bottom-center
    const int idx_br = (i + 1) * cols + (j + 1);  // bottom-right

    // Sobel Gx kernel: [[-1, 0, 1], [-2, 0, 2], [-1, 0, 1]]
    const float gx =
        -1.0f * img[idx_tl] + 1.0f * img[idx_tr]
        -2.0f * img[idx_ml] + 2.0f * img[idx_mr]
        -1.0f * img[idx_bl] + 1.0f * img[idx_br];

    // Sobel Gy kernel: [[-1, -2, -1], [0, 0, 0], [1, 2, 1]]
    const float gy =
        -1.0f * img[idx_tl] - 2.0f * img[idx_tc] - 1.0f * img[idx_tr]
        +1.0f * img[idx_bl] + 2.0f * img[idx_bc] + 1.0f * img[idx_br];

    output[idx] = sqrt(gx * gx + gy * gy);
}

__kernel void sobel_threshold(
    __global float* output,
    float threshold,
    int rows,
    int cols
) {
    const int idx = get_global_id(0);
    if (idx >= rows * cols) {
        return;
    }

    if (output[idx] < threshold) {
        output[idx] = 0.0f;
    }
    output[idx] = min(output[idx], 1.0f);
}
