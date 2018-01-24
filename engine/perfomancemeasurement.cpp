#include "perfomancemeasurement.h"
PerfomanceMeasurement measure_evaluation;

PerfomanceMeasurement::PerfomanceMeasurement()
{

}

void PerfomanceMeasurement::AddMeasure(const std::chrono::microseconds &delta)
{
    times++;
    time_span += delta;
}
