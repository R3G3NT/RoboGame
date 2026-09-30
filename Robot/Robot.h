#pragma once
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
    Robot(std::string name, Type type, int hp, int attack, int energy);

    Type get_type() const;
    std::string get_name() const;
    int get_hp() const;

    void change_hp(int amount);
    void change_energy(int amount);
    void add_exp(int amount);

    void restore_energy();
    void interact(Robot& other);

private:
    void check_rank_up();
};