__kernel void canny_gradient(
    __global const float* img,
    __global float* grad_mag,
    __global float* grad_dir,
    int rows,
    int cols
) {
    const int idx = get_global_id(0);
    const int i = idx / cols;
    const int j = idx % cols;

    if (i < 1 || i >= rows - 1 || j < 1 || j >= cols - 1) {
        return;
    }

    const int idx_tl = (i - 1) * cols + (j - 1);
    const int idx_tc = (i - 1) * cols +  j;
    const int idx_tr = (i - 1) * cols + (j + 1);
    const int idx_ml =  i      * cols + (j - 1);
    const int idx_mr =  i      * cols + (j + 1);
    const int idx_bl = (i + 1) * cols + (j - 1);
    const int idx_bc = (i + 1) * cols +  j;
    const int idx_br = (i + 1) * cols + (j + 1);

    const float gx =
        -1.0f * img[idx_tl] + 1.0f * img[idx_tr]
        -2.0f * img[idx_ml] + 2.0f * img[idx_mr]
        -1.0f * img[idx_bl] + 1.0f * img[idx_br];

    const float gy =
        -1.0f * img[idx_tl] - 2.0f * img[idx_tc] - 1.0f * img[idx_tr]
        +1.0f * img[idx_bl] + 2.0f * img[idx_bc] + 1.0f * img[idx_br];

    grad_mag[idx] = sqrt(gx * gx + gy * gy);
    grad_dir[idx] = atan2(gy, gx);
}

__kernel void canny_nms(
    __global const float* grad_mag,
    __global const float* grad_dir,
    __global float* nms_output,
    int rows,
    int cols
) {
    const int idx = get_global_id(0);
    const int i = idx / cols;
    const int j = idx % cols;

    if (i < 1 || i >= rows - 1 || j < 1 || j >= cols - 1) {
        return;
    }

    float angle = grad_dir[idx] * 180.0f / M_PI_F;
    if (angle < 0.0f) {
        angle += 180.0f;
    }

    float q = 0.0f;
    float r = 0.0f;

    // 0 degrees (horizontal)
    if ((angle >= 0.0f && angle < 22.5f) || (angle >= 157.5f && angle <= 180.0f)) {
        q = grad_mag[i * cols + (j + 1)];
        r = grad_mag[i * cols + (j - 1)];
    }
    // 45 degrees
    else if (angle >= 22.5f && angle < 67.5f) {
        q = grad_mag[(i + 1) * cols + (j - 1)];
        r = grad_mag[(i - 1) * cols + (j + 1)];
    }
    // 90 degrees (vertical)
    else if (angle >= 67.5f && angle < 112.5f) {
        q = grad_mag[(i + 1) * cols + j];
        r = grad_mag[(i - 1) * cols + j];
    }
    // 135 degrees
    else if (angle >= 112.5f && angle < 157.5f) {
        q = grad_mag[(i - 1) * cols + (j - 1)];
        r = grad_mag[(i + 1) * cols + (j + 1)];
    }

    if (grad_mag[idx] >= q && grad_mag[idx] >= r) {
        nms_output[idx] = grad_mag[idx];
    }
}

__kernel void canny_double_threshold(
    __global const float* nms,
    __global float* output,
    float low_threshold,
    float high_threshold,
    float strong_val,
    float weak_val,
    int rows,
    int cols
) {
    const int idx = get_global_id(0);
    if (idx >= rows * cols) {
        return;
    }

    if (nms[idx] >= high_threshold) {
        output[idx] = strong_val;
    } else if (nms[idx] >= low_threshold) {
        output[idx] = weak_val;
    } else {
        output[idx] = 0.0f;
    }
}

__kernel void canny_hysteresis(
    __global const float* input,
    __global float* output,
    __global atomic_int* changed_flag,
    float strong_val,
    float weak_val,
    int rows,
    int cols
) {
    const int idx = get_global_id(0);
    const int i = idx / cols;
    const int j = idx % cols;

    if (i < 1 || i >= rows - 1 || j < 1 || j >= cols - 1) {
        output[idx] = input[idx];
        return;
    }

    const int idx_tl = (i - 1) * cols + (j - 1);
    const int idx_tc = (i - 1) * cols +  j;
    const int idx_tr = (i - 1) * cols + (j + 1);
    const int idx_ml =  i      * cols + (j - 1);
    const int idx_mr =  i      * cols + (j + 1);
    const int idx_bl = (i + 1) * cols + (j - 1);
    const int idx_bc = (i + 1) * cols +  j;
    const int idx_br = (i + 1) * cols + (j + 1);

    if (input[idx] == weak_val) {
        if (input[idx_tl] == strong_val || input[idx_tc] == strong_val ||
            input[idx_tr] == strong_val || input[idx_ml] == strong_val ||
            input[idx_mr] == strong_val || input[idx_bl] == strong_val ||
            input[idx_bc] == strong_val || input[idx_br] == strong_val) {
            output[idx] = strong_val;
            atomic_fetch_or_explicit(changed_flag, 1, memory_order_relaxed, memory_scope_device);
            return;
        }
    }
    output[idx] = input[idx];
}

__kernel void canny_suppress_weak(
    __global float* output,
    float strong_val,
    int rows,
    int cols
) {
    const int idx = get_global_id(0);
    if (idx >= rows * cols) {
        return;
    }

    if (output[idx] != strong_val) {
        output[idx] = 0.0f;
    }
}
