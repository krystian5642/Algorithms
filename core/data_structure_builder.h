#ifndef DATA_STRUCTURE_BUILDER_H
#define DATA_STRUCTURE_BUILDER_H

#include <QHash>
#include <QObject>

class DataStructure;

class DataStructureBuilder : public QObject
{
    Q_OBJECT
public:
    explicit DataStructureBuilder(QObject *parent = nullptr);
    virtual ~DataStructureBuilder();

    virtual DataStructure* buildDataStructure() = 0;
    virtual QWidget* createPropertiesWidget(QWidget* parent = nullptr);

protected:
    QList<QPair<QString, std::function<DataStructure*()>>> dataStructures;
};

#endif // DATA_STRUCTURE_BUILDER_H
