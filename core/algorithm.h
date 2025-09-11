#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "../core/benchmark_timers.h"

#include <functional>

#include <QObject>
#include <QRunnable>
#include <QPointF>
#include <QString>
#include <QHash>

class DataStructureBuilder;
class DataStructure;

class Algorithm : public QObject, public QRunnable
{
    Q_OBJECT

    Q_PROPERTY(int iterationsNumber READ getIterationsNumber WRITE setIterationsNumber NOTIFY iterationsNumberChanged FINAL)
public:
    using ComplexityFunction = std::function<qreal(int, int, int)>;
    using ComplexityNameToFunction = QPair<QString, ComplexityFunction>;
    using ComplexityPairsList = QList<ComplexityNameToFunction>;

    explicit Algorithm(QObject* parent = nullptr);
    virtual ~Algorithm() = 0;

    virtual void run() = 0;
    virtual qreal calculateXForCurrentIteration() const = 0;

    virtual bool canRunAlgorithm(QString& outInfo) const;

    virtual QWidget* createPropertiesWidget(QWidget* parent = nullptr);
    virtual void appendPropertiesInfo(QString& infoText);

#ifdef QT_DEBUG
    virtual void debugRun() {};
#endif

    void requestEnd();

    int getIterationsNumber() const;
    void setIterationsNumber(int newIterationsNumber);

#ifdef QT_DEBUG
    bool getIsDebugRun() const;
    void setIsDebugRun(bool newIsDebugRun);
#endif

signals:
    void started();
    void finished(const AlgorithmBenchmarkResult &resultData);

    void iterationsNumberChanged();

protected:
    virtual void execute() = 0;

    int iterationsNumber;
    QString selectedComplexity;

#ifdef QT_DEBUG
    bool isDebugRun;
#endif

    bool requestedEnd;

    ComplexityPairsList complexityList;

    QList<DataStructureBuilder*> dataStructureBuilders;

    ComplexityFunction currentComplexityFunction;
    int currentIteration;
};

#endif // ALGORITHM_H
