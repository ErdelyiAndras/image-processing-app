#include <benchmark/benchmark.h>

#include "Context.h"
#include "Image.h"

#include "GaussianBlurCPU.h"
#include "GaussianBlurGPU.h"
#include "TVDenoisingCPU.h"
#include "TVDenoisingGPU.h"
#include "SobelEdgeDetectionCPU.h"
#include "SobelEdgeDetectionGPU.h"
#include "CannyEdgeDetectionCPU.h"
#include "CannyEdgeDetectionGPU.h"
#include "HoughLineShapeDetectionCPU.h"
#include "HoughLineShapeDetectionGPU.h"
#include "HoughCircleShapeDetectionCPU.h"
#include "HoughCircleShapeDetectionGPU.h"

#include <algorithm>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

using namespace components;
using namespace components::denoising;
using namespace components::edge_detection;
using namespace components::shape_detection;

namespace benchmark {
    static std::filesystem::path image_dir{ BENCHMARK_IMAGES_DIR };

    struct ImageEntry {
        std::string name;
        std::filesystem::path path;
    };

    std::vector<ImageEntry> discoverImages() {
        std::vector<ImageEntry> result;
        for (const auto& entry : std::filesystem::directory_iterator(image_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".png") {
                result.push_back({ entry.path().stem().string(), entry.path() });
            }
        }
        std::sort(result.begin(), result.end(),
            [](const ImageEntry& a, const ImageEntry& b) { return a.name < b.name; });
        return result;
    }

    void setMetrics(::benchmark::State& state, const Image& img) {
        state.SetBytesProcessed(
            static_cast<int64_t>(state.iterations()) *
            static_cast<int64_t>(img.getRows()) * img.getCols() * sizeof(float)
        );
        state.SetLabel(
            std::to_string(img.getCols()) + "x" + std::to_string(img.getRows())
        );
    }

    Image prepareDenoised(const Image& img) {
        Image copy{ img };
        Context ctx{ copy };
        GaussianBlurCPU gauss;
        gauss.process(ctx);
        return ctx.getProcessedImage();
    }

    Image prepareEdgeMap(const Image& img) {
        Image denoised{ prepareDenoised(img) };
        Context ctx{ denoised };
        SobelEdgeDetectionCPU sobel;
        sobel.process(ctx);
        return ctx.getEdgeMap();
    }

    template<typename T>
    bool tryMakeGPU(::benchmark::State& state, std::unique_ptr<T>& out) {
        try {
            out = std::make_unique<T>();
            return true;
        } catch (const std::runtime_error& e) {
            state.SkipWithMessage(e.what());
            return false;
        }
    }

    void registerGaussianBlurBenchmarks(const std::vector<ImageEntry>& images) {
        for (const auto& img_entry : images) {
            ::benchmark::RegisterBenchmark(
                ("BM_GaussianBlur_CPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    GaussianBlurCPU comp;
                    for (auto _ : state) {
                        Context ctx{ img };
                        comp.process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);

            ::benchmark::RegisterBenchmark(
                ("BM_GaussianBlur_GPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    std::unique_ptr<GaussianBlurGPU> comp;
                    if (!tryMakeGPU(state, comp)) {
                        return;
                    }
                    for (auto _ : state) {
                        Context ctx{ img };
                        comp->process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);
        }
    }

    void registerTVDenoisingBenchmarks(const std::vector<ImageEntry>& images) {
        for (const auto& img_entry : images) {
            ::benchmark::RegisterBenchmark(
                ("BM_TVDenoising_CPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    TVDenoisingCPU comp;
                    for (auto _ : state) {
                        Context ctx{ img };
                        comp.process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);

            ::benchmark::RegisterBenchmark(
                ("BM_TVDenoising_GPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    std::unique_ptr<TVDenoisingGPU> comp;
                    if (!tryMakeGPU(state, comp)) {
                        return;
                    }
                    for (auto _ : state) {
                        Context ctx{ img };
                        comp->process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);
        }
    }

    void registerSobelBenchmarks(const std::vector<ImageEntry>& images) {
        for (const auto& img_entry : images) {
            ::benchmark::RegisterBenchmark(
                ("BM_Sobel_CPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    Image denoised{ prepareDenoised(img) };
                    SobelEdgeDetectionCPU comp;
                    for (auto _ : state) {
                        Context ctx{ denoised };
                        comp.process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);

            ::benchmark::RegisterBenchmark(
                ("BM_Sobel_GPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    Image denoised{ prepareDenoised(img) };
                    std::unique_ptr<SobelEdgeDetectionGPU> comp;
                    if (!tryMakeGPU(state, comp)) {
                        return;
                    }
                    for (auto _ : state) {
                        Context ctx{ denoised };
                        comp->process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);
        }
    }

    void registerCannyBenchmarks(const std::vector<ImageEntry>& images) {
        for (const auto& img_entry : images) {
            ::benchmark::RegisterBenchmark(
                ("BM_Canny_CPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    Image denoised{ prepareDenoised(img) };
                    CannyEdgeDetectionCPU comp;
                    for (auto _ : state) {
                        Context ctx{ denoised };
                        comp.process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);

            ::benchmark::RegisterBenchmark(
                ("BM_Canny_GPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    Image denoised{ prepareDenoised(img) };
                    std::unique_ptr<CannyEdgeDetectionGPU> comp;
                    if (!tryMakeGPU(state, comp)) {
                        return;
                    }
                    for (auto _ : state) {
                        Context ctx{ denoised };
                        comp->process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);
        }
    }

    void registerHoughLineBenchmarks(const std::vector<ImageEntry>& images) {
        for (const auto& img_entry : images) {
            ::benchmark::RegisterBenchmark(
                ("BM_HoughLine_CPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    Image edgeMap{ prepareEdgeMap(img) };
                    HoughLineShapeDetectionCPU comp;
                    for (auto _ : state) {
                        Context ctx{ img };
                        ctx.getEdgeMap() = edgeMap;
                        comp.process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);

            ::benchmark::RegisterBenchmark(
                ("BM_HoughLine_GPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    Image edgeMap{ prepareEdgeMap(img) };
                    std::unique_ptr<HoughLineShapeDetectionGPU> comp;
                    if (!tryMakeGPU(state, comp)) {
                        return;
                    }
                    for (auto _ : state) {
                        Context ctx{ img };
                        ctx.getEdgeMap() = edgeMap;
                        comp->process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);
        }
    }

    void registerHoughCircleBenchmarks(const std::vector<ImageEntry>& images) {
        for (const auto& img_entry : images) {
            ::benchmark::RegisterBenchmark(
                ("BM_HoughCircle_CPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    Image edgeMap{ prepareEdgeMap(img) };
                    HoughCircleShapeDetectionCPU comp;
                    for (auto _ : state) {
                        Context ctx{ img };
                        ctx.getEdgeMap() = edgeMap;
                        comp.process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);

            ::benchmark::RegisterBenchmark(
                ("BM_HoughCircle_GPU/" + img_entry.name).c_str(),
                [p = img_entry.path](::benchmark::State& state) {
                    Image img{ p.string() };
                    Image edgeMap{ prepareEdgeMap(img) };
                    std::unique_ptr<HoughCircleShapeDetectionGPU> comp;
                    if (!tryMakeGPU(state, comp)) {
                        return;
                    }
                    for (auto _ : state) {
                        Context ctx{ img };
                        ctx.getEdgeMap() = edgeMap;
                        comp->process(ctx);
                    }
                    setMetrics(state, img);
                }
            )->Unit(::benchmark::kMillisecond);
        }
    }
} // namespace benchmark

int main(int argc, char** argv) {
    std::vector<char*> fwd;
    fwd.reserve(static_cast<size_t>(argc));
    for (int i{ 0 }; i < argc; ++i) {
        const std::string_view arg{ argv[i] };
        static constexpr std::string_view flag{ "--images-dir=" };
        if (arg.find(flag) == 0) {
            benchmark::image_dir = arg.substr(flag.size());
        } else {
            fwd.push_back(argv[i]);
        }
    }

    const std::vector<benchmark::ImageEntry> images = benchmark::discoverImages();

    benchmark::registerGaussianBlurBenchmarks(images);
    benchmark::registerTVDenoisingBenchmarks(images);
    benchmark::registerSobelBenchmarks(images);
    benchmark::registerCannyBenchmarks(images);
    benchmark::registerHoughLineBenchmarks(images);
    benchmark::registerHoughCircleBenchmarks(images);

    int fwd_argc{ static_cast<int>(fwd.size()) };
    ::benchmark::Initialize(&fwd_argc, fwd.data());
    if (::benchmark::ReportUnrecognizedArguments(fwd_argc, fwd.data())) {
        return 1;
    }
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}
