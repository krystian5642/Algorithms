#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include <QPointF>

class Algorithm : public QObject
{
    Q_OBJECT
public:
    explicit Algorithm(QObject* parent = nullptr);
    virtual ~Algorithm() = 0;

    virtual void run() = 0;

signals:
    void finished(const QList<QPointF>& result);

protected:
    virtual void execute() = 0;
};

#endif // ALGORITHM_H
