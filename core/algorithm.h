#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include <QPointF>
#include <QRunnable>

class Algorithm : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Algorithm(QObject* parent = nullptr);
    virtual ~Algorithm() = 0;

    virtual void run() = 0;
    virtual void init() {};
    virtual void clear() {};

    void requestEnd();

signals:
    void started();
    void finished(const QList<QPointF>& result);

protected:
    virtual void execute() = 0;

protected:
    bool requestedEnd;
};

#endif // ALGORITHM_H
