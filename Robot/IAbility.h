#ifndef IABILITY_H_
#define IABILITY_H_

#include <string>

class Robot;
class Field;

class IAbility {
public:
    virtual ~IAbility() = default;

    virtual std::string GetName() const = 0;

    virtual int GetEnergyCost() const = 0;

    virtual bool Use(Robot* user, Field& field, int target_x = 0, int target_y = 0) = 0;
};

#endif