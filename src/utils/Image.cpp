#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Image.h"

#include <algorithm>
#include <cstring>
#include <vector>

Image::Image(int rows, int cols) : rows(rows), cols(cols), image(nullptr) {
    if (rows < 0 || cols < 0) {
        throw std::invalid_argument("Rows and columns must be non-negative.");
    }
    if (rows == 0 || cols == 0) {
        return;
    }
    image = new float[rows * cols];
    std::memset(image, 0, sizeof(float) * rows * cols);
}

Image::Image(const std::string& path) : rows(0), cols(0), image(nullptr) {
    int w, h, channels;
    unsigned char* raw = stbi_load(path.c_str(), &w, &h, &channels, 1); // force 1 channel (grayscale)
    if (!raw) {
        throw std::runtime_error("Failed to load image from path: " + path);
    }

    rows = h;
    cols = w;
    image = new float[rows * cols];

    for (int i = 0; i < rows * cols; ++i) {
        image[i] = static_cast<float>(raw[i]) / 255.0f;
    }

    stbi_image_free(raw);
}

Image::Image(const Image& other) : rows(0), cols(0), image(nullptr) {
    if (!other.image) {
        return;
    }

    rows = other.rows;
    cols = other.cols;
    image = new float[rows * cols];
    std::memcpy(image, other.image, sizeof(float) * rows * cols);
}

Image::~Image() {
    delete[] image;
    image = nullptr;
}

Image& Image::operator=(const Image& other) {
    if (this == &other) {
        return *this;
    }

    delete[] image;
    image = nullptr;

    rows = other.rows;
    cols = other.cols;

    if (other.image) {
        image = new float[rows * cols];
        std::memcpy(image, other.image, sizeof(float) * rows * cols);
    }

    return *this;
}

float& Image::operator()(int row, int col) {
    return image[row * cols + col];
}

const float& Image::operator()(int row, int col) const {
    return image[row * cols + col];
}

bool Image::save(const std::string& path) const {
    if (!image || rows == 0 || cols == 0) {
        return false;
    }

    // Convert float [0.0, 1.0] to uint8 [0, 255]
    std::vector<unsigned char> output(rows * cols);
    for (int i = 0; i < rows * cols; ++i) {
        float val = std::clamp(image[i], 0.0f, 1.0f);
        output[i] = static_cast<unsigned char>(val * 255.0f + 0.5f);
    }

    // Determine format from extension
    std::string ext;
    size_t dot = path.find_last_of('.');
    if (dot != std::string::npos) {
        ext = path.substr(dot);
    }

    if (ext == ".png") {
        return stbi_write_png(path.c_str(), cols, rows, 1, output.data(), cols) != 0;
    } else if (ext == ".jpg" || ext == ".jpeg") {
        return stbi_write_jpg(path.c_str(), cols, rows, 1, output.data(), 95) != 0;
    } else if (ext == ".bmp") {
        return stbi_write_bmp(path.c_str(), cols, rows, 1, output.data()) != 0;
    } else if (ext == ".tga") {
        return stbi_write_tga(path.c_str(), cols, rows, 1, output.data()) != 0;
    }

    return false;
}
