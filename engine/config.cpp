#include "config.h"
#include <string>

using namespace std;
Config Config::from_args(int argc, char *argv[]) {
    Config conf;

    const string MASTER = "--master";
    const string SLAVE = "--slave";

    for (int i = 1; i < argc; i++) {
        conf.ai_color = (string("white") == argv[i] ? WHITE : BLACK);
    }

    if (argc > 2 && MASTER == argv[1] ) {
        conf.mode = Mode::Master;
    } else {
        if (argc >= 3 && SLAVE == argv[1] ) {
            conf.mode = Mode::Slave;
        }
    }


    //conf.color
    return conf;
}

Config Config::from_start_color(int color)
{
    Config conf;
    conf.ai_color = color;
    return conf;
}
