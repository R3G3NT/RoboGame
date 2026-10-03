#include <algorithm>
#include "Robot.h"
#include "Prices.h"

Robot::Robot(std::string name, Type type, int hp, int attack, int energy)
    : name(name), type(type), hp(hp), max_hp(hp), attack_power(attack),
      energy(energy), max_energy(energy), exp(0), exp_to_next_rank(50) {}

Robot::Type Robot::get_type() const { return type; }
std::string Robot::get_name() const { return name; }
int Robot::get_hp() const { return hp; }

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

void Robot::restore_energy() {
    this->change_energy(10);
}

void Robot::interact(Robot &other) {
    if (other.type == Type::Enemy) {
        other.change_hp(-this->attack_power);
        this->add_exp(5);
        this->change_energy(-static_cast<int>(Prices::Attack));
    }
    else {
        other.change_hp(15);
        this->change_energy(-static_cast<int>(Prices::Heal));
    }
}