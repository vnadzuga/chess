#include "global.h"
#pragma once
enum class Mode {
    Human,
    Master,
    Slave,
};

struct Config
{
    static Config from_args(int argc, char *argv[]);
    static Config from_start_color(int color);

    bool modeHuman() {
        return mode == Mode::Human;
    }

    bool modeMaster() {
        return mode == Mode::Master;
    }

    bool modeSlave() {
        return mode == Mode::Slave;
    }

    bool isAiBlack() { return ai_color == BLACK; }

    Mode mode = Mode::Human;
    int ai_color = BLACK;
};
