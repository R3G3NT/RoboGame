#ifndef CONFIG_H_
#define CONFIG_H_

enum class Config {
    // Цены действий
    AttackCost         = 20,
    HealCost           = 20,

    // Эффекты действий
    HealAmount         = 15,

    // Игровой цикл
    VisibilityRadius   = 3,
    EnergyPerTurn      = 10,
    EnergyPerWait      = 10,

    // Параметры роботов
    PlayerHp           = 100,
    PlayerAttack       = 20,
    PlayerEnergy       = 60,
    PlayerSpeed        = 2,

    FriendHp           = 30,
    FriendAttack       = 5,
    FriendEnergy       = 20,
    FriendSpeed        = 0,

    EnemyHp            = 50,
    EnemyAttack        = 10,
    EnemyEnergy        = 30,
    EnemySpeed         = 1,
};

#endif
