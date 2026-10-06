#ifndef VISUALIZER_H_
#define VISUALISER_H_

class Field;
class Robot;

class Visualizer {
public:
    static void InitScreen();
    static void Shutdown();
    static void Draw(Field& field);
    static void DrawStatus(Robot* robot);
    static void UpdateScreen(Field& field, int current_move, Robot* player_ptr);
};

#endif