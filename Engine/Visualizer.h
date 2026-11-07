#ifndef VISUALIZER_H_
#define VISUALIZER_H_

#include <memory>

class Field;
class Robot;

class Visualizer {
public:
    enum class RenderMode { Normal, HighlightEnemies, HighlightWalls, HighlightTraps };

private:
    static RenderMode current_render_mode;

public:
    static void InitScreen();
    static void Shutdown();
    static void Draw(Field& field);
    static void DrawStatus(std::shared_ptr<Robot> robot);
    static void UpdateScreen(Field& field, int current_move, std::shared_ptr<Robot> player_ptr);
    static void ShowUpgradeMenu(std::shared_ptr<Robot> player);
    
    static void SetRenderMode(RenderMode mode);
};

#endif