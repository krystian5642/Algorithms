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

BenchmarkScopedTimer::BenchmarkScopedTimer(const Algorithm* inAlgorithm, const QString &inName, const QColor &inColor, AggregationMode inAggregationMode)
    : algorithm(inAlgorithm)
    , name(inName)
    , color(inColor)
    , aggregationMode(inAggregationMode)
{
#ifdef QT_DEBUG
    if(!algorithm->getIsDebugRun())
#endif
    {
        AlgorithmBenchmarkTimerManager::getTimerManager().algorithmToResultData[algorithm][name].color = color;

        QueryThreadCycleTime(GetCurrentThread(), &start);
    }
}

BenchmarkScopedTimer::~BenchmarkScopedTimer()
{
#ifdef QT_DEBUG
    if(!algorithm->getIsDebugRun())
#endif
    {
        QueryThreadCycleTime(GetCurrentThread(), &end);

        ULONG64 workStart;
        QueryThreadCycleTime(GetCurrentThread(), &workStart);

        auto& timerManager = AlgorithmBenchmarkTimerManager::getTimerManager();
        auto& resultData = timerManager.algorithmToResultData[algorithm][name];

        const qreal elapsedCycles = (qreal)(end - start);
        const qreal x = algorithm->calculateXForCurrentIteration();

        auto it = std::find_if(resultData.points.begin(), resultData.points.end(), [x] (const QPointF& point)
        {
            return point.x() == x;
        });

        if(it != resultData.points.end())
        {
            switch (aggregationMode)
            {
            case AggregationMode::Sum:
                it->ry() += elapsedCycles;
                break;

            case AggregationMode::Min:
                it->ry() = std::min(it->y(), elapsedCycles);
                break;

            case AggregationMode::Max:
                it->ry() = std::max(it->y(), elapsedCycles);
                break;

            default:
                break;
            }
        }
        else
        {
            resultData.points.append((QPointF{x, elapsedCycles}));
        }

        ULONG64 workEnd;
        QueryThreadCycleTime(GetCurrentThread(), &workEnd);

        timerManager.timersWorkCycles += workEnd - workStart;
    }
}
