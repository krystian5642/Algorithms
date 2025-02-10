#include "graphwidget.h"

#include <QMouseEvent>
#include <QPainter>

#include "./ui_graphwidget.h"
#include "addgraphedgedialog.h"

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GraphWidget)
{
    ui->setupUi(this);
}

GraphWidget::~GraphWidget()
{
    delete ui;
}

void GraphWidget::clearGraph()
{
    graph.clear();
    graphNodeVisualData.clear();
    graphEdgeVisualData.clear();
    update();
}

QJsonObject GraphWidget::toJsonObject() const
{
    QJsonObject locationsAsJsonObject;
    for(auto it = graphNodeVisualData.constBegin(); it != graphNodeVisualData.constEnd(); it++)
    {
        QJsonObject locationAsJsonObject;
        locationAsJsonObject["x"] = it.value().location.x();
        locationAsJsonObject["y"] = it.value().location.y();

        locationsAsJsonObject[QString::number(it.key())] = locationAsJsonObject;
    }
    return locationsAsJsonObject;
}

void GraphWidget::fromJsonObject(const QJsonObject &jsonObj)
{
    graphNodeVisualData.clear();
    for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); ++it)
    {
        const QJsonObject jsonObject = it.value().toObject();
        const QPoint graphNodeLocation(jsonObject["x"].toInt(), jsonObject["y"].toInt());

        graphNodeVisualData[it.key().toInt()].location = graphNodeLocation;
    }
    update();
}

void GraphWidget::setNodeColor(int value, const QColor& color)
{
    auto it = graphNodeVisualData.find(value);
    if(it != graphNodeVisualData.end() && it->color != color)
    {
        it->color = color;
        update();
    }
}

void GraphWidget::setEdgeColor(int start, int end, const QColor &color)
{
    const QPair<int, int> edge(qMin(start, end), qMax(start, end));
    auto it = graphEdgeVisualData.find(edge);
    if(it != graphEdgeVisualData.end() && it->color != color)
    {
        it->color = color;
        update();
    }
}

void GraphWidget::addNode(int value, const QPoint& location)
{
    if(!graphNodeVisualData.contains(value))
    {
        graphNodeVisualData[value].location = location;
        graph.addNode(value);
    }
}

void GraphWidget::addEdge(int startValue, int endValue, float weight)
{
    graph.addEdge(startValue, endValue, weight);
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    QPen nodePen;
    nodePen.setBrush(Qt::black);
    nodePen.setWidth(2);

    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(nodePen);

    painter.translate(dragDelta);
    painter.scale(currentScaleData.currentScale, currentScaleData.currentScale);
    painter.translate(scaleOffset);

    paintGraph(painter);
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        AddGraphEdgeDialog addGraphEdgeDialog;
        const int returnCode = addGraphEdgeDialog.exec();

        QPoint nodeLocation = event->pos();

        if(returnCode == QDialog::DialogCode::Accepted)
        {
            bool startValueOk = false;
            const int startValue = addGraphEdgeDialog.getStartValue<int>(&startValueOk);
            if(startValueOk)
            {
                addNode(startValue, nodeLocation);
                nodeLocation += QPoint(50, 50);
            }

            bool endValueOk = false;
            const int endValue = addGraphEdgeDialog.getEndValue<int>(&endValueOk);
            if(endValueOk)
            {
                addNode(endValue, nodeLocation);
            }

            if(startValueOk && endValueOk)
            {
                const double weightValue = addGraphEdgeDialog.getEdgeWeight();
                addEdge(startValue, endValue, weightValue);
            }

            update();
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        isDragging = true;
        lastMousePos = event->position();
    }
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        isDragging = false;
    }
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(isDragging)
    {
        dragDelta += event->position() - lastMousePos;
        lastMousePos = event->position();
        update();
    }
}

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    const int angleDelta = event->angleDelta().y();
    if(angleDelta > 0)
    {
        currentScaleData.currentScale *= currentScaleData.scaleMultiplier;
    }
    else
    {
        currentScaleData.currentScale /= currentScaleData.scaleMultiplier;
    }

    scaleOffset = QPointF(event->position().x() * (1 - currentScaleData.currentScale), event->position().y() * (1 - currentScaleData.currentScale));

    update();
}

void GraphWidget::paintGraph(QPainter &painter)
{
    painter.save();

    paintEdges(painter);
    paintNodes(painter);
    paintNodeValues(painter);

    painter.restore();
}

void GraphWidget::paintEdges(QPainter &painter)
{
    QSet<QPair<int, int>> drawnEdges;
    drawnEdges.reserve(qMax(0, graph.getIsDirected() ? graph.getEdgesNum() : graph.getEdgesNum() / 2));

    for (auto it = graph.constBegin(); it != graph.constEnd(); ++it)
    {
        const auto& value = it.key();
        const auto& neighbours = it.value();

        for(const auto& neighbour : neighbours)
        {
            const QPair<int, int> edge(qMin(value, neighbour.getEndValue()), qMax(value, neighbour.getEndValue()));
            if(drawnEdges.contains(edge))
            {
                continue;
            }

            QPen edgePen;
            edgePen.setBrush(graphEdgeVisualData[edge].color);
            painter.setPen(edgePen);

            painter.drawLine(graphNodeVisualData[value].location, graphNodeVisualData[neighbour.getEndValue()].location);
            drawnEdges.insert(edge);
        }
    }
}

void GraphWidget::paintNodes(QPainter &painter)
{
    for (auto it = graph.constBegin(); it != graph.constEnd(); ++it)
    {
        const auto& value = it.key();
        const auto& neighbours = it.value();

        QPen nodePen;
        nodePen.setBrush(graphNodeVisualData[value].color);
        painter.setPen(nodePen);
        painter.setBrush(graphNodeVisualData[value].color);

        painter.drawEllipse(graphNodeVisualData[value].location, 15, 15);
    }
}

void GraphWidget::paintNodeValues(QPainter &painter)
{
    QPen textPen;
    textPen.setBrush(Qt::white);
    painter.setPen(textPen);

    for (auto it = graph.constBegin(); it != graph.constEnd(); ++it)
    {
        const auto& value = it.key();
        const auto& neighbours = it.value();

        painter.drawText(graphNodeVisualData[value].location + QPointF(-3.5, 3), QString("%1").arg(value));
    }
}
