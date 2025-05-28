#ifndef DATA_STRUCTURE_VISUAL_BUILDER_H
#define DATA_STRUCTURE_VISUAL_BUILDER_H

#include <QObject>

class DataStructureWidget;
class DataStructure;

class DataStructureVisualBuilder : public QObject
{
    Q_OBJECT
public:
    explicit DataStructureVisualBuilder(QObject *parent = nullptr);
    virtual ~DataStructureVisualBuilder();

    virtual void buildDataStructureVisualization(DataStructureWidget* dataStructureWidget) = 0;
};

#endif // DATA_STRUCTURE_VISUAL_BUILDER_H
