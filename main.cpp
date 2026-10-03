#include "Game/GameEngine.h"
#include "Game/Visualizer.h"
#include <ncurses.h>
#include <memory>

int main() {
    // 1. Включаем ncurses графику (инициализируем экран, скрываем курсор, включаем cbreak)
    Visualizer::init_screen();

    try {
        // 2. Создаем игровой движок с полем размером 20 на 12 ячеек
        GameEngine engine(20, 12);

        // 3. Создаем и спавним робота-игрока (Player)
        // Характеристики: Имя, Тип, Очки здоровья (HP), Сила атаки, Энергия
        auto player = std::make_unique<Robot>("Оптимус", Robot::Type::Player, 100, 20, 60);
        engine.spawn_robot(std::move(player), 2, 2); // Спавн в левой верхней части

        // 4. Создаем и спавним агрессивного врага (Enemy)
        auto enemy = std::make_unique<Robot>("Десептикон", Robot::Type::Enemy, 40, 15, 30);
        engine.spawn_robot(std::move(enemy), 10, 5);

        // 5. Создаем и спавним доброго медика-союзника (Friend)
        auto friend_bot = std::make_unique<Robot>("Бамблби", Robot::Type::Friend, 50, 10, 40);
        engine.spawn_robot(std::move(friend_bot), 4, 3);

        // 6. Запускаем главный интерактивный цикл игры
        // Управление внутри движка настроено на WASD или Стрелочки, выход по клавише Q.
        engine.run();

    } catch (const std::exception& e) {
        // Если в процессе игры произойдет критическая ошибка,
        // мы аварийно, но безопасно закрываем ncurses для восстановления консоли
        Visualizer::shutdown();
        return 1;
    }

    // 7. Корректно закрываем ncurses при штатном выходе из игры
    Visualizer::shutdown();
    return 0;
}
