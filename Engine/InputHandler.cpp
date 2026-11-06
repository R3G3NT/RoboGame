#include "InputHandler.h"
#include <ncurses.h>

Command InputHandler::ReadFromKeyboard() {
    Command cmd;
    int ch = getch();
    switch (ch) {
        case 'w': case 'W': case KEY_UP:
            cmd.type = Command::Type::Move; 
            cmd.dy = -1; 
            break;
        case 's': case 'S': case KEY_DOWN:
            cmd.type = Command::Type::Move; 
            cmd.dy =  1; 
            break;
        case 'a': case 'A': case KEY_LEFT:
            cmd.type = Command::Type::Move; 
            cmd.dx = -1; 
            break;
        case 'd': case 'D': case KEY_RIGHT:
            cmd.type = Command::Type::Move; 
            cmd.dx =  1; 
            break;
        case '\n':
            cmd.type = Command::Type::Wait; 
            break;
        case 'q': case 'Q':
            cmd.type = Command::Type::Quit; 
            break;
        default:
            cmd.type = Command::Type::None; 
            break;
    }
    return cmd;
}