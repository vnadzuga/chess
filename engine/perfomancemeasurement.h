#pragma once
#include <chrono>

class PerfomanceMeasurement
{
public:
    PerfomanceMeasurement();
    PerfomanceMeasurement(const PerfomanceMeasurement &) = delete;
    PerfomanceMeasurement& operator = (const PerfomanceMeasurement &) = delete;
    void AddMeasure(const std::chrono::microseconds & delta);
    std::chrono::microseconds time_span = std::chrono::microseconds::zero();
    int times = 0;
};

class Point
{
public:
     Point(PerfomanceMeasurement * m):
         m(m)
     {
        start = std::chrono::high_resolution_clock::now();
     }

     ~Point()
     {
         using namespace std::chrono;
         microseconds time_span
            = duration_cast<microseconds>(high_resolution_clock::now() - start);
         m->AddMeasure(time_span);

     }
private:
     Point(const Point &) = delete;
     Point& operator = (const Point &) = delete;
     PerfomanceMeasurement * m;
     std::chrono::high_resolution_clock::time_point start;
};

extern PerfomanceMeasurement measure_evaluation;
#define EVALUATION_PROF_POINT Point evalutaion_point(&measure_evaluation)
