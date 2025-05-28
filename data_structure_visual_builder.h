#ifndef DATA_STRUCTURE_VISUAL_BUILDER_H
#define DATA_STRUCTURE_VISUAL_BUILDER_H

#include <QObject>

class DataStructureVisualBuilder : public QObject
{
    Q_OBJECT
public:
    explicit DataStructureVisualBuilder(QObject *parent = nullptr);

signals:
};

#endif // DATA_STRUCTURE_VISUAL_BUILDER_H
