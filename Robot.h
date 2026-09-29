#pragma once

class Robot {
private:
    int hp, max_hp;
    int attack_power;
    int energy, max_energy;
    int exp, exp_to_next_rank; // Ранг матрицы :)
    int rank;
    bool is_enemy; // true - враг, false - дружбан

public:
    Robot(bool is_enemy);

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

    void interact(Robot& robot);

private:
    void check_rank_up();
};