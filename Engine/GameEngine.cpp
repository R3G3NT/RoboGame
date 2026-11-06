#include "GameEngine.h"
#include "Visualizer.h"
#include "InputHandler.h"
#include "Config.h"
#include <ncurses.h> 

GameEngine::GameEngine(int width, int height) : field(width, height) {}

void GameEngine::SpawnRobot(std::shared_ptr<Robot> robot, int x, int y) {
    if (field.IsWalkable(x, y)) {
        if (robot->GetType() == Robot::Type::Player) {
            player_ptr = robot.get();
        }
        robot->x = x;
        robot->y = y;
        field.GetCell(x, y).SetRobot(robot.get());
        robots.push_back(robot);
    }
}

bool GameEngine::MoveRobot(int from_x, int from_y, int dx, int dy) {
    if (!field.GetCell(from_x, from_y).HasRobot()) {
        return false;
    }

    Robot* moving_robot = field.GetCell(from_x, from_y).GetRobot();
    int speed = moving_robot->GetSpeed();
    if (moving_robot == player_ptr) {
        speed = 1;
    }
    int x = from_x, y = from_y;
    while (speed != 0) {
        x += dx;
        y += dy;

        if (x < 0 || x >= field.GetWidth() ||
            y < 0 || y >= field.GetHeight()) break;

        Cell& target_cell = field.GetCell(x, y);
        if (target_cell.HasRobot()) {
            moving_robot->Interact(target_cell.GetRobot());
            return true;
        }

        if (field.IsWalkable(x, y)) {
            field.GetCell(from_x, from_y).RemoveRobot();
            target_cell.SetRobot(moving_robot);
            from_x = x;
            from_y = y;
            moving_robot->x = x;
            moving_robot->y = y;
        }
        else {
            break;
        }
        speed--;
    }

    if (field.GetCell(moving_robot->x, moving_robot->y).GetTerrain() == Cell::Terrain::Swamp) {
        moving_robot->SetMaxSpeed(1);
    }

    return false;
}

void GameEngine::CleanDeadRobots() {
    std::erase_if(robots, [this](std::shared_ptr<Robot>& robot) {
        if (robot->GetHp() <= 0) {
            field.GetCell(robot->x, robot->y).RemoveRobot();
            if (robot.get() == player_ptr) {
                player_ptr = nullptr;
            }
            return true;
        }
        return false;
    });
}

void GameEngine::HandleEnemiesTurn() {
    if (!player_ptr) return;

    for (int i = 0; i < robots.size(); i++) {
        if (!player_ptr) return;

        if (robots[i]->GetType() == Robot::Type::Enemy) {
            int dx = 0, dy = 0;
            if (player_ptr->x > robots[i]->x)      dx = 1;
            else if (player_ptr->x < robots[i]->x) dx = -1;
            else if (player_ptr->y > robots[i]->y) dy = 1;
            else if (player_ptr->y < robots[i]->y) dy = -1;

            MoveRobot(robots[i]->x, robots[i]->y, dx, dy);
        }
    }
}

void GameEngine::UpdateVisibility() {
    if (!player_ptr) return;

    int radius = Config::VisibilityRadius;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius - 3; dx <= radius + 3; dx++) {
            int x = player_ptr->x + dx;
            int y = player_ptr->y + dy;

            if (x < 0 || x >= field.GetWidth() ||
                y < 0 || y >= field.GetHeight()) continue;
                
            field.GetCell(x, y).SetVisible(true);
        }
    }
}

void GameEngine::LoadLevel(const std::vector<std::string>& level) {
    int e = 1, f = 1;
    for (int y = 0; y < field.GetHeight(); y++) {
        for (int x = 0; x < field.GetWidth(); x++) {
            char c = level[y][x];
            switch (c) {
                case '#':
                    field.GetCell(x, y).SetTerrain(Cell::Terrain::Wall);
                    break;
                case '~':
                    field.GetCell(x, y).SetTerrain(Cell::Terrain::Swamp);
                    break;
                case 'R':
                    SpawnRobot(
                        std::make_shared<Robot>("Optimus", Robot::Type::Player,
                            Config::PlayerHp, Config::PlayerAttack,
                            Config::PlayerEnergy, Config::PlayerSpeed),
                        x, y);
                    break;
                case 'F':
                    SpawnRobot(
                        std::make_shared<Robot>("Friend" + std::to_string(f), Robot::Type::Friend,
                            Config::FriendHp, Config::FriendAttack,
                            Config::FriendEnergy, Config::FriendSpeed),
                        x, y);
                    f++;
                    break;
                case 'E':
                    SpawnRobot(
                        std::make_shared<Robot>("Enemy" + std::to_string(e), Robot::Type::Enemy,
                            Config::EnemyHp, Config::EnemyAttack,
                            Config::EnemyEnergy, Config::EnemySpeed),
                        x, y);
                    e++;
            }
        }
    }
}

void GameEngine::HandlePlayerTurn(Command cmd) {
    if (!player_ptr) return;

    switch (cmd.type) {
        case Command::Type::Quit:
            Visualizer::Shutdown();
            exit(0);
        case Command::Type::Wait:
            player_ptr->ChangeEnergy(Config::EnergyPerWait);
            return;
        case Command::Type::Move:
            MovePlayer(cmd.dx, cmd.dy);
            return;
    }
}

void GameEngine::MovePlayer(int dx, int dy) {
    while ((dx != 0 || dy != 0) && player_ptr->GetSpeed() != 0) {
        bool was_interacted = MoveRobot(player_ptr->x, player_ptr->y, dx, dy);
        player_ptr->ChangeSpeed(-1);

        field.ClearVisibility();
        UpdateVisibility();
        Visualizer::UpdateScreen(field, current_move, player_ptr);

        dx = dy = 0;
        if (player_ptr->GetSpeed() != 0 && !was_interacted) {
            Command next = InputHandler::ReadFromKeyboard();
            if (next.type == Command::Type::Quit) {
                Visualizer::Shutdown();
                exit(0);
            }
            else if (next.type == Command::Type::Move) {
                dx = next.dx;
                dy = next.dy;
            }
        }
    }

    while (player_ptr->GetSpeed() < player_ptr->GetMaxSpeed()) {
        player_ptr->ChangeSpeed(1);
    }
}

void GameEngine::ShowMessage(const std::string& message) {
    erase();
    Visualizer::Draw(field);
    printw("%s\n", message.c_str());
    refresh();
    getch();
}

bool GameEngine::Step(Command cmd) {
    HandlePlayerTurn(cmd);
    CleanDeadRobots();
    HandleEnemiesTurn();
    CleanDeadRobots();

    if (!player_ptr) {
        ShowMessage("\nGAME OVER! Your robot has been destroyed :(\nPress any key...");
        return false;
    }

    bool no_enemy = true;
    for (const auto& r : robots) {
        if (r->GetType() == Robot::Type::Enemy) {
            no_enemy = false;
            break;
        }
    }
    if (no_enemy) {
        ShowMessage("\nYou destroyed all the enemies! You WON :)\nPress any key...\n");
        return false;
    }

    for (auto& r : robots) {
        r->ChangeEnergy(Config::EnergyPerTurn);
    }

    ++current_move;
    field.ClearVisibility();
    UpdateVisibility();
    Visualizer::UpdateScreen(field, current_move, player_ptr);
    return true;
}

void GameEngine::Run() {
    UpdateVisibility();
    Visualizer::UpdateScreen(field, current_move, player_ptr);

    while (Step(InputHandler::ReadFromKeyboard())) { }
}