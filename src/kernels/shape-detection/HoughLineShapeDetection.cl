__kernel void hough_line_vote(
    __global const float* edge_map,
    __global volatile atomic_uint* accumulator,
    __global const float* cos_table,
    __global const float* sin_table,
    int rows,
    int cols,
    float rho_max,
    float rho_resolution,
    int num_rho_bins,
    int num_theta_bins
) {
    const int idx = get_global_id(0);
    const int i = idx / cols;
    const int j = idx % cols;

    if (i >= rows || j >= cols) {
        return;
    }

    if (edge_map[idx] == 0.0f) {
        return;
    }

    for (int theta_idx = 0; theta_idx < num_theta_bins; ++theta_idx) {
        const float rho = (float)j * cos_table[theta_idx] + (float)i * sin_table[theta_idx];
        const float rho_idx_float = round((rho + rho_max) / rho_resolution);

        if (rho_idx_float >= 0.0f && rho_idx_float < (float)num_rho_bins) {
            const int rho_idx = (int)rho_idx_float;
            const int acc_idx = rho_idx * num_theta_bins + theta_idx;
            atomic_fetch_add_explicit(&accumulator[acc_idx], 1u, memory_order_relaxed, memory_scope_device);
        }
    }
}
