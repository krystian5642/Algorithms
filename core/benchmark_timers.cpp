#include "benchmark_timers.h"
#include "algorithm.h"

AlgorithmBenchmarkTimerManager &AlgorithmBenchmarkTimerManager::getTimerManager()
{
    static AlgorithmBenchmarkTimerManager timerManager;
    return timerManager;
}

ULONG64 AlgorithmBenchmarkTimerManager::takeTimersWorkCycles()
{
    const ULONG64 cycles = timersWorkCycles;
    timersWorkCycles = 0;
    return cycles;
}

void AlgorithmBenchmarkTimerManager::clear(const Algorithm* algorithm)
{
    algorithmToResultData[algorithm].clear();
    timersWorkCycles = 0;
}

BenchmarkScopedTimer::BenchmarkScopedTimer(const Algorithm* inAlgorithm, const QString &inName, const QColor &inColor, bool inAdditive)
    : algorithm(inAlgorithm)
    , name(inName)
    , color(inColor)
    , additive(inAdditive)
{
    AlgorithmBenchmarkTimerManager::getTimerManager().algorithmToResultData[algorithm][name].color = color;

    QueryThreadCycleTime(GetCurrentThread(), &start);
}

BenchmarkScopedTimer::~BenchmarkScopedTimer()
{
    QueryThreadCycleTime(GetCurrentThread(), &end);

    ULONG64 workStart;
    QueryThreadCycleTime(GetCurrentThread(), &workStart);

    auto& timerManager = AlgorithmBenchmarkTimerManager::getTimerManager();
    auto& resultData = timerManager.algorithmToResultData[algorithm][name];

    const qreal elapsedCycles = (qreal)(end - start);
    const qreal x = algorithm->calculateXForCurrentIteration();

    bool append = true;
    if(additive)
    {
        auto it = std::find_if(resultData.points.begin(), resultData.points.end(), [x] (const QPointF& point)
        {
           return point.x() == x;
        });

        if(it != resultData.points.end())
        {
            it->ry() += elapsedCycles;
            append = false;
        }
    }

    if(append)
    {
        resultData.points.append((QPointF{x, elapsedCycles}));
    }

    ULONG64 workEnd;
    QueryThreadCycleTime(GetCurrentThread(), &workEnd);

    timerManager.timersWorkCycles += workEnd - workStart;
}
