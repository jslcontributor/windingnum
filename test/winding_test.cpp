/*
 * Justin Lee
 * jsL118@ucsd.edu
 */

#include <gtest/gtest.h>

#include <filesystem>  // A C++17 capable compiler is assumed here.
#include <optional>
#include <string>
#include <tuple>

#include <winding.hpp>
#include <poly_io.hpp>

namespace winding_number {

using poly::Polygon;

class WindingNumberTest : public ::testing::Test {
protected:
    WindingNumberTest() :
            reader_(poly::IPolygonReader::Create()),
            algorithm_(IWindingNumberAlgorithm::Create()),
            polygons_file_path_((std::filesystem::current_path() / "polygons.txt").string()),
            tolerance_(1e-6f) {
        algorithm_->tolerance(tolerance_);
    }

    std::unique_ptr<poly::IPolygonReader> reader_;
    std::unique_ptr<IWindingNumberAlgorithm> algorithm_;
    const std::string polygons_file_path_;
    const float tolerance_;
};

TEST_F(WindingNumberTest, FailsWithUnclosedPolygon) {
    Polygon p;
    p.AppendPoint(0.0, 0.0);
    p.AppendPoint(1.0, 0.0);
    p.AppendPoint(1.0, 1.0);
    EXPECT_FALSE(p.IsClosed());
    auto winding_algo = IWindingNumberAlgorithm::Create();
    auto winding_num = winding_algo->CalculateWindingNumber2D(0.0, 0.0, std::move(p));
    // Either this is due to an unimplemented algorithm or it should be std::nullopt -- as it does not make sense to
    // make this call with unclosed polygons.
    EXPECT_FALSE(winding_num);
}

TEST_F(WindingNumberTest, CanGetPointInPolygon) {
    Polygon p;
    p.AppendPoint(0.0, 0.0);
    p.AppendPoint(1.0, 0.0);
    p.AppendPoint(1.0, 1.0);
    p.AppendPoint(0.0, 1.0);
    p.AppendPoint(0.0, 0.0);
    EXPECT_TRUE(p.IsClosed());
    auto winding_num = algorithm_->CalculateWindingNumber2D(0.0, 0.0, std::move(p));
    ASSERT_TRUE(winding_num);
    EXPECT_EQ(1, *winding_num);
}

// TODO: Add more expected_winding_numbers to this test, based on results from the polygons.txt file.
TEST_F(WindingNumberTest, CanGetWindingNumbersForPolygonsFromFile) {
    std::vector<std::optional<int>> expected_winding_numbers = {1, -1, 1, std::nullopt, 1, 4};
    auto points_and_polygons = reader_->ReadPointsAndPolygonsFromFile(polygons_file_path_);
    ASSERT_GE(points_and_polygons.size(), expected_winding_numbers.size());
    for (size_t i = 0; i < expected_winding_numbers.size(); ++i) {
        const auto& p = points_and_polygons[i];
        if (expected_winding_numbers[i]) {
            // Only a point and a closed polygon should expect any sort of winding number.
            EXPECT_TRUE(std::get<2>(p).IsClosed(tolerance_));
        }
        auto winding_num = algorithm_->CalculateWindingNumber2D(std::get<0>(p), std::get<1>(p), std::get<2>(p));
        if (winding_num) {
            ASSERT_TRUE(expected_winding_numbers[i]) << "Have a winding number for expected_winding_number[" << i
                                                     << "], which is expected to be missing.";
            EXPECT_EQ(*expected_winding_numbers[i], *winding_num);
        } else {
            // TODO: Uncomment this once the algorithm is implemented.
             ASSERT_FALSE(expected_winding_numbers[i]) << "Missing a winding number for expected_winding_number[" << i
                                                       << "], which is expected to exist.";
        }
    }
}

TEST_F(WindingNumberTest, WindingCalculationsTests){
    Polygon p;
    p.AppendPoint(5, 0);
    p.AppendPoint(0, 5);
    p.AppendPoint(-5, 0);
    p.AppendPoint(0, -5);
    p.AppendPoint(6, 0);
    p.AppendPoint(0, 6);
    p.AppendPoint(-6, 0);
    p.AppendPoint(0, -6);
    p.AppendPoint(5, 0);
    auto winding_num = algorithm_->CalculateWindingNumber2D(0.0, 0.0, std::move(p));
    ASSERT_TRUE(winding_num);
    EXPECT_EQ(2, *winding_num);
    
    Polygon p1;
    p1.AppendPoint(5, 0);
    p1.AppendPoint(0, 5);
    p1.AppendPoint(-5, 0);
    p1.AppendPoint(0, -5);
    p1.AppendPoint(6, 0);
    p1.AppendPoint(0, 6);
    p1.AppendPoint(-6, 0);
    p1.AppendPoint(0, -6);
    p1.AppendPoint(5, 1);
    p1.AppendPoint(5, 0);
    auto winding_num1 = algorithm_->CalculateWindingNumber2D(0.0, 0.0, std::move(p1));
    ASSERT_TRUE(winding_num1);
    EXPECT_EQ(2, *winding_num1);
   
    
    Polygon p2;
    p2.AppendPoint(-0.5, -0.5);
    p2.AppendPoint(0.5, 0.5);
    p2.AppendPoint(0.0, 1.0);
    p2.AppendPoint(-0.5, -0.5);
    auto winding_num3 = algorithm_->CalculateWindingNumber2D(0.0, 0.0, std::move(p2));
    ASSERT_TRUE(winding_num3);
    EXPECT_EQ(1, *winding_num3);
    
    Polygon p3;
    p3.AppendPoint(-0.5, 0.5);
    p3.AppendPoint(0.5, -0.5);
    p3.AppendPoint(0, -1);
    p3.AppendPoint(-0.5, 0.5);
    auto winding_num4 = algorithm_->CalculateWindingNumber2D(0.0, 0.0, std::move(p3));
    ASSERT_TRUE(winding_num4);
    EXPECT_EQ(1, *winding_num4);




}

// Hint, you will probably also want to add more tests...

}  // namespace winding_number
