#include "Robot/Robot.h"
#include "Robot/Statuses.h"
#include <gtest/gtest.h>

TEST(Status, ShieldAbsorbsDamageAndExpires) {
    Robot player("Optimus", Robot::Type::Player, 100, 20, 60, 2);
    
    // Накладываем щит прочностью 30 единиц
    player.ApplyStatus(std::make_shared<ShieldStatus>(30));
    
    // Сценарий А: урон меньше прочности щита
    player.TakeDamage(20);
    EXPECT_EQ(player.GetHp(), 100); // HP не изменилось
    
    // Сценарий Б: урон пробивает остаток щита (10)
    // 30 входящего урона - 10 остатка щита = 20 урона должно пойти в HP
    player.TakeDamage(30);
    EXPECT_EQ(player.GetHp(), 80); // 100 - 20 = 80 HP
}

TEST(Status, ShieldStackingIncreasesPoints) {
    Robot player("Optimus", Robot::Type::Player, 100, 20, 60, 2);
    
    player.ApplyStatus(std::make_shared<ShieldStatus>(20));
    player.ApplyStatus(std::make_shared<ShieldStatus>(30)); // Должны стакнуться в 50
    
    player.TakeDamage(40);
    EXPECT_EQ(player.GetHp(), 100); // Поглощено щитом целиком
}

TEST(Status, SlowBlocksMovementAndStacks) {
    Robot player("Optimus", Robot::Type::Player, 100, 20, 60, 2);
    
    // По умолчанию SlowStatus длится 2 хода (текущий + следующий)
    player.ApplyStatus(std::make_shared<SlowStatus>());
    EXPECT_FALSE(player.CanMove());
    
    player.UpdateStatusesAtTurnEnd(); // Списываем 1 ход
    EXPECT_FALSE(player.CanMove());   // Всё еще замедлен
    
    player.UpdateStatusesAtTurnEnd(); // Списываем 2 ход
    EXPECT_TRUE(player.CanMove());    // Свободен
}

TEST(Status, OverloadBlocksAbilitiesOnce) {
    Robot player("Optimus", Robot::Type::Player, 100, 20, 60, 2);
    
    player.ApplyStatus(std::make_shared<OverloadStatus>());
    
    // Первая попытка: кастовать нельзя
    EXPECT_FALSE(player.CanUseAbilities());
    
    // Вторая попытка на этом же ходу: статус самоликвидировался
    EXPECT_TRUE(player.CanUseAbilities());
}

TEST(Status, BurnDamageAtTurnStart) {
    Robot enemy("Enemy", Robot::Type::Enemy, 50, 10, 30, 1);
    
    enemy.ApplyStatus(std::make_shared<BurnStatus>(2)); // Горение на 2 хода
    
    enemy.UpdateStatusesAtTurnStart(); // -5 HP от горения
    EXPECT_EQ(enemy.GetHp(), 45);
    enemy.UpdateStatusesAtTurnEnd();
    
    enemy.UpdateStatusesAtTurnStart(); // Еще -5 HP
    EXPECT_EQ(enemy.GetHp(), 40);
}