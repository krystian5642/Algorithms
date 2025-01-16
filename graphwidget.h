#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class GraphWidget;
}
QT_END_NAMESPACE

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

private:
    Ui::GraphWidget *ui;
};

#endif // GRAPHWIDGET_H
