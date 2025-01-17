#include "graphwidget.h"
#include "./ui_graphwidget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>

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

void GraphWidget::paintEvent(QPaintEvent *event)
{
    QPen nodePen;
    nodePen.setBrush(Qt::black);
    nodePen.setWidth(2);

    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(nodePen);

    graph.paint(painter);
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {       
        const GraphNode<int> newNode(QRandomGenerator::global()->bounded(30), event->pos());

        const auto& graphContainer = graph.getGraphContainer();
        if(graphContainer.empty())
        {
            graph.addNode(newNode);
        }
        else
        {
            const auto nodes = graphContainer.keys();
            const int randomIndex = QRandomGenerator::global()->bounded(nodes.size());

            graph.addEdge(newNode, nodes[randomIndex], 10.f);
        }

        update();
    }
}



