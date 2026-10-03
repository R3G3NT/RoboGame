#ifndef ROBOT_H_
#define ROBOT_H_

#include <string>

class Robot {
public:
    enum class Type { Player, Friend, Enemy };

private:
    std::string name;
    Type type;
    int hp, max_hp;
    int attack_power;
    int energy, max_energy;
    int exp, exp_to_next_rank;
    int rank = 1;

public:
    int x, y;
    Robot(std::string name, Type type, int hp, int attack, int energy);

    Type GetType() const;
    std::string GetName() const;
    int GetHp() const;

    void ChangeHp(int amount);
    void ChangeEnergy(int amount);
    void AddExp(int amount);

    void RestoreEnergy();
    void Interact(Robot& other);

private:
    void CheckRankUp();
};

#endif
