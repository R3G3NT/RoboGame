#include "Engine/GameEngine.h"
#include "Robot/Robot.h"
#include <gtest/gtest.h>

TEST(GameEngine, SpawnRobotOnEmptyCell) {
    GameEngine engine(20, 10);
    auto robot = std::make_unique<Robot>(
        "P", Robot::Type::Player, 100, 20, 60, 2);

    Robot* r = robot.get();
    engine.SpawnRobot(std::move(robot), 5, 5);

    EXPECT_TRUE(engine.field.GetCell(5, 5).HasRobot());
    EXPECT_EQ(engine.field.GetCell(5, 5).GetRobot(), r);
}

TEST(GameEngine, SpawnRobotOnWallDoesNothing) {
    GameEngine engine(20, 10);
    engine.field.GetCell(5, 5).SetTerrain(Cell::Terrain::Wall);

    auto robot = std::make_unique<Robot>(
        "P", Robot::Type::Player, 100, 20, 60, 2);
    engine.SpawnRobot(std::move(robot), 5, 5);

    EXPECT_FALSE(engine.field.GetCell(5, 5).HasRobot());
}

TEST(GameEngine, LoadLevelParsesSymbols) {
    std::vector<std::string> level = {
        "##########",
        "#R.....E.#",
        "#...~....#",
        "#F.......#",
        "##########"
    };

    GameEngine engine(10, 5);
    engine.LoadLevel(level);

    EXPECT_EQ(engine.field.GetCell(1, 1).GetRobot()->GetType(),
              Robot::Type::Player);
    EXPECT_EQ(engine.field.GetCell(7, 1).GetRobot()->GetType(),
              Robot::Type::Enemy);
    EXPECT_EQ(engine.field.GetCell(1, 3).GetRobot()->GetType(),
              Robot::Type::Friend);
    EXPECT_EQ(engine.field.GetCell(4, 2).GetTerrain(), Cell::Terrain::Swamp);
}

TEST(GameEngine, MoveRobotMovesPlayer) {
    GameEngine engine(20, 10);
    auto player = std::make_unique<Robot>(
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

    auto player = std::make_unique<Robot>(
        "P", Robot::Type::Player, 100, 20, 60, 2);
    Robot* r = player.get();
    engine.SpawnRobot(std::move(player), 5, 5);

    engine.MoveRobot(5, 5, 1, 0);

    EXPECT_EQ(r->x, 6);   // прошёл только одну клетку и врезался в стену
}

TEST(GameEngine, MoveRobotAttacksEnemy) {
    GameEngine engine(20, 10);

    auto player = std::make_unique<Robot>(
        "P", Robot::Type::Player, 100, 20, 60, 1);
    engine.SpawnRobot(std::move(player), 6, 5);

    auto enemy = std::make_unique<Robot>(
        "E", Robot::Type::Enemy, 50, 10, 30, 1);
    Robot* e = enemy.get();
    engine.SpawnRobot(std::move(enemy), 7, 5);

    engine.MoveRobot(6, 5, 1, 0);

    EXPECT_EQ(e->GetHp(), 30);   // 50 - 20 от атаки
}

TEST(GameEngine, UpdateVisibilityRevealsAroundPlayer) {
    std::vector<std::string> level = {
        "##########",
        "#........#",
        "#........#",
        "#........#",
        "#........#",
        "#........#",
        "#........#",
        "#...R....#",
        "##########"
    };

    GameEngine engine(10, 9);
    engine.LoadLevel(level);
    engine.UpdateVisibility();

    EXPECT_TRUE(engine.field.GetCell(4, 7).IsVisible());
    EXPECT_FALSE(engine.field.GetCell(8, 1).IsVisible());
}