#include "Robot.h"
#include "Prices.h"
#include <algorithm>

Robot::Robot(std::string p_name, int start_y, int start_x, bool p_enemy) {
    y = start_y;
    x = start_x;
    name = p_name;
    max_hp = 100;
    hp = max_hp;
    attack_power = 20;
    max_energy = 100;
    energy = max_energy;
    exp_to_next_rank = 50;
    exp = 0;
    rank = 1;
    enemy = p_enemy;
}

std::string Robot::get_name() const {
    return name;
}

int Robot::get_y() const {
    return y;
}

int Robot::get_x() const {
    return x;
}

int Robot::get_hp() const {
    return hp;
}

int Robot::get_max_hp() const {
    return max_hp;
}

int Robot::get_attack_power() const {
    return attack_power;
}

int Robot::get_energy() const {
    return energy;
}

int Robot::get_max_energy() const {
    return max_energy;
}

int Robot::get_exp() const {
    return exp;
}

int Robot::get_exp_to_next_rank() const {
    return exp_to_next_rank;
}

int Robot::get_rank() const {
    return rank;
}

bool Robot::is_enemy() const {
    return enemy;
}

void Robot::change_hp(int amount) {
    hp = std::clamp(hp + amount, 0, max_hp);
}

void Robot::change_energy(int amount) {
    energy = std::clamp(energy + amount, 0, max_hp);
}

void Robot::add_exp(int amount) {
    if (amount <= 0) return;
    exp += amount;
    check_rank_up();
}

void Robot::check_rank_up() {
    while (exp >= exp_to_next_rank) {
        rank++;

        max_hp += 25;
        max_energy += 25;
        attack_power += 5;
        hp = max_hp;
        energy = max_energy;

        exp -= exp_to_next_rank;
        exp_to_next_rank *= 2;
    }
}

void Robot::interact(Robot &other) {
    if (this->enemy != other.enemy) {
        other.change_hp(-this->attack_power);
        this->change_energy(-static_cast<int>(Prices::Attack));
    }
    else {
        other.change_hp(15);
        this->change_energy(-static_cast<int>(Prices::Heal));
    }
}
