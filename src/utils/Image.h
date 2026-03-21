#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <stdexcept>
#include <vector>

#include "types.h"

/**
 * @class Image
 * @brief A simple image class for handling 2D, single-channel (grayscale) images with float precision.
 *
 * Provides constructors for creating images from dimensions, file paths, or by copying another Image.
 * Supports element access, assignment, and saving to file via stb_image_write.
 */
class Image {
public:
    /**
     * @brief Constructs an empty image or an image with the given dimensions.
     * @param rows Number of rows (default: 0).
     * @param cols Number of columns (default: 0).
     */
    Image(PixelIdx rows = 0, PixelIdx cols = 0);

    /**
     * @brief Constructs an image by loading from a file (grayscale).
     * @param path Path to the image file.
     */
    Image(const std::string& path);

    /**
     * @brief Copy constructor.
     * @param other Image to copy from.
     */
    Image(const Image& other);

    /**
     * @brief Move constructor.
     * @param other Image to move from.
     */
    Image(Image&& other) noexcept;

    /**
     * @brief Destructor. Releases allocated memory.
     */
    ~Image();

    /**
     * @brief Returns the number of rows in the image.
     * @return Number of rows.
     */
    inline PixelIdx getRows() const { return rows; }

    /**
     * @brief Returns the number of columns in the image.
     * @return Number of columns.
     */
    inline PixelIdx getCols() const { return cols; }

    /**
     * @brief Assignment operator.
     * @param other Image to assign from.
     * @return Reference to this image.
     */
    Image& operator=(const Image& other);

    /**
     * @brief Move assignment operator.
     * @param other Image to move from.
     * @return Reference to this image.
     */
    Image& operator=(Image&& other) noexcept;

    /**
     * @brief Accesses a pixel value (modifiable).
     * @param row Row index.
     * @param col Column index.
     * @return Reference to the pixel value at (row, col).
     */
    PixelValue& operator()(PixelIdx row, PixelIdx col);

    /**
     * @brief Accesses a pixel value (const).
     * @param row Row index.
     * @param col Column index.
     * @return Const reference to the pixel value at (row, col).
     */
    const PixelValue& operator()(PixelIdx row, PixelIdx col) const;

    /**
     * @brief Saves the image to a file.
     *
     * Supported formats: .png, .jpg/.jpeg, .bmp, .tga
     * The format is determined by the file extension.
     *
     * @param path Output file path.
     * @return true if saved successfully, false otherwise.
     */
    bool save(const std::string& name, const std::string& ext) const;

    void clear();

    /**
     * @brief Returns a pointer to the underlying memory of the flattened image.
     *
     * The data is stored in row-major order as a contiguous 1D array of size rows * cols.
     * Pixel values are in the range [0.0, 1.0].
     * This pointer can be used for efficient data transfer to GPU memory.
     *
     * @return Pointer to the first element of the image data buffer.
     */
    inline PixelValue* data() { return image; }

    /**
     * @brief Returns a const pointer to the underlying memory of the flattened image.
     *
     * The data is stored in row-major order as a contiguous 1D array of size rows * cols.
     * Pixel values are in the range [0.0, 1.0].
     * This pointer can be used for efficient data transfer to GPU memory.
     *
     * @return Const pointer to the first element of the image data buffer.
     */
    inline const PixelValue* data() const { return image; }

    static bool saveComposite(
        const std::string& name,
        const std::string& ext,
        const Image& processedImage,
        const Image& edgeMap,
        const Image& shapeMap
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
