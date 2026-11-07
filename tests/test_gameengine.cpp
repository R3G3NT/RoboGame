#include "Engine/GameEngine.h"
#include "Robot/Robot.h"
#include "Engine/LevelLoader.h"
#include <gtest/gtest.h>

TEST(GameEngine, SpawnRobotOnEmptyCell) {
    GameEngine engine(20, 10);
    auto robot = std::make_shared<Robot>(
        "P", Robot::Type::Player, 100, 20, 60, 2);

    Robot* r = robot.get();
    engine.SpawnRobot(std::move(robot), 5, 5);

    EXPECT_TRUE(engine.field.GetCell(5, 5).HasRobot());
    EXPECT_EQ(engine.field.GetCell(5, 5).GetRobot(), r);
}

TEST(GameEngine, SpawnRobotOnWallDoesNothing) {
    GameEngine engine(20, 10);
    engine.field.GetCell(5, 5).SetTerrain(Cell::Terrain::Wall);

    auto robot = std::make_shared<Robot>(
        "P", Robot::Type::Player, 100, 20, 60, 2);
    engine.SpawnRobot(std::move(robot), 5, 5);

    EXPECT_FALSE(engine.field.GetCell(5, 5).HasRobot());
}

TEST(GameEngine, MoveRobotMovesPlayer) {
    GameEngine engine(20, 10);
    auto player = std::make_shared<Robot>(
        "P", Robot::Type::Player, 100, 20, 60, 1);
    Robot* r = player.get();
    engine.SpawnRobot(std::move(player), 5, 5);

    engine.MoveRobot(5, 5, 1, 0);

    EXPECT_EQ(r->x, 6);
    EXPECT_EQ(r->y, 5);
    EXPECT_TRUE(engine.field.GetCell(6, 5).HasRobot());
    EXPECT_FALSE(engine.field.GetCell(5, 5).HasRobot());
}

TEST(GameEngine, MoveRobotStopsAtWall) {
    GameEngine engine(20, 10);
    engine.field.GetCell(7, 5).SetTerrain(Cell::Terrain::Wall);

    auto player = std::make_shared<Robot>(
        "P", Robot::Type::Player, 100, 20, 60, 2);
    Robot* r = player.get();
    engine.SpawnRobot(std::move(player), 5, 5);

    engine.MoveRobot(5, 5, 1, 0);

    EXPECT_EQ(r->x, 6);   // прошёл только одну клетку и врезался в стену
}

TEST(GameEngine, MoveRobotAttacksEnemy) {
    GameEngine engine(20, 10);

    auto player = std::make_shared<Robot>(
        "P", Robot::Type::Player, 100, 20, 60, 1);
    engine.SpawnRobot(std::move(player), 6, 5);

    auto enemy = std::make_shared<Robot>(
        "E", Robot::Type::Enemy, 50, 10, 30, 1);
    Robot* e = enemy.get();
    engine.SpawnRobot(std::move(enemy), 7, 5);

    engine.MoveRobot(6, 5, 1, 0);

    EXPECT_EQ(e->GetHp(), 30);   // 50 - 20 от атаки
}