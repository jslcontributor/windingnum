#include <gtest/gtest.h>
#include <poly_io.hpp>

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <tuple>

namespace poly {

class PolygonTest : public ::testing::Test {
protected:
    PolygonTest() :
            reader_(IPolygonReader::Create()),
            polygons_file_path_((std::filesystem::current_path() / "polygons.txt").string()) {}

    std::unique_ptr<IPolygonReader> reader_;
    const std::string polygons_file_path_;
};

TEST_F(PolygonTest, CanMakePolygon) {
    Polygon polygon = Polygon();

    polygon.AppendPoint(0.0, 0.0);
    polygon.AppendPoint(1.0, 0.0);
    polygon.AppendPoint(1.0, 1.0);
    polygon.AppendPoint(0.0, 1.0);
    polygon.AppendPoint(0.0, 0.0);

    EXPECT_TRUE(polygon.IsClosed());
}

TEST_F(PolygonTest, CanMakePolygonFromString) {
    std::string polygon_string = "4.0 5.0 0.0 0.0 1.0 0.0 1.0 1.0 0.0 1.0 0.0 0.0";
    auto point_and_polygon = reader_->CreatePointAndPolygonFromString(polygon_string);
    const auto& polygon = std::get<2>(point_and_polygon);
    EXPECT_TRUE(polygon.IsClosed());
}

TEST_F(PolygonTest, CanMakePolygonFromStringWithManyDelimiters) {
    std::string polygon_string = "4.0  5.0  0.0 0.0   1.0  0.0 1.0  1.0  0.0 1.0 0.0 0.0  ";
    auto point_and_polygon = reader_->CreatePointAndPolygonFromString(polygon_string);
    const auto& polygon = std::get<2>(point_and_polygon);
    EXPECT_TRUE(polygon.IsClosed());
}

TEST_F(PolygonTest, FailToMakePolygonFromStringWithBadCharacters) {
    std::string polygon_string = "0.0 0.0 1.0 0.0 1.0 I_Am_Not_A_float 1.0 0.0 1.0 0.0";
    EXPECT_THROW(auto polygon = reader_->CreatePointAndPolygonFromString(polygon_string), std::runtime_error);
}

TEST_F(PolygonTest, CanReadPolygonsFromFile) {
    auto polygons = reader_->ReadPointsAndPolygonsFromFile(polygons_file_path_);
    EXPECT_FALSE(polygons.empty());
}

TEST_F(PolygonTest, FailToMakePolgyonFromStringWithMismatchedXYVectors) {
    std::string polygon_string = "4.0 5.0 0.0 0.0 1.0 0.0 1.0 1.0 0.0 1.0 0.0 0.0 3.0";
    EXPECT_THROW(auto polygon = reader_->CreatePointAndPolygonFromString(polygon_string), std::runtime_error);
}

}  // namespace poly
