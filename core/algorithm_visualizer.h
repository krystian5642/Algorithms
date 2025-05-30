#ifndef ALGORITHM_VISUALIZER_H
#define ALGORITHM_VISUALIZER_H

#include <QTimer>
#include <QWidget>

class AlgorithmVisualizer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int stepTime READ getStepTime WRITE setStepTime NOTIFY stepTimeChanged FINAL)
public:
    AlgorithmVisualizer(QObject *parent = nullptr);
    virtual ~AlgorithmVisualizer() = 0;

    virtual void run(QWidget* widget) = 0;
    virtual void clear();

    void setPause(bool pause);
    QWidget* createPropertiesWidget(QWidget* parent = nullptr);

    int getStepTime() const;
    void setStepTime(int newStepTime);

signals:
    void finished();

    void stepTimeChanged();

protected:
    virtual void updateVisualization() = 0;

    QTimer visualizationTimer;
    int stepTime;
};

#endif // ALGORITHMVISUALIZER_H
