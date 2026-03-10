__kernel void tv_norm_mtx_and_dx_dy(
    __global const float* img,
    __global float* tv_norm_mtx,
    __global float* dx_mtx,
    __global float* dy_mtx,
    int rows,
    int cols,
    float eps
) {
    const int idx = get_global_id(0);
    const int i = idx / cols;
    const int j = idx % cols;

    if (i >= rows - 1 || j >= cols - 1) {
        return;
    }

    const int idx_right = idx + 1;
    const int idx_down = idx + cols;

    const float x_diff = img[idx] - img[idx_right];
    const float y_diff = img[idx] - img[idx_down];
    const float grad_mag = sqrt(x_diff * x_diff + y_diff * y_diff + eps);

    tv_norm_mtx[idx] = grad_mag;

    const float dx = x_diff / grad_mag;
    const float dy = y_diff / grad_mag;

    dx_mtx[idx] = dx;
    dy_mtx[idx] = dy;
}

__kernel void grad_from_dx_dy(
    __global const float* dx,
    __global const float* dy,
    __global float* grad,
    int rows,
    int cols
) {
    const int idx = get_global_id(0);
    const int i = idx / cols;
    const int j = idx % cols;

    if (idx >= rows * cols) {
        return;
    }

    float g = 0.0f;

    if (i < rows - 1 && j < cols - 1) {
        g += dx[idx] + dy[idx];
    }

    if (j > 0 && i < rows - 1) {
        g -= dx[idx - 1];
    }

    if (i > 0 && j < cols - 1) {
        g -= dy[idx - cols];
    }

    grad[idx] = g;
}

__kernel void l2_norm_mtx_and_grad(
    __global const float* img,
    __global const float* orig,
    __global float* l2_norm_mtx,
    __global float* grad,
    int rows,
    int cols
) {
    int idx = get_global_id(0);
    if (idx >= rows * cols) {
        return;
    }
    float diff = img[idx] - orig[idx];
    grad[idx] = diff;
    l2_norm_mtx[idx] = diff * diff;
}

__kernel void eval_loss_and_grad(
    __global float* grad,
    __global const float* tv_grad,
    __global const float* l2_grad,
    float strength
) {
    int idx = get_global_id(0);
    grad[idx] = strength * tv_grad[idx] + l2_grad[idx];
}

__kernel void eval_momentum(
    __global float* momentum,
    __global const float* grad,
    float momentum_beta
) {
    int idx = get_global_id(0);
    momentum[idx] *= momentum_beta;
    momentum[idx] += grad[idx] * (1.0f - momentum_beta);
}

__kernel void update_img(
    __global float* img,
    __global const float* momentum,
    float step,
    float momentum_beta,
    float counter
) {
    int idx = get_global_id(0);
    float bias_correction = 1.0f - pow(momentum_beta, counter);
    img[idx] -= step / bias_correction * momentum[idx];
}
