#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include "Command.h"

class InputHandler {
public:
    static Command ReadFromKeyboard();
};

#endif