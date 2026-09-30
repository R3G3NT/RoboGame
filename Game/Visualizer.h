#pragma once

class Field;

class Visualizer {
public:
    static void init_screen();
    static void shutdown();
    static void draw(Field& field);
};
