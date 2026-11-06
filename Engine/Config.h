#ifndef CONFIG_H_
#define CONFIG_H_

namespace Config {
    // Цены базовых действий
    constexpr int AttackCost = 20;
    constexpr int HealCost   = 20;

    // Эффекты базовых действий
    constexpr int HealAmount = 15;

    // Игровой цикл
    constexpr int VisibilityRadius = 3;
    constexpr int EnergyPerTurn    = 10;
    constexpr int EnergyPerWait    = 10;

    // Параметры роботов
    constexpr int PlayerHp     = 100;
    constexpr int PlayerAttack = 20;
    constexpr int PlayerEnergy = 60;
    constexpr int PlayerSpeed  = 2;

    constexpr int FriendHp     = 30;
    constexpr int FriendAttack = 5;
    constexpr int FriendEnergy = 20;
    constexpr int FriendSpeed  = 0;

    constexpr int EnemyHp      = 50;
    constexpr int EnemyAttack  = 10;
    constexpr int EnemyEnergy  = 30;
    constexpr int EnemySpeed   = 1;

    // ПАРАМЕТРЫ ДЛЯ СПОСОБНОСТЕЙ

    // Энергозатраты
    constexpr int AreaAttackEnergyCost = 25;
    constexpr int RangedAttackEnergyCost = 20;
    constexpr int SelfHealEnergyCost = 15;
    constexpr int TeleportEnergyCost = 30;

    // Сила эффектов
    constexpr int AreaAttackDamage = 15;
    constexpr int RangedAttackDamage = 20;
    constexpr int SelfHealAmount = 20;
}

#endif