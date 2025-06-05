#ifndef ALGORITHM_H
#define ALGORITHM_H

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
    using ComplexityNameToFunction = QPair<QString, std::function<int(int, int, int)>>;
    using ComplexityPairsList = QList<ComplexityNameToFunction>;

    explicit Algorithm(QObject* parent = nullptr);
    virtual ~Algorithm() = 0;

    virtual void run() = 0;
    virtual void init(const DataStructure* dataStructure) {};
    virtual void clear() {};

    virtual QWidget* createPropertiesWidget(QWidget* parent = nullptr);
    virtual void appendPropertiesInfo(QString& infoText);

#ifdef QT_DEBUG
    virtual void debugRun() {};
#endif

    void requestEnd();

    int getIterationsNumber() const;
    void setIterationsNumber(int newIterationsNumber);

signals:
    void started();
    void finished(const QList<QPointF>& result, const QString& toolTip);

    void iterationsNumberChanged();

protected:
    virtual void execute() = 0;

    int iterationsNumber;
    QString selectedComplexity;

protected:
    bool requestedEnd;

protected:
    ComplexityPairsList complexityList;

    QList<DataStructureBuilder*> dataStructureBuilders;
};

#endif // ALGORITHM_H
