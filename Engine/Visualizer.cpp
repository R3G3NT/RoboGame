#include "Visualizer.h"
#include "Field/Field.h"
#include "Robot/Robot.h"
#include "Robot/IAbility.h"
#include "Robot/Abilities.h"
#include <ncurses.h>

void Visualizer::InitScreen() {
    initscr();             // Врубаем ncurses
    cbreak();              // Отключаем буферизацию строк
    noecho();              // Не выводим нажатые клавиши на экране
    keypad(stdscr, TRUE);  // Разрешаем стрелочки и F-клавиши
    curs_set(0);           // Прячем мигающий курсор
    clear();               // Чистим экран
}

void Visualizer::Shutdown() {
    endwin(); // Восстанавливаем стандартные настройки терминала
}

void Visualizer::Draw(Field& field) {
    for (int y = 0; y < field.GetHeight(); y++) {
        for (int x = 0; x < field.GetWidth(); x++) {
            Cell& cell = field.GetCell(x, y);

            if (!cell.IsVisible()) {
                printw(".");
            } 
            else if (cell.GetTerrain() == Cell::Terrain::Wall) {
                printw("#");
            } 
            else if (cell.HasRobot()) {
                Robot* robot = cell.GetRobot();
                
                if (robot->GetType() == Robot::Type::Player) {
                    printw("R");
                } 
                else if (robot->GetType() == Robot::Type::Friend) {
                    printw("F");
                } 
                else if (robot->GetType() == Robot::Type::Enemy) {
                    printw("E");
                }
            } 
            else if (cell.GetTerrain() == Cell::Terrain::Swamp) {
                printw("~");
            } 
            else {
                printw(" ");
            }
        }
        printw("\n");
    }
}

void Visualizer::DrawStatus(std::shared_ptr<Robot> robot) {
    if (!robot) {
        printw(" No player \n");
        return;
    }

    printw("%s\n", robot->GetName().c_str());
    printw("HP: %d/%d\n", robot->GetHp(), robot->GetMaxHp());
    printw("EN: %d/%d\n", robot->GetEnergy(), robot->GetMaxEnergy());
    printw("ATK: %d\n", robot->GetAttackPower());
    printw("SPD: %d\n", robot->GetSpeed());
    printw("RANK: %d\n", robot->GetRank());
    printw("EXP: %d/%d\n", robot->GetExp(), robot->GetExpToNextRank());
    printw("POS: (%d, %d)\n", robot->x, robot->y);

    printw("\n=== AVAILABLE ABILITIES ===\n");
    
    const auto& abilities = robot->GetAbilities();
    
    if (abilities.empty()) {
        printw(" No abilities learned yet.\n");
    } 
    else {
        for (size_t i = 0; i < abilities.size(); i++) {
            std::string ability_display_name = abilities[i]->GetName() + " (LVL " + std::to_string(abilities[i]->GetLevel()) + ")";
            printw("[%d] %-15s (%d EN)\n", 
                       static_cast<int>(i + 1), 
                       ability_display_name.c_str(), 
                       abilities[i]->GetEnergyCost());
        }
    }
}

void Visualizer::UpdateScreen(Field& field, int current_move, std::shared_ptr<Robot> player_ptr) {
    erase();
    Visualizer::Draw(field);
    printw("\n");
    printw("=== Move: %d ===\n\n", current_move);
    Visualizer::DrawStatus(player_ptr);
    printw("\n");
    refresh();
}

void Visualizer::ShowUpgradeMenu(std::shared_ptr<Robot> player) {
    if (!player) return;

    int choice = 0;
    const auto& current_abilities = player->GetAbilities();

    // Определяем, какие способности из 4-х базовых еще НЕ получены
    bool has_area = false, has_ranged = false, has_heal = false, has_teleport = false;
    for (const auto& ab : current_abilities) {
        if (ab->GetName() == "Area Attack") has_area = true;
        if (ab->GetName() == "Ranged Attack") has_ranged = true;
        if (ab->GetName() == "Self Heal") has_heal = true;
        if (ab->GetName() == "Teleport") has_teleport = true;
    }

    while (true) {
        erase();
        attron(A_BOLD);
        mvprintw(0, 0, "== RANK UP! Choose your reward (Rank %d) ==", player->GetRank());
        attroff(A_BOLD);

        std::vector<std::string> menu_items;
        
        // Опции улучшения существующих способностей
        for (size_t i = 0; i < current_abilities.size(); ++i) {
            menu_items.push_back("Upgrade: " + current_abilities[i]->GetName() + 
                                 " (LVL " + std::to_string(current_abilities[i]->GetLevel()) + " -> " + 
                                 std::to_string(current_abilities[i]->GetLevel() + 1) + ")");
        }

        if (!has_area) menu_items.push_back("Learn NEW: Area Attack");
        if (!has_ranged) menu_items.push_back("Learn NEW: Ranged Attack");
        if (!has_heal) menu_items.push_back("Learn NEW: Self Heal");
        if (!has_teleport) menu_items.push_back("Learn NEW: Teleport");

        // Отрисовка пунктов меню прокачки
        for (int i = 0; i < static_cast<int>(menu_items.size()); i++) {
            if (i == choice) {
                attron(A_REVERSE | A_BOLD);
                mvprintw(2 + i, 0, "-> %s", menu_items[i].c_str());
                attroff(A_REVERSE | A_BOLD);
            } else {
                mvprintw(2 + i, 0, "   %s", menu_items[i].c_str());
            }
        }

        mvprintw(3 + menu_items.size(), 0, "Use Arrow Keys / WASD, Enter to Confirm.");
        refresh();

        int ch = getch();
        switch (ch) {
            case 'w': case 'W': case KEY_UP:
                choice = (choice - 1 + menu_items.size()) % menu_items.size();
                break;
            case 's': case 'S': case KEY_DOWN:
                choice = (choice + 1) % menu_items.size();
                break;
            case '\n':
                if (choice < static_cast<int>(current_abilities.size())) {
                    current_abilities[choice]->Upgrade();
                } else {
                    std::string chosen_text = menu_items[choice];
                    if (chosen_text.find("Area Attack") != std::string::npos) {
                        player->AddAbility(std::make_shared<AreaAttackAbility>());
                    }
                    else if (chosen_text.find("Ranged Attack") != std::string::npos) {
                        player->AddAbility(std::make_shared<RangedAttackAbility>());
                    }
                    else if (chosen_text.find("Self Heal") != std::string::npos) {
                        player->AddAbility(std::make_shared<HealAbility>());
                    }
                    else if (chosen_text.find("Teleport") != std::string::npos) { 
                        player->AddAbility(std::make_shared<TeleportAbility>());
                    }
                }
                erase();
                return;
        }
    }
}