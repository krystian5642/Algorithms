#ifndef ALGORITHM_WIDGET_H
#define ALGORITHM_WIDGET_H

#include <QTimer>
#include <QWidget>

class DataStructureVisualBuilder;
class AlgorithmVisualizer;

struct ScaleData
{
    qreal currentScale = 1.0;
    qreal minScale = 0.1;
    qreal maxScale = 15.0;
    qreal scaleMultiplier = 1.1;
};

struct RuntimeMouseDragData
{
    bool isDragging = false;
    QPointF dragDelta;
    QPointF lastMousePos;
};

class DataStructureWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataStructureWidget(QWidget *parent = nullptr);
    ~DataStructureWidget();

    // actions
    virtual void saveAction() = 0;
    virtual void loadAction() = 0;
    virtual void clearAction() = 0;
    virtual void generateRandomDataStructureAction() = 0;
    virtual bool visualizeAlgorithmAction(AlgorithmVisualizer* algorithmVisualizer, bool pause) = 0;
    virtual void registerAlgorithmVisualizers();

    const QList<AlgorithmVisualizer*>& getAlgorithmVisualizers() const;

    const QString& getCategory() const;

    const QList<QAction*>& getAdditionalActions() const;
    const QList<QWidget*>& getAdditionalToolBarWidget() const;
    qint64 getLastPaintTime() const;

    QTransform getCurrentTransform() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    QList<AlgorithmVisualizer*> algorithmVisualizers;
    QList<QAction*> additionalActions;
    QList<QWidget*> additionalToolBarWidgets;
    QList<DataStructureVisualBuilder*> dataStructureVisualBuilders;

    AlgorithmVisualizer* currentAlgorithmVisualizer = nullptr;

    QString category;

    qint64 lastPaintTime;

    RuntimeMouseDragData dragData;

    ScaleData scaleData;

    QPointF scaleOffset;

private:
    virtual void paintDataStructure(QPainter& painter);

};

#endif // ALGORITHM_WIDGET_H
