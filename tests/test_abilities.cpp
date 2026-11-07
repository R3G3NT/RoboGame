#include "Robot/Robot.h"
#include "Robot/IAbility.h"
#include "Robot/Abilities.h"
#include "Field/Field.h"
#include <gtest/gtest.h>

TEST(Ability, AreaAttackDamageAndOverloads) {
    Field field(15, 10);
    auto player = std::make_shared<Robot>("P", Robot::Type::Player, 100, 20, 60, 1);
    auto enemy = std::make_shared<Robot>("E", Robot::Type::Enemy, 50, 10, 30, 1);
    
    player->x = 5; player->y = 5;
    enemy->x = 6;  enemy->y = 5; // Враг стоит вплотную справа
    
    field.GetCell(5, 5).SetRobot(player.get());
    field.GetCell(6, 5).SetRobot(enemy.get());
    
    std::shared_ptr<IAbility> area = std::make_shared<AreaAttackAbility>();
    bool success = area->Use(player.get(), field);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(enemy->GetHp(), 20); // 50 - 30 урона
    EXPECT_FALSE(enemy->CanUseAbilities()); // Наложена Перегрузка!
}

TEST(Ability, RangedAttackAppliesBurn) {
    Field field(15, 10);
    auto player = std::make_shared<Robot>("P", Robot::Type::Player, 100, 20, 60, 1);
    auto enemy = std::make_shared<Robot>("E", Robot::Type::Enemy, 50, 10, 30, 1);
    
    player->x = 2; player->y = 2;
    enemy->x = 5;  enemy->y = 2; // Дистанция ровно 3 клетки
    
    field.GetCell(2, 2).SetRobot(player.get());
    field.GetCell(5, 2).SetRobot(enemy.get());
    
    RangedAttackAbility ranged;
    bool success = ranged.Use(player.get(), field, 5, 2);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(enemy->GetHp(), 20); // 50 - 30 урона
    
    enemy->UpdateStatusesAtTurnStart();
    EXPECT_EQ(enemy->GetHp(), 15); // Еще -5 HP от загоревшегося статуса
}

TEST(Ability, HealConvertsOverhealToShield) {
    Field field(15, 10);
    auto player = std::make_shared<Robot>("P", Robot::Type::Player, 100, 20, 60, 1);
    player->ChangeHp(-5); 
    
    std::shared_ptr<IAbility> heal = std::make_shared<HealAbility>();
    // Базовое лечение = 20. 5 уйдет в HP, 15 остатка должно стать щитом
    bool success = heal->Use(player.get(), field, 0, 0);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(player->GetHp(), 100); // Вылечен до максимума
    
    // Проверяем прочность щита: бьем на 15 урона, HP не должно упасть
    player->TakeDamage(15);
    EXPECT_EQ(player->GetHp(), 100);
}