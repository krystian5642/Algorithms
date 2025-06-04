#include "data_structure_widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>

DataStructureWidget::DataStructureWidget(QWidget *parent)
    : QWidget(parent)
    , lastPaintTime(0)
{
}

DataStructureWidget::~DataStructureWidget()
{
}

void DataStructureWidget::registerAlgorithmVisualizers()
{

}

const QList<AlgorithmVisualizer *> &DataStructureWidget::getAlgorithmVisualizers() const
{
    return algorithmVisualizers;
}

const QString& DataStructureWidget::getCategory() const
{
    return category;
}

const QList<QAction *> &DataStructureWidget::getAdditionalActions() const
{
    return additionalActions;
}

const QList<QWidget *> &DataStructureWidget::getAdditionalToolBarWidget() const
{
    return additionalToolBarWidgets;
}

void DataStructureWidget::paintEvent(QPaintEvent *event)
{
    QPen nodePen;
    nodePen.setBrush(Qt::black);
    nodePen.setWidth(2);

    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(nodePen);

    painter.setTransform(getCurrentTransform());

    paintDataStructure(painter);
}

void DataStructureWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        dragData.isDragging = true;
        dragData.lastMousePos = event->position();
    }
}

void DataStructureWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        dragData.isDragging = false;
    }
}

void DataStructureWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(dragData.isDragging)
    {
        dragData.dragDelta += event->position() - dragData.lastMousePos;
        dragData.lastMousePos = event->position();
        update();
    }
}

void DataStructureWidget::wheelEvent(QWheelEvent *event)
{
    const qreal prevScale = scaleData.currentScale;

    const QTransform prevTransform = getCurrentTransform();
    const QPointF worldBefore = prevTransform.inverted().map(event->position());

    const int angleDelta = event->angleDelta().y();
    if(angleDelta > 0 && scaleData.currentScale < scaleData.maxScale)
    {
        scaleData.currentScale = std::min(scaleData.maxScale, scaleData.currentScale * scaleData.scaleMultiplier);
    }
    else if(angleDelta < 0 && scaleData.currentScale > scaleData.minScale)
    {
        scaleData.currentScale = std::max(scaleData.minScale, scaleData.currentScale / scaleData.scaleMultiplier);
    }

    if(prevScale != scaleData.currentScale)
    {
        const QTransform newTransform = getCurrentTransform();
        const QPointF screenAfter = newTransform.map(worldBefore);

        const QPointF delta = event->position() - screenAfter;
        scaleOffset += delta;

        update();
    }
}

qint64 DataStructureWidget::getLastPaintTime() const
{
    return lastPaintTime;
}

QTransform DataStructureWidget::getCurrentTransform() const
{
    QTransform transform;
    transform.translate(scaleOffset.x() + dragData.dragDelta.x(), scaleOffset.y() + dragData.dragDelta.y());
    transform.scale(scaleData.currentScale, scaleData.currentScale);

    return transform;
}

void DataStructureWidget::paintDataStructure(QPainter &painter)
{

}
