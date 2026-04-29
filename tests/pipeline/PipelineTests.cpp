#include "CombineEdgeMap.h"
#include "CombineShapeMap.h"
#include "Context.h"
#include "Image.h"
#include "MockComponents.h"
#include "Pipeline.h"
#include "types.h"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <unordered_map>

using namespace pipeline;
using namespace components;

namespace test {
    static Image makeImage(PixelIdx rows, PixelIdx cols, float value = 1.0f) {
        Image img{ rows, cols };
        for (PixelIdx i{ 0U }; i < rows; ++i) {
            for (PixelIdx j{ 0U }; j < cols; ++j) {
                img(i, j) = value;
            }
        }
        return img;
    }

    static void expectAllPixels(const Image& img, float expected) {
        for (PixelIdx i{ 0U }; i < img.getRows(); ++i) {
            for (PixelIdx j{ 0U }; j < img.getCols(); ++j) {
                EXPECT_FLOAT_EQ(img(i, j), expected) << "at row = " << i << " col = " << j;
            }
        }
    }
} // namespace test

TEST(EmptyPipeline, IsValid) {
    Pipeline p;
    EXPECT_TRUE(p.validate());
}

TEST(EmptyPipeline, ExecuteReturnsEmptyMap) {
    Pipeline p;
    Image img{ test::makeImage(4, 4) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };
    EXPECT_TRUE(results.empty());
}


TEST(SingleComponent, DenoisingFillsProcessedImage) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.0f)) };

    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 1U);
    test::expectAllPixels(results.at(n0).getProcessedImage(), 0.0f);
}

TEST(SingleComponent, EdgeDetectionFillsEdgeMap) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.5f)) };

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 1U);
    test::expectAllPixels(results.at(n0).getEdgeMap(), 0.5f);
}

TEST(SingleComponent, ShapeDetectionFillsShapeMap) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockShapeDetectionComponent>(0.25f)) };

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 1U);
    test::expectAllPixels(results.at(n0).getShapeMap(), 0.25f);
}


TEST(LinearPipeline, ThreeComponentsAllChannelsSet) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.5f)) };
    const NodeId n1{ p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.7f)) };
    const NodeId n2{ p.addNode(std::make_unique<test::MockShapeDetectionComponent>(0.3f)) };
    p.connect(n0, n1);
    p.connect(n1, n2);

    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 1U);
    const Context& out{ results.at(n2) };
    test::expectAllPixels(out.getProcessedImage(), 0.5f);
    test::expectAllPixels(out.getEdgeMap(),        0.7f);
    test::expectAllPixels(out.getShapeMap(),       0.3f);
}

TEST(LinearPipeline, LastWriterWinsForProcessedImage) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.1f)) };
    const NodeId n1{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.2f)) };
    const NodeId n2{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.4f)) };
    p.connect(n0, n1);
    p.connect(n1, n2);

    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 1U);
    test::expectAllPixels(results.at(n2).getProcessedImage(), 0.4f);
}


TEST(FanOut, TwoIndependentBranches) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.5f)) };
    const NodeId n1{ p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.3f)) };
    const NodeId n2{ p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.7f)) };
    p.connect(n0, n1);
    p.connect(n0, n2);

    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 2U);

    test::expectAllPixels(results.at(n1).getProcessedImage(), 0.5f);
    test::expectAllPixels(results.at(n2).getProcessedImage(), 0.5f);

    test::expectAllPixels(results.at(n1).getEdgeMap(), 0.3f);
    test::expectAllPixels(results.at(n2).getEdgeMap(), 0.7f);
}

TEST(FanOut, ThreeBranches) {
    Pipeline p;
    const NodeId src{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.0f)) };
    const NodeId n1 { p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.2f)) };
    const NodeId n2 { p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.5f)) };
    const NodeId n3 { p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.9f)) };
    p.connect(src, n1);
    p.connect(src, n2);
    p.connect(src, n3);

    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 3U);
    test::expectAllPixels(results.at(n1).getEdgeMap(), 0.2f);
    test::expectAllPixels(results.at(n2).getEdgeMap(), 0.5f);
    test::expectAllPixels(results.at(n3).getEdgeMap(), 0.9f);
}


TEST(MergeNode, CombineEdgeMapsPicksMaximum) {
    Pipeline p;
    const NodeId src  { p.addNode(std::make_unique<test::MockDenoisingComponent>(0.0f)) };
    const NodeId n1   { p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.3f)) };
    const NodeId n2   { p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.7f)) };
    const NodeId merge{ p.addNode(std::make_unique<CombineEdgeMap>()) };
    const NodeId shape{ p.addNode(std::make_unique<test::MockShapeDetectionComponent>(0.5f)) };
    p.connect(src,   n1);
    p.connect(src,   n2);
    p.connect(n1,    merge);
    p.connect(n2,    merge);
    p.connect(merge, shape);

    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 1U);
    const Context& out{ results.at(shape) };
    test::expectAllPixels(out.getEdgeMap(),  0.7f);
    test::expectAllPixels(out.getShapeMap(), 0.5f);
}

TEST(MergeNode, CombineShapeMapsPicksMaximum) {
    Pipeline p;
    const NodeId src  { p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.0f)) };
    const NodeId n1   { p.addNode(std::make_unique<test::MockShapeDetectionComponent>(0.2f)) };
    const NodeId n2   { p.addNode(std::make_unique<test::MockShapeDetectionComponent>(0.8f)) };
    const NodeId merge{ p.addNode(std::make_unique<CombineShapeMap>()) };
    p.connect(src, n1);
    p.connect(src, n2);
    p.connect(n1,  merge);
    p.connect(n2,  merge);

    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 1U);
    test::expectAllPixels(results.at(merge).getShapeMap(), 0.8f);
}


TEST(DisconnectedPipeline, BothComponentsRun) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.3f)) };
    const NodeId n1{ p.addNode(std::make_unique<test::MockEdgeDetectionComponent>(0.6f)) };

    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 2U);
    test::expectAllPixels(results.at(n0).getProcessedImage(), 0.3f);
    test::expectAllPixels(results.at(n1).getEdgeMap(),        0.6f);
}


TEST(RemoveNode, RemoveSinkThenRevalidate) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.5f)) };
    const NodeId n1{ p.addNode(std::make_unique<test::MockDenoisingComponent>(0.9f)) };
    p.connect(n0, n1);

    p.removeNode(n1);
    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };

    ASSERT_EQ(results.size(), 1U);
    test::expectAllPixels(results.at(n0).getProcessedImage(), 0.5f);
}

TEST(RemoveNode, RemoveAllNodesLeavesEmptyPipeline) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    const NodeId n1{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    p.connect(n0, n1);

    p.removeNode(n0);
    p.removeNode(n1);
    EXPECT_TRUE(p.validate());

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    std::unordered_map<NodeId, Context> results{ p.execute(std::move(ctx)) };
    EXPECT_TRUE(results.empty());
}


TEST(InvalidPipeline, DirectCycleIsDetected) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    const NodeId n1{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    p.connect(n0, n1);
    p.connect(n1, n0);

    EXPECT_FALSE(p.validate());
}

TEST(InvalidPipeline, ExecuteWithCycleThrows) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    const NodeId n1{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    p.connect(n0, n1);
    p.connect(n1, n0);

    Image img{ test::makeImage(4, 4, 1.0f) };
    Context ctx{ img };
    EXPECT_THROW(p.execute(std::move(ctx)), std::runtime_error);
}

TEST(InvalidPipeline, MergeNodeWithNoPredecessorsIsInvalid) {
    Pipeline p;
    p.addNode(std::make_unique<CombineEdgeMap>());

    EXPECT_FALSE(p.validate());
}

TEST(InvalidPipeline, MergeNodeWithSinglePredecessorIsInvalid) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    const NodeId m0{ p.addNode(std::make_unique<CombineEdgeMap>()) };
    p.connect(n0, m0);

    EXPECT_FALSE(p.validate());
}


TEST(ConnectErrors, DuplicateConnectionThrows) {
    Pipeline p;
    const NodeId n0{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    const NodeId n1{ p.addNode(std::make_unique<test::MockEdgeDetectionComponent>()) };
    p.connect(n0, n1);

    EXPECT_THROW(p.connect(n0, n1), std::invalid_argument);
}

TEST(ConnectErrors, ProcessingNodeWithTwoPredecessorsThrows) {
    Pipeline p;
    const NodeId src1{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    const NodeId src2{ p.addNode(std::make_unique<test::MockDenoisingComponent>()) };
    const NodeId dst { p.addNode(std::make_unique<test::MockEdgeDetectionComponent>()) };
    p.connect(src1, dst);

    EXPECT_THROW(p.connect(src2, dst), std::invalid_argument);
}
