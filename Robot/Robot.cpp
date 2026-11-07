#include "Robot.h"
#include "IAbility.h"
#include "Abilities.h"
#include "Engine/Config.h"
#include "IStatus.h"
#include "Statuses.h"
#include <algorithm>

Robot::Robot(std::string name, Type type, int hp, int attack, int energy, int speed)
    : name(name), type(type), hp(hp), max_hp(hp), attack_power(attack),
      energy(energy), max_energy(energy), exp(0), exp_for_next_rank(50), speed(speed), max_speed(speed) {}

std::string Robot::GetName() const { return name; }
Robot::Type Robot::GetType() const { return type; }
int Robot::GetHp() const { return hp; }
int Robot::GetMaxHp() const { return max_hp; }
int Robot::GetAttackPower() const { return attack_power; }
int Robot::GetSpeed() const { return speed; }
int Robot::GetMaxSpeed() const { return max_speed; }
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

//Возвращает true, если ранг повысился
bool Robot::AddExp(int amount) {
    if (amount <= 0) return false;
    exp += amount;
    return CheckRankUp();
}

void Robot::ChangeSpeed(int amount) {
    if (speed + amount < 0) return;
    speed += amount;
}

void Robot::SetMaxSpeed(int new_max_speed) {
    if (new_max_speed < 0) return;
    max_speed = new_max_speed;
}

//Возвращает true, если ранг повысился
bool Robot::CheckRankUp() {
    bool leveled_up = false;
    while (exp >= exp_for_next_rank) {
        rank++;
        leveled_up = true;

        max_hp += 25;
        max_energy += 25;
        attack_power += 5;
        hp = max_hp;
        energy = max_energy;

        exp -= exp_for_next_rank;
        exp_for_next_rank *= 2;
    }
    return leveled_up;
}

void Robot::Interact(Robot* other) {
    bool is_enemy =
        (this->type == Type::Player && other->type == Type::Enemy) ||
        (this->type == Type::Enemy  && other->type == Type::Player);

    if (is_enemy) {
        if (this->energy >= Config::AttackCost) {
            other->TakeDamage(this->attack_power);
            this->ChangeEnergy(-Config::AttackCost);
            this->AddExp(5);
        }
    }
    else if (this->type == Type::Player && other->type == Type::Friend) {
        if (other->energy >= Config::HealCost) { 
            this->ChangeHp(Config::HealAmount);
            other->ChangeEnergy(-Config::HealCost);
        }
    }
}

void Robot::AddAbility(std::shared_ptr<IAbility> ability) {
    if (ability) {
        abilities.push_back(ability);
    }
}

const std::vector<std::shared_ptr<IAbility>>& Robot::GetAbilities() const {
    return abilities;
}

bool Robot::UseAbility(size_t index, Field& field, int target_x, int target_y) {
    if (index >= abilities.size()) {
        return false;
    }
    
    return abilities[index]->Use(this, field, target_x, target_y);
}

void Robot::ApplyStatus(std::shared_ptr<IStatus> new_status) {
    if (!new_status) return;

    // Ищем, нет ли уже у робота статуса с таким же именем
    for (auto& current_status : statuses) {
        if (current_status->GetName() == new_status->GetName()) {
            current_status->Stack(new_status.get());
            return;
        }
    }

    statuses.push_back(new_status);
}

void Robot::UpdateStatusesAtTurnStart() {
    for (auto& status : statuses) {
        status->OnTurnStart(this);
    }
    std::erase_if(statuses, [](const std::shared_ptr<IStatus>& s) { return s->IsExpired(); });
}

void Robot::UpdateStatusesAtTurnEnd() {
    for (auto& status : statuses) {
        status->OnTurnEnd(this);
    }
    std::erase_if(statuses, [](const std::shared_ptr<IStatus>& s) { return s->IsExpired(); });
}

bool Robot::CanMove() const {
    for (const auto& status : statuses) {
        if (!status->CanMove()) return false;
    }
    return true;
}

bool Robot::CanUseAbilities() {
    bool allowed = true;
    for (auto& status : statuses) {
        if (!status->CanUseAbilities()) {
            allowed = false;
        }
        // Оповещаем статусы о попытке использовать способность (триггер для сброса Перегрузки)
        status->OnAbilityAttempt(); 
    }
    std::erase_if(statuses, [](const std::shared_ptr<IStatus>& s) { return s->IsExpired(); });
    return allowed;
}

void Robot::TakeDamage(int damage) {
    if (damage <= 0) return;

    int final_damage = damage;
    for (auto& status : statuses) {
        final_damage = status->ProcessIncomingDamage(final_damage);
    }

    if (final_damage > 0) {
        ChangeHp(-final_damage);
    }
    
    std::erase_if(statuses, [](const std::shared_ptr<IStatus>& s) { return s->IsExpired(); });
}

const std::vector<std::shared_ptr<IStatus>>& Robot::GetStatuses() const {
    return statuses;
}