#include "Robot.h"
#include "Prices.h"
#include <algorithm>

Robot::Robot(std::string name, Type type, int hp, int attack, int energy)
    : name(name), type(type), hp(hp), max_hp(hp), attack_power(attack),
      energy(energy), max_energy(energy), exp(0), exp_to_next_rank(50) {}

Robot::Type Robot::GetType() const { return type; }
std::string Robot::GetName() const { return name; }
int Robot::GetHp() const { return hp; }

void Robot::ChangeHp(int amount) {
    hp = std::clamp(hp + amount, 0, max_hp);
}

void Robot::ChangeEnergy(int amount) {
    energy = std::clamp(energy + amount, 0, max_hp);
}

void Robot::AddExp(int amount) {
    if (amount <= 0) return;
    exp += amount;
    CheckRankUp();
}

void Robot::CheckRankUp() {
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

void Robot::RestoreEnergy() {
    this->ChangeEnergy(10);
}

void Robot::Interact(Robot &other) {
    if (other.type == Type::Enemy) {
        other.ChangeHp(-this->attack_power);
        this->AddExp(5);
        this->ChangeEnergy(-static_cast<int>(Prices::Attack));
    }
    else {
        other.ChangeHp(15);
        this->ChangeEnergy(-static_cast<int>(Prices::Heal));
    }
}
