#ifndef ROBOT_H_
#define ROBOT_H_

#include <string>
#include <vector>
#include <memory>

class IAbility; 
class Field;

class Robot {
public:
    enum class Type { Player, Friend, Enemy };

private:
    std::string name;
    Type type;
    int hp, max_hp;
    int attack_power;
    int speed, max_speed;
    int energy, max_energy;
    int exp, exp_for_next_rank;
    int rank = 1;

    std::vector<std::shared_ptr<IAbility>> abilities;

public:
    int x, y;
    Robot(std::string name, Type type, int hp, int attack, int energy, int speed);

    std::string GetName() const;
    Type GetType() const;
    int GetHp() const;
    int GetMaxHp() const;
    int GetAttackPower() const;
    int GetSpeed() const;
    int GetMaxSpeed() const;
    int GetEnergy() const;
    int GetMaxEnergy() const;
    int GetExp() const;
    int GetExpToNextRank() const;
    int GetRank() const;

    void ChangeHp(int amount);
    void ChangeEnergy(int amount);
    void ChangeSpeed(int amount);
    void SetMaxSpeed(int new_max_speed);
    void AddExp(int amount);

    void Interact(Robot* other);

    void AddAbility(std::shared_ptr<IAbility> ability);

    const std::vector<std::shared_ptr<IAbility>>& GetAbilities() const;

    bool UseAbility(size_t index, Field& field, int target_x = 0, int target_y = 0);

private:
    void CheckRankUp();
};

#endif