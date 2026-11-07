#ifndef STATUSES_H_
#define STATUSES_H_

#include "IStatus.h"
#include "Robot/Robot.h"
#include <string>
#include <algorithm>

class SlowStatus : public IStatus {
private:
    int duration = 1;

public:
    std::string GetName() const override { return "Slow"; }
    int GetValue() const override { return duration; }
    bool IsExpired() const override { return duration <= 0; }
    bool CanMove() const override { return false; }

    void Stack(IStatus* other) override {
        if (other && other->GetName() == this->GetName()) {
            this->duration += other->GetValue();
        }
    }

    void OnTurnStart(Robot* /*robot*/) override {}
    
    void OnTurnEnd(Robot* /*robot*/) override {
        duration--; // Статус спадает в конце хода
    }
};

class OverloadStatus : public IStatus {
private:
    bool expired = false;

public:
    std::string GetName() const override { return "Overload"; }
    int GetValue() const override { return expired ? 0 : 1; }
    bool IsExpired() const override { return expired; }
    bool CanUseAbilities() const override { return false; }

    void Stack(IStatus* other) override {
        // При получении нескольких перегрузок, длительность не растет, работает как одна
    }

    void OnTurnStart(Robot* /*robot*/) override {}
    void OnTurnEnd(Robot* /*robot*/) override {}

    void OnAbilityAttempt() override {
        expired = true; // Статус пропадает при попытке использовать способность
    }
};

class BurnStatus : public IStatus {
private:
    int duration;
    const int damage_per_turn = 5;

public:
    BurnStatus(int initial_duration) : duration(initial_duration) {}

    std::string GetName() const override { return "Burn"; }
    int GetValue() const override { return duration; }
    bool IsExpired() const override { return duration <= 0; }

    void Stack(IStatus* other) override {
        if (other && other->GetName() == this->GetName()) {
            this->duration += other->GetValue();
        }
    }

    void OnTurnStart(Robot* robot) override {
        if (duration > 0) {
            robot->ChangeHp(-damage_per_turn);
            duration--;
        }
    }

    void OnTurnEnd(Robot* /*robot*/) override {}
};

class ShieldStatus : public IStatus {
private:
    int shield_points;

public:
    ShieldStatus(int points) : shield_points(points) {}

    std::string GetName() const override { return "Shield"; }
    int GetValue() const override { return shield_points; }
    bool IsExpired() const override { return shield_points <= 0; }

    void Stack(IStatus* other) override {
        if (other && other->GetName() == this->GetName()) {
            this->shield_points += other->GetValue(); // Увеличивается прочность щита
        }
    }

    void OnTurnStart(Robot* /*robot*/) override {}
    void OnTurnEnd(Robot* /*robot*/) override {}

    int ProcessIncomingDamage(int damage) override {
        if (damage <= 0) return damage;

        if (shield_points >= damage) {
            shield_points -= damage;
            return 0;
        } else {
            int remaining_damage = damage - shield_points;
            shield_points = 0;
            return remaining_damage;
        }
    }
};

#endif