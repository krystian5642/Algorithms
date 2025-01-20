#include "graphwidget.h"
#include "./ui_graphwidget.h"
#include "addgraphedgedialog.h"

#include <QMouseEvent>
#include <QPainter>

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
        AddGraphEdgeDialog addGraphEdgeDialog;
        const int returnCode = addGraphEdgeDialog.exec();

        if(returnCode == QDialog::DialogCode::Accepted)
        {
            GraphNode startNode(0, event->pos());
            GraphNode endNode(0, event->pos() + QPoint(40, 40));

            bool startValueOk = false;
            const int startValue = addGraphEdgeDialog.getStartValue<int>(&startValueOk);
            if(startValueOk)
            {
                startNode.setValue(startValue);
                graph.addNode(startNode);
            }

            bool endValueOk = false;
            const int endValue = addGraphEdgeDialog.getEndValue<int>(&endValueOk);
            if(endValueOk)
            {
                endNode.setValue(endValue);
                graph.addNode(endNode);
            }

            if(startValueOk && endValueOk)
            {
                graph.addEdge(startNode, endNode, 10.f);
            }

            update();
        }
    }
}



