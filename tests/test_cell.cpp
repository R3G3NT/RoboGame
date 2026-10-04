#include "Field/Cell.h"
#include "Robot/Robot.h"
#include <gtest/gtest.h>

TEST(Cell, DefaultIsFloorAndWalkable) {
    Cell c(Terrain::Floor);

    EXPECT_EQ(c.GetTerrain(), Terrain::Floor);
    EXPECT_TRUE(c.IsWalkable());
    EXPECT_FALSE(c.HasRobot());
    EXPECT_FALSE(c.IsVisible());
}

TEST(Cell, WallIsNotWalkable) {
    Cell c(Terrain::Wall);
    EXPECT_FALSE(c.IsWalkable());
}

TEST(Cell, RobotBlocksWalkable) {
    Cell c(Terrain::Floor);
    Robot r("A", Robot::Type::Player, 100, 10, 10, 1);

    c.SetRobot(&r);
    EXPECT_TRUE(c.HasRobot());
    EXPECT_FALSE(c.IsWalkable());       // робот блокирует проход

    c.RemoveRobot();
    EXPECT_FALSE(c.HasRobot());
    EXPECT_TRUE(c.IsWalkable());
}

TEST(Cell, VisibilityCanBeChanged) {
    Cell c(Terrain::Floor);

    c.SetVisible(true);
    EXPECT_TRUE(c.IsVisible());

    c.SetVisible(false);
    EXPECT_FALSE(c.IsVisible());
}