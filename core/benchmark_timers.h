#ifndef BENCHMARK_TIMERS_H
#define BENCHMARK_TIMERS_H

#include <windows.h>

#include <QColor>
#include <QElapsedTimer>
#include <QHash>
#include <QPointF>

class Algorithm;

#define BENCHMARK_SCOPED_TIMER(name, color, additive) BenchmarkScopedTimer scopedTimer##__LINE__(this, name, color, additive);

struct SubSeriesData
{
    QColor color;
    QList<QPointF> points;
};

struct AlgorithmBenchmarkResult
{
    QList<QPointF> mainSeries;
    QString toolTipInfo;
    QHash<QString, SubSeriesData> subSeriesNameToSubSeriesData;

    void clear()
    {
        mainSeries.clear();
        toolTipInfo.clear();
        subSeriesNameToSubSeriesData.clear();
    }

    SubSeriesData& operator[](const QString& name)
    {
        return subSeriesNameToSubSeriesData[name];
    }
};

class AlgorithmBenchmarkTimerManager
{
public:
    friend class BenchmarkScopedTimer;

    static AlgorithmBenchmarkTimerManager& getTimerManager();

    QHash<const Algorithm*, AlgorithmBenchmarkResult> algorithmToResultData;

    ULONG64 takeTimersWorkCycles();
    void clear(const Algorithm* algorithm);

private:
    ULONG64 timersWorkCycles = 0;
};

class BenchmarkScopedTimer
{
public:
    BenchmarkScopedTimer(const Algorithm* inAlgorithm, const QString &inName, const QColor &inColor, bool inAdditive);
    ~BenchmarkScopedTimer();

protected:
    QString name;
    QColor color;
    bool additive;

private:
    ULONG64 start;
    ULONG64 end;

    const Algorithm* algorithm;
};

#endif // BENCHMARK_TIMERS_H
