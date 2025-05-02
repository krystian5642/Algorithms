#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <functional>

#include <QObject>
#include <QRunnable>
#include <QPointF>
#include <QString>
#include <QHash>
#include <QComboBox>

class Algorithm : public QObject, public QRunnable
{
    Q_OBJECT

    Q_PROPERTY(int iterationsNumber READ getIterationsNumber WRITE setIterationsNumber NOTIFY iterationsNumberChanged FINAL)
public:
    using ComplexityPair = QPair<QString, std::function<int(int, int, int)>>;
    using ComplexityList = QList<ComplexityPair>;

    explicit Algorithm(QObject* parent = nullptr);
    virtual ~Algorithm() = 0;

    virtual void run() = 0;
    virtual void init() {};
    virtual void clear() {};

    virtual QWidget* createPropertiesWidget(QWidget* parent = nullptr);

    QString getSelectedComplexity() const;

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

protected:
    bool requestedEnd;

protected:
    ComplexityList complexityList;

private:
    QComboBox* complexityComboBox;
};

#endif // ALGORITHM_H
