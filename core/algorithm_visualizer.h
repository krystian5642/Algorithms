#ifndef ALGORITHM_VISUALIZER_H
#define ALGORITHM_VISUALIZER_H

#include <QTimer>
#include <QWidget>

class AlgorithmVisualizer : public QObject
{
    Q_OBJECT
public:
    AlgorithmVisualizer(QObject *parent = nullptr);
    virtual ~AlgorithmVisualizer() = 0;

    virtual void run(QWidget* widget) = 0;
    virtual void reset();

    void setPause(bool pause);
    QWidget* createPropertiesWidget() const;

    int getStepTime() const;
    void setStepTime(int newStepTime);

protected:
    QTimer visualizationTimer;
    int stepTime = 1000;
};

#endif // ALGORITHMVISUALIZER_H
