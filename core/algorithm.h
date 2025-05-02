#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <functional>

#include <QObject>
#include <QRunnable>
#include <QPointF>
#include <QString>
#include <QHash>

class Algorithm : public QObject, public QRunnable
{
    Q_OBJECT

    Q_PROPERTY(int iterationsNumber READ getIterationsNumber WRITE setIterationsNumber NOTIFY iterationsNumberChanged FINAL)
public:
    using StringToFunction = QPair<QString, std::function<int(int, int, int)>>;
    using StringFunctionPairs = QList<StringToFunction>;

    explicit Algorithm(QObject* parent = nullptr);
    virtual ~Algorithm() = 0;

    virtual void run() = 0;
    virtual void init() {};
    virtual void clear() {};

    virtual QWidget* createPropertiesWidget(QWidget* parent = nullptr, bool addStretch = true);

    void requestEnd();

    int getIterationsNumber() const;
    void setIterationsNumber(int newIterationsNumber);

signals:
    void started();
    void finished(const QList<QPointF>& result);

    void iterationsNumberChanged();

protected:
    virtual void execute() = 0;

    int iterationsNumber;
    QString selectedComplexity;

protected:
    bool requestedEnd;

protected:
    StringFunctionPairs complexityList;
};

#endif // ALGORITHM_H
