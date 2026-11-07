#ifndef ABILITIES_H_
#define ABILITIES_H_

#include "IAbility.h"
#include "Robot.h"
#include "Field/Field.h"
#include "Engine/Config.h"
#include "IStatus.h"
#include "Statuses.h"
#include <string>
#include <algorithm>

inline bool IsInsideRadius(int start_x, int start_y, int target_x, int target_y, int radius) {
    int dx = start_x - target_x;
    int dy = start_y - target_y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

class AreaAttackAbility : public IAbility {
private:
    int level = 1;

public:
    std::string GetName() const override { return "Area Attack"; }
    int GetEnergyCost() const override { return Config::AreaAttackEnergyCost; } 
    int GetLevel() const override { return level; }
    void Upgrade() override { level++; }

    bool Use(Robot* user, Field& field, int /*target_x*/, int /*target_y*/) override {
        if (user->GetEnergy() < GetEnergyCost()) return false;

        bool hit_anyone = false;
        int current_radius = level; 

        for (int dy = -current_radius; dy <= current_radius; dy++) {
            for (int dx = -current_radius; dx <= current_radius; dx++) {
                if (dx == 0 && dy == 0) continue;

                int nx = user->x + dx;
                int ny = user->y + dy;

                if (nx >= 0 && nx < field.GetWidth() && ny >= 0 && ny < field.GetHeight()) {
                    Cell& cell = field.GetCell(nx, ny);

                    if (cell.HasRobot() && cell.GetRobot()->GetType() == Robot::Type::Enemy) {
                        Robot* target = cell.GetRobot();
                        target->TakeDamage(Config::AreaAttackDamage); 

                        target->ApplyStatus(std::make_shared<OverloadStatus>());
                        
                        hit_anyone = true;
                    }
                }
            }
        }

        user->ChangeEnergy(-GetEnergyCost());
        if (hit_anyone) user->AddExp(5); 
        return true;
    }
};

class RangedAttackAbility : public IAbility {
private:
    int level = 1;

public:
    std::string GetName() const override { return "Ranged Attack"; }
    int GetEnergyCost() const override { return Config::RangedAttackEnergyCost; }
    int GetLevel() const override { return level; }
    void Upgrade() override { level++; }

    bool Use(Robot* user, Field& field, int target_x, int target_y) override {
        if (user->GetEnergy() < GetEnergyCost()) 
            return false;
        if (target_x < 0 || target_x >= field.GetWidth() || target_y < 0 || target_y >= field.GetHeight()) 
            return false;

        if (!IsInsideRadius(user->x, user->y, target_x, target_y, 3)) 
            return false; 

        Cell& cell = field.GetCell(target_x, target_y);
        if (!cell.HasRobot()) 
            return false; 

        Robot* target = cell.GetRobot();
        if (cell.GetRobot()->GetType() == Robot::Type::Enemy) {
            int final_damage = Config::RangedAttackDamage + (level - 1) * 15;
            target->TakeDamage(final_damage);
            
            int burn_duration = 2 + (level - 1); 
            target->ApplyStatus(std::make_shared<BurnStatus>(burn_duration));
            
            user->ChangeEnergy(-GetEnergyCost());
            user->AddExp(5);
            return true;
        }
        return false; 
    }
};


class HealAbility : public IAbility {
private:
    int level = 1;

public:
    std::string GetName() const override { return "Self Heal"; }
    int GetEnergyCost() const override { return Config::SelfHealEnergyCost; }
    int GetLevel() const override { return level; }
    void Upgrade() override { level++; }

    bool Use(Robot* user, Field& /*field*/, int /*target_x*/, int /*target_y*/) override {
        if (user->GetEnergy() < GetEnergyCost()) 
            return false;
        
        int final_heal = Config::SelfHealAmount + (level - 1) * 15;
        int missing_hp = user->GetMaxHp() - user->GetHp();

        if (final_heal <= missing_hp) {
            user->ChangeHp(final_heal);
        } 
        else {
            user->ChangeHp(missing_hp);
            int overheal = final_heal - missing_hp;
            user->ApplyStatus(std::make_shared<ShieldStatus>(overheal));
        }

        user->ChangeEnergy(-GetEnergyCost());
        return true;
    }
};

class TeleportAbility : public IAbility {
private:
    int level = 1;

public:
    std::string GetName() const override { return "Teleport"; }
    int GetLevel() const override { return level; }
    void Upgrade() override { level++; }
    
    int GetEnergyCost() const override { 
        return std::max(5, Config::TeleportEnergyCost - (level - 1) * 5); 
    }

    bool Use(Robot* user, Field& field, int target_x, int target_y) override {
        if (user->GetEnergy() < GetEnergyCost()) 
            return false;
        if (target_x < 0 || target_x >= field.GetWidth() || target_y < 0 || target_y >= field.GetHeight()) 
            return false;

        if (!IsInsideRadius(user->x, user->y, target_x, target_y, 5)) 
            return false;

        Cell& target_cell = field.GetCell(target_x, target_y);
        if (target_cell.GetTerrain() == Cell::Terrain::Wall || target_cell.HasRobot()) 
            return false;

        field.GetCell(user->x, user->y).RemoveRobot();
        target_cell.SetRobot(user);
        if (target_cell.GetCellStatuses().size() > 0) {
            target_cell.TransferRandomStatusToRobot(target_cell.GetRobot());
        }

        user->x = target_x;
        user->y = target_y;

        user->ChangeEnergy(-GetEnergyCost());
        return true;
    }
};

#endif