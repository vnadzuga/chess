#pragma once
#include <fstream>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

// Constants to compare with the macros
#define WHITE 0x00
#define BLACK 0x10
#define TOGGLE_COLOR(x) (0x10 ^ x)
#define NOT !

//#define TRACE
#define SHOW_DETAILS

class Global
{
public:
    Global();
    void setLoggingFileName(const std::string & fileName);
    void log(const std::string & str);
    static Global& instance();
    int color = -1;
    std::fstream file;
    boost::posix_time::ptime start;
public:

};






