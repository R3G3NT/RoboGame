#include "Robot.h"
#include "../Engine/Config.h"
#include <algorithm>

Robot::Robot(std::string name, Type type, int hp, int attack, int energy, int speed)
    : name(name), type(type), hp(hp), max_hp(hp), attack_power(attack),
      energy(energy), max_energy(energy), exp(0), exp_for_next_rank(50), speed(speed) {}

std::string Robot::GetName() const { return name; }
Robot::Type Robot::GetType() const { return type; }
int Robot::GetHp() const { return hp; }
int Robot::GetMaxHp() const { return max_hp; }
int Robot::GetAttackPower() const { return attack_power; }
int Robot::GetSpeed() const { return speed; }
int Robot::GetEnergy() const { return energy; }
int Robot::GetMaxEnergy() const { return max_energy; }
int Robot::GetExp() const { return exp; }
int Robot::GetExpToNextRank() const { return exp_for_next_rank; }
int Robot::GetRank() const { return rank; }

void Robot::ChangeHp(int amount) {
    hp = std::clamp(hp + amount, 0, max_hp);
}

void Robot::ChangeEnergy(int amount) {
    energy = std::clamp(energy + amount, 0, max_energy);
}

void Robot::AddExp(int amount) {
    if (amount <= 0) return;
    exp += amount;
    CheckRankUp();
}

void Robot::ChangeSpeed(int amount) {
    if (speed + amount < 0) return;
    speed += amount;
}

void Robot::CheckRankUp() {
    while (exp >= exp_for_next_rank) {
        rank++;

        max_hp += 25;
        max_energy += 25;
        attack_power += 5;
        hp = max_hp;
        energy = max_energy;

        exp -= exp_for_next_rank;
        exp_for_next_rank *= 2;
    }
}

void Robot::Interact(Robot* other) {
    bool is_enemy =
        (this->type == Type::Player && other->type == Type::Enemy) ||
        (this->type == Type::Enemy  && other->type == Type::Player);

    if (is_enemy) {
        if (this->energy >= static_cast<int>(Config::AttackCost)) {
            other->ChangeHp(-this->attack_power);
            this->ChangeEnergy(-static_cast<int>(Config::AttackCost));
            this->AddExp(5);
        }
    }
    else if (this->type == Type::Player && other->type == Type::Friend) {
        if (other->energy >= static_cast<int>(Config::HealCost)) { 
            this->ChangeHp(static_cast<int>(Config::HealAmount));
            other->ChangeEnergy(-static_cast<int>(Config::HealCost));
        }
    }
}