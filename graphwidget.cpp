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
    update();
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

    paintGraph(painter);
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        AddGraphEdgeDialog addGraphEdgeDialog;
        const int returnCode = addGraphEdgeDialog.exec();

        if(returnCode == QDialog::DialogCode::Accepted)
        {
            bool startValueOk = false;
            const int startValue = addGraphEdgeDialog.getStartValue<int>(&startValueOk);
            if(startValueOk)
            {
                addNode(startValue, event->pos());
            }

            bool endValueOk = false;
            const int endValue = addGraphEdgeDialog.getEndValue<int>(&endValueOk);
            if(endValueOk)
            {
                addNode(endValue, event->pos() + QPoint(50, 50));
            }

            if(startValueOk && endValueOk)
            {
                const double weightValue = addGraphEdgeDialog.getEdgeWeight();
                addEdge(startValue, endValue, weightValue);
            }

            update();
        }
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

void GraphWidget::paintGraph(QPainter &painter)
{
    painter.save();

    const auto& graphContainer = graph.getGraphContainer();

    for (auto it = graphContainer.constBegin(); it != graphContainer.constEnd(); ++it)
    {    
        const auto& value = it.key();
        const auto& neighbours = it.value();

        QPen edgePen;
        edgePen.setBrush(graphNodeVisualData[value].color);
        painter.setPen(edgePen);
        painter.setBrush(graphNodeVisualData[value].color);

        painter.drawEllipse(graphNodeVisualData[value].location, 15, 15);

        for(const auto& neighbour : neighbours)
        {
            painter.drawLine(graphNodeVisualData[value].location, graphNodeVisualData[neighbour.getEndValue()].location);
        }
    }

    QPen textPen;
    textPen.setBrush(Qt::white);

    painter.setPen(textPen);

    for (auto it = graphContainer.constBegin(); it != graphContainer.constEnd(); ++it)
    {
        const auto& value = it.key();
        const auto& neighbours = it.value();

        painter.drawText(graphNodeVisualData[value].location + QPointF(-3.5, 3), QString("%1").arg(value));
    }

    painter.restore();
}
