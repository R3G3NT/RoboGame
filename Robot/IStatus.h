#ifndef ISTATUS_H_
#define ISTATUS_H_

#include <string>

class Robot;

class IStatus {
public:
    virtual ~IStatus() = default;

    virtual std::string GetName() const = 0;

    // Возвращает текущую длительность в ходах (или прочность для щита)
    virtual int GetValue() const = 0;

    // Проверяет, исчерпал ли себя статус и нужно ли его удалить
    virtual bool IsExpired() const = 0;

    // Логика объединения статусов при повторном наложении
    virtual void Stack(IStatus* other) = 0;

    // Эффекты, срабатывающие в начале хода (например, урон от горения)
    virtual void OnTurnStart(Robot* robot) = 0;

    // Эффекты, срабатывающие в конце хода (например, уменьшение длительности замедления)
    virtual void OnTurnEnd(Robot* robot) = 0;

    // Модификаторы ограничений действий робота
    virtual bool CanMove() const { return true; }
    virtual bool CanUseAbilities() const { return true; }

    // Перехват входящего урона (для Щита). Возвращает урон, прошедший в здоровье.
    virtual int ProcessIncomingDamage(int damage) { return damage; }

    // Триггер, срабатывающий при попытке применить способность (для Перегрузки)
    virtual void OnAbilityAttempt() {}
};

#endif