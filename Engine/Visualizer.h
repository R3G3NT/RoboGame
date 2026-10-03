#ifndef VISUALIZER_H_
#define VISUALISER_H_

class Field;

class Visualizer {
public:
    static void InitScreen();
    static void Shutdown();
    static void Draw(Field& field);
};

#endif
