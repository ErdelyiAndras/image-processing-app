#ifndef IMAGE_H
#define IMAGE_H

#include "Color.h"
#include "types.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

class Image {
public:
    Image(PixelIdx rows = 0, PixelIdx cols = 0);

    Image(const std::string& path);

    Image(const Image& other);

    Image(Image&& other) noexcept;

    ~Image();

    inline PixelIdx getRows() const { return rows; }

    inline PixelIdx getCols() const { return cols; }

    Image& operator=(const Image& other);

    Image& operator=(Image&& other) noexcept;

    PixelValue& operator()(PixelIdx row, PixelIdx col);

    const PixelValue& operator()(PixelIdx row, PixelIdx col) const;

    bool save(const std::string& name, const std::string& ext) const;

    void clear();

    inline PixelValue* data() { return image; }

    inline const PixelValue* data() const { return image; }

    static bool saveComposite(
        const std::string& name,
        const std::string& ext,
        const Image& baseImage,
        const std::vector<std::pair<const Image* const, Color>>& overlays
    );

private:
    PixelIdx rows;
    PixelIdx cols;
    PixelValue* image;

    inline size_t size() const { return static_cast<size_t>(rows) * static_cast<size_t>(cols); }

    static bool saveUsingFormat(
        const std::string& name,
        const std::string& ext,
        const std::vector<uint8_t>& output,
        int cols,
        int rows,
        int channels
    );

    static uint8_t toUint8(PixelValue value);
    static uint8_t blend(uint8_t base, uint8_t overlay, float t);
};

#endif // IMAGE_H
