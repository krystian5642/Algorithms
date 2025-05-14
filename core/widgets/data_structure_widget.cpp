#include "data_structure_widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>

DataStructureWidget::DataStructureWidget(QWidget *parent)
    : QWidget(parent)
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

void DataStructureWidget::paintEvent(QPaintEvent *event)
{
    QPen nodePen;
    nodePen.setBrush(Qt::black);
    nodePen.setWidth(2);

    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(nodePen);

    painter.translate(dragData.dragDelta);
    painter.scale(scaleData.currentScale, scaleData.currentScale);
    painter.translate(scaleOffset);

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
    const int angleDelta = event->angleDelta().y();
    if(angleDelta > 0)
    {
        scaleData.currentScale *= scaleData.scaleMultiplier;
    }
    else
    {
        scaleData.currentScale /= scaleData.scaleMultiplier;
    }

    scaleOffset = QPointF(event->position().x() * (1 - scaleData.currentScale), event->position().y() * (1 - scaleData.currentScale));

    update();
}

void DataStructureWidget::paintDataStructure(QPainter &painter)
{

}
