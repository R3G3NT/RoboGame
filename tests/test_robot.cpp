#include "Robot/Robot.h"
#include <gtest/gtest.h>

TEST(Robot, ConstructorSetsValues) {
    Robot r("Optimus", Robot::Type::Player, 100, 20, 60, 2);

    EXPECT_EQ(r.GetName(), "Optimus");
    EXPECT_EQ(r.GetType(), Robot::Type::Player);
    EXPECT_EQ(r.GetHp(), 100);
    EXPECT_EQ(r.GetAttackPower(), 20);
    EXPECT_EQ(r.GetEnergy(), 60);
    EXPECT_EQ(r.GetSpeed(), 2);
    EXPECT_EQ(r.GetRank(), 1);
}

TEST(Robot, ChangeHpClampsToRange) {
    Robot r("A", Robot::Type::Player, 100, 10, 10, 1);

    r.ChangeHp(50);       // не превысит максимум
    EXPECT_EQ(r.GetHp(), 100);

    r.ChangeHp(-30);      // обычное уменьшение
    EXPECT_EQ(r.GetHp(), 70);

    r.ChangeHp(-1000);    // не уйдёт в минус
    EXPECT_EQ(r.GetHp(), 0);
}

TEST(Robot, RankUpIncreasesStats) {
    Robot r("A", Robot::Type::Player, 100, 20, 50, 1);

    r.AddExp(50);

    EXPECT_EQ(r.GetRank(), 2);
    EXPECT_EQ(r.GetMaxHp(), 125);
    EXPECT_EQ(r.GetAttackPower(), 25);
}

TEST(Robot, PlayerAttacksEnemy) {
    Robot player("P", Robot::Type::Player, 100, 20, 60, 1);
    Robot enemy ("E", Robot::Type::Enemy,  50, 10, 30, 1);

    player.Interact(&enemy);

    EXPECT_EQ(enemy.GetHp(), 30);       // 50 - 20
    EXPECT_EQ(player.GetExp(), 5);      // получил опыт
}

TEST(Robot, PlayerNotAttacksWithoutEnergy) {
    Robot player("P", Robot::Type::Player, 100, 20, 0, 1);
    Robot enemy ("E", Robot::Type::Enemy,  50, 10, 30, 1);

    player.Interact(&enemy);

    EXPECT_EQ(enemy.GetHp(), 50);       // без изменений
}