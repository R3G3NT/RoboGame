#pragma once
#include <string>

class Robot {
private:
    std::string name;
    int y, x;
    int hp, max_hp;
    int attack_power;
    int energy, max_energy;
    int exp, exp_to_next_rank;
    int rank; // Ранг матрицы :)
    bool enemy; // true - враг, false - дружбан

public:
    Robot(std::string p_name, int start_y, int start_x, bool p_is_enemy);

    std::string get_name() const;
    int get_y() const;
    int get_x() const;
    int get_hp() const;
    int get_max_hp() const;
    int get_attack_power() const;
    int get_energy() const;
    int get_max_energy() const;
    int get_exp() const;
    int get_exp_to_next_rank() const;
    int get_rank() const;
    bool is_enemy() const;

    void change_hp(int amount);
    void change_energy(int amount);
    void add_exp(int amount);

    void interact(Robot& other);

private:
    void check_rank_up();
};