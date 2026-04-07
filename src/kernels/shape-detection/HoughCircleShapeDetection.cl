__kernel void hough_circle_vote(
    __global const float* edge_map,
    __global volatile atomic_uint* accumulator,
    __global const float* cos_table,
    __global const float* sin_table,
    int rows,
    int cols,
    int min_radius,
    int max_radius,
    int num_angle_steps
) {
    const int idx = get_global_id(0);
    const int py = idx / cols;
    const int px = idx % cols;

    if (py >= rows || px >= cols) {
        return;
    }

    if (edge_map[idx] == 0.0f) {
        return;
    }

    for (uint r = min_radius; r <= max_radius; ++r) {
        const uint r_idx = r - min_radius;

        for (uint a = 0; a < num_angle_steps; ++a) {
            const int cx = px - (int)round((float)r * cos_table[a]);
            const int cy = py - (int)round((float)r * sin_table[a]);

            if (cx >= 0 && cx < cols && cy >= 0 && cy < rows) {
                const uint acc_idx = (r_idx * (uint)rows + (uint)cy) * (uint)cols + (uint)cx;
                atomic_fetch_add_explicit(&accumulator[acc_idx], 1u, memory_order_relaxed, memory_scope_device);
            }
        }
    }
}
