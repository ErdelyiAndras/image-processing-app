#include "Context.h"
#include "Image.h"
#include "types.h"

#include "GaussianBlurCPU.h"
#include "TVDenoisingCPU.h"
#include "SobelEdgeDetectionCPU.h"
#include "CannyEdgeDetectionCPU.h"
#include "HoughLineShapeDetectionCPU.h"
#include "HoughCircleShapeDetectionCPU.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>

namespace test {
    static constexpr float tolerance{ 1.0f / 255.0f };

    static const std::filesystem::path reference_image_dir{ REFERENCE_IMAGES_DIR };

    static std::vector<std::string> findImageSets() {
        std::vector<std::string> sets;
        if (!std::filesystem::exists(test::reference_image_dir)) {
            return sets;
        }
        for (const auto& entry : std::filesystem::directory_iterator(test::reference_image_dir)) {
            if (entry.is_directory() && std::filesystem::exists(entry.path() / "input.png")) {
                sets.push_back(entry.path().filename().string());
            }
        }
        return sets;
    }

    static void compareImages(const std::string& label, const Image& actual, const Image& expected) {
        ASSERT_EQ(actual.getRows(), expected.getRows()) << label << ": row count mismatch";
        ASSERT_EQ(actual.getCols(), expected.getCols()) << label << ": column count mismatch";

        for (PixelIdx i{ 0U }; i < actual.getRows(); ++i) {
            for (PixelIdx j{ 0U }; j < actual.getCols(); ++j) {
                EXPECT_NEAR(actual(i, j), expected(i, j), test::tolerance) << label << " at row = " << i << " col = " << j;
            }
        }
    }

    static void checkOrGenerateReference(const std::string& set_name, const Image& out_image, const std::string& component_name) {
        const std::filesystem::path ref_path{ test::reference_image_dir / set_name / (component_name + ".png") };

        if (!std::filesystem::exists(ref_path)) {
            const std::string base{ (test::reference_image_dir / set_name / component_name).generic_string() };
            ASSERT_TRUE(out_image.save(base, ".png")) << "Failed to write reference image: " << ref_path;
            GTEST_SKIP() << "Generated new reference image: " << ref_path << "  - re-run the test suite to enable comparison.";
            return;
        }

        const Image ref{ ref_path.string() };
        const std::string label{ set_name + "/" + component_name };
        test::compareImages(label, out_image, ref);
    }

    static std::string gauss_cpu_filename{ "gauss-cpu" };
    static std::string tv_cpu_filename{ "tv-cpu" };
    static std::string sobel_cpu_filename{ "sobel-cpu" };
    static std::string canny_cpu_filename{ "canny-cpu" };
    static std::string hough_line_cpu_filename{ "hough-line-cpu" };
    static std::string hough_circle_cpu_filename{ "hough-circle-cpu" };

} // namespace test

TEST(ComponentRegressionTest, GaussianBlurCPU) {
    const std::vector<std::string> sets = test::findImageSets();
    if (sets.empty()) {
        GTEST_SKIP() << "No reference image sets found in " << test::reference_image_dir;
    }

    for (const std::string& set_name : sets) {
        SCOPED_TRACE("image set: " + set_name);

        Image input{ (test::reference_image_dir / set_name / "input.png").string() };

        components::denoising::GaussianBlurCPU component{};
        components::Context ctx{ input };
        component.process(ctx);

        test::checkOrGenerateReference(set_name, ctx.getProcessedImage(), test::gauss_cpu_filename);
    }
}

TEST(ComponentRegressionTest, TVDenoisingCPU) {
    const std::vector<std::string> sets = test::findImageSets();
    if (sets.empty()) {
        GTEST_SKIP() << "No reference image sets found in " << test::reference_image_dir;
    }

    for (const std::string& set_name : sets) {
        SCOPED_TRACE("image set: " + set_name);

        Image input{ (test::reference_image_dir / set_name / "input.png").string() };

        components::denoising::TVDenoisingCPU component{};
        components::Context ctx{ input };
        component.process(ctx);

        test::checkOrGenerateReference(set_name, ctx.getProcessedImage(), test::tv_cpu_filename);
    }
}

TEST(ComponentRegressionTest, SobelEdgeDetectionCPU) {
    const std::vector<std::string> sets = test::findImageSets();
    if (sets.empty()) {
        GTEST_SKIP() << "No reference image sets found in " << test::reference_image_dir;
    }

    for (const std::string& set_name : sets) {
        SCOPED_TRACE("image set: " + set_name);

        Image input{ (test::reference_image_dir / set_name / "input.png").string() };

        components::Context ctx{ input };

        if (std::filesystem::exists(test::reference_image_dir / set_name / (test::tv_cpu_filename + ".png"))) {
            Image tv_result{ (test::reference_image_dir / set_name / (test::tv_cpu_filename + ".png")).string() };
            ctx.getProcessedImage() = tv_result;
        }
        else {
            components::denoising::TVDenoisingCPU tv{};
            tv.process(ctx);
        }

        components::edge_detection::SobelEdgeDetectionCPU sobel{};
        sobel.process(ctx);

        test::checkOrGenerateReference(set_name, ctx.getEdgeMap(), test::sobel_cpu_filename);
    }
}

TEST(ComponentRegressionTest, CannyEdgeDetectionCPU) {
    const std::vector<std::string> sets = test::findImageSets();
    if (sets.empty()) {
        GTEST_SKIP() << "No reference image sets found in " << test::reference_image_dir;
    }

    for (const std::string& set_name : sets) {
        SCOPED_TRACE("image set: " + set_name);

        Image input{ (test::reference_image_dir / set_name / "input.png").string() };

        components::Context ctx{ input };

        if (std::filesystem::exists(test::reference_image_dir / set_name / (test::tv_cpu_filename + ".png"))) {
            Image tv_result{ (test::reference_image_dir / set_name / (test::tv_cpu_filename + ".png")).string() };
            ctx.getProcessedImage() = tv_result;
        }
        else {
            components::denoising::TVDenoisingCPU tv{};
            tv.process(ctx);
        }

        components::edge_detection::CannyEdgeDetectionCPU canny{};
        canny.process(ctx);

        test::checkOrGenerateReference(set_name, ctx.getEdgeMap(), test::canny_cpu_filename);
    }
}

TEST(ComponentRegressionTest, HoughLineShapeDetectionCPU) {
    const std::vector<std::string> sets = test::findImageSets();
    if (sets.empty()) {
        GTEST_SKIP() << "No reference image sets found in " << test::reference_image_dir;
    }

    for (const std::string& set_name : sets) {
        SCOPED_TRACE("image set: " + set_name);

        Image input{ (test::reference_image_dir / set_name / "input.png").string() };

        components::Context ctx{ input };

        if (std::filesystem::exists(test::reference_image_dir / set_name / (test::sobel_cpu_filename + ".png"))) {
            Image sobel_result{ (test::reference_image_dir / set_name / (test::sobel_cpu_filename + ".png")).string() };
            ctx.getEdgeMap() = sobel_result;
        }
        else if (std::filesystem::exists(test::reference_image_dir / set_name / (test::tv_cpu_filename + ".png"))) {
            Image tv_result{ (test::reference_image_dir / set_name / (test::tv_cpu_filename + ".png")).string() };
            ctx.getProcessedImage() = tv_result;

            components::edge_detection::SobelEdgeDetectionCPU sobel{};
            sobel.process(ctx);
        }
        else {
            components::denoising::TVDenoisingCPU tv{};
            tv.process(ctx);

            components::edge_detection::SobelEdgeDetectionCPU sobel{};
            sobel.process(ctx);
        }


        components::shape_detection::HoughLineShapeDetectionCPU hough{};
        hough.process(ctx);

        test::checkOrGenerateReference(set_name, ctx.getShapeMap(), test::hough_line_cpu_filename);
    }
}

TEST(ComponentRegressionTest, HoughCircleShapeDetectionCPU) {
    const std::vector<std::string> sets = test::findImageSets();
    if (sets.empty()) {
        GTEST_SKIP() << "No reference image sets found in " << test::reference_image_dir;
    }

    for (const std::string& set_name : sets) {
        SCOPED_TRACE("image set: " + set_name);

        Image input{ (test::reference_image_dir / set_name / "input.png").string() };

        components::Context ctx{ input };

        if (std::filesystem::exists(test::reference_image_dir / set_name / (test::sobel_cpu_filename + ".png"))) {
            Image sobel_result{ (test::reference_image_dir / set_name / (test::sobel_cpu_filename + ".png")).string() };
            ctx.getEdgeMap() = sobel_result;
        }
        else if (std::filesystem::exists(test::reference_image_dir / set_name / (test::tv_cpu_filename + ".png"))) {
            Image tv_result{ (test::reference_image_dir / set_name / (test::tv_cpu_filename + ".png")).string() };
            ctx.getProcessedImage() = tv_result;

            components::edge_detection::SobelEdgeDetectionCPU sobel{};
            sobel.process(ctx);
        }
        else {
            components::denoising::TVDenoisingCPU tv{};
            tv.process(ctx);

            components::edge_detection::SobelEdgeDetectionCPU sobel{};
            sobel.process(ctx);
        }

        components::shape_detection::HoughCircleShapeDetectionCPU hough{};
        hough.process(ctx);

        test::checkOrGenerateReference(set_name, ctx.getShapeMap(), test::hough_circle_cpu_filename);
    }
}
