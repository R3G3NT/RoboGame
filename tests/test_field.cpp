#include "Field/Field.h"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(Field, ValidSize) {
    EXPECT_NO_THROW(Field(20, 10));
    EXPECT_EQ(Field(20, 10).GetWidth(), 20);
    EXPECT_EQ(Field(20, 10).GetHeight(), 10);
}

TEST(Field, InvalidSizeThrows) {
    EXPECT_THROW(Field(5, 10), std::invalid_argument);
    EXPECT_THROW(Field(20, 3), std::invalid_argument); 
}

TEST(Field, CellsStartAsFloor) {
    Field f(20, 10);
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 20; ++x) {
            EXPECT_EQ(f.GetCell(x, y).GetTerrain(), Cell::Terrain::Floor);
        }
    }
}

TEST(Field, WallBlocksWalkable) {
    Field f(20, 10);
    f.GetCell(5, 5).SetTerrain(Cell::Terrain::Wall);

    EXPECT_FALSE(f.IsWalkable(5, 5));
    EXPECT_TRUE(f.IsWalkable(5, 6));
}

TEST(Field, ClearVisibilityResets) {
    Field f(20, 10);
    f.GetCell(3, 3).SetVisible(true);
    f.ClearVisibility();

    EXPECT_FALSE(f.GetCell(3, 3).IsVisible());
}