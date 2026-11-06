#ifndef COMMAND_H_
#define COMMAND_H_

struct Command {
    enum class Type { None, Move, Wait, Quit };

    Type type = Type::None;
    int dx = 0;
    int dy = 0;
};

#endif