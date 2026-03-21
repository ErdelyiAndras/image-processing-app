#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Image.h"
#include "types.h"

#include <algorithm>
#include <cstring>
#include <vector>
#include <cmath>

Image::Image(PixelIdx rows, PixelIdx cols) : rows(rows), cols(cols), image(nullptr) {
    if (rows == 0 || cols == 0) {
        return;
    }
    const size_t num_pixels = size();
    image = new PixelValue[num_pixels];
    std::memset(image, 0, sizeof(PixelValue) * num_pixels);
}

Image::Image(const std::string& path) : rows(0), cols(0), image(nullptr) {
    int w, h, channels;
    unsigned char* raw = stbi_load(path.c_str(), &w, &h, &channels, 1); // force 1 channel (grayscale)
    if (!raw) {
        throw std::runtime_error("Failed to load image from path: " + path);
    }

    rows = static_cast<PixelIdx>(h);
    cols = static_cast<PixelIdx>(w);
    const size_t num_pixels = size();
    image = new PixelValue[num_pixels];

    for (size_t i = 0; i < num_pixels; ++i) {
        image[i] = static_cast<PixelValue>(raw[i]) / 255.0f;
    }

    stbi_image_free(raw);
}

Image::Image(const Image& other) : rows(0), cols(0), image(nullptr) {
    if (!other.image) {
        return;
    }

    rows = other.rows;
    cols = other.cols;
    const size_t num_pixels = size();
    image = new PixelValue[num_pixels];
    std::memcpy(image, other.image, sizeof(PixelValue) * num_pixels);
}

Image::Image(Image&& other) noexcept
    : rows(other.rows), cols(other.cols), image(other.image) {
    other.rows = 0;
    other.cols = 0;
    other.image = nullptr;
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
        const size_t num_pixels = size();
        image = new PixelValue[num_pixels];
        std::memcpy(image, other.image, sizeof(PixelValue) * num_pixels);
    }

    return *this;
}

Image& Image::operator=(Image&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    delete[] image;

    rows = other.rows;
    cols = other.cols;
    image = other.image;

    other.rows = 0;
    other.cols = 0;
    other.image = nullptr;

    return *this;
}

PixelValue& Image::operator()(PixelIdx row, PixelIdx col) {
    if (row >= rows || col >= cols) {
        throw std::out_of_range("Pixel index out of range");
    }
    return image[row * cols + col];
}

const PixelValue& Image::operator()(PixelIdx row, PixelIdx col) const {
    if (row >= rows || col >= cols) {
        throw std::out_of_range("Pixel index out of range");
    }
    return image[row * cols + col];
}

bool Image::save(const std::string& name, const std::string& ext) const {
    if (!image || rows == 0 || cols == 0) {
        return false;
    }

    std::vector<uint8_t> output(size());
    for (uint64_t i = 0; i < size(); ++i) {
        PixelValue val = std::clamp(image[i], 0.0f, 1.0f);
        output[i] = static_cast<uint8_t>(val * 255.0f + 0.5f);
    }
    return saveUsingFormat(name, ext, output, static_cast<int>(cols), static_cast<int>(rows), 1);
}

void Image::clear() {
    if (image) {
        std::memset(image, 0, sizeof(PixelValue) * size());
    }
}

bool Image::saveComposite(
    const std::string& name,
    const std::string& ext,
    const Image& baseImage,
    const std::vector<std::pair<const Image* const, Color>>& overlays
) {
    const PixelIdx rows{ baseImage.getRows() };
    const PixelIdx cols{ baseImage.getCols() };

    std::vector<uint8_t> rgbData(static_cast<size_t>(rows) * cols * 3U);

    for (PixelIdx i{ 0U }; i < rows; ++i) {
        for (PixelIdx j{ 0U }; j < cols; ++j) {
            const size_t pixel{ (static_cast<size_t>(i) * cols + j) * 3 };
            const uint8_t gray{ toUint8(baseImage(i, j)) };

            rgbData[pixel]     = gray;
            rgbData[pixel + 1] = gray;
            rgbData[pixel + 2] = gray;

            for (const auto& [overlay, color] : overlays) {
                const PixelValue t{ (*overlay)(i, j) };
                if (t > 0.0f) {
                    rgbData[pixel]     = blend(rgbData[pixel],     color.r, t);
                    rgbData[pixel + 1] = blend(rgbData[pixel + 1], color.g, t);
                    rgbData[pixel + 2] = blend(rgbData[pixel + 2], color.b, t);
                }
            }
        }
    }

    return saveUsingFormat(name, ext, rgbData, static_cast<int>(cols), static_cast<int>(rows), 3);
}

bool Image::saveUsingFormat(
    const std::string& name,
    const std::string& ext,
    const std::vector<uint8_t>& output,
    int cols,
    int rows,
    int channels
) {
    if (ext == ".png") {
        return stbi_write_png((name + ext).c_str(), cols, rows, channels, output.data(), cols * channels) != 0;
    } else if (ext == ".jpg" || ext == ".jpeg") {
        return stbi_write_jpg((name + ext).c_str(), cols, rows, channels, output.data(), 95) != 0;
    } else if (ext == ".bmp") {
        return stbi_write_bmp((name + ext).c_str(), cols, rows, channels, output.data()) != 0;
    } else if (ext == ".tga") {
        return stbi_write_tga((name + ext).c_str(), cols, rows, channels, output.data()) != 0;
    }
    return false;
}

uint8_t Image::toUint8(PixelValue value) {
    return static_cast<uint8_t>(std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
}

uint8_t Image::blend(uint8_t base, uint8_t overlay, float t) {
    return static_cast<uint8_t>(std::round((1.0f - t) * base + t * overlay));
}
