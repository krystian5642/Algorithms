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

    virtual DataStructure* createDataStructure() = 0;
    virtual QWidget* createPropertiesWidget(QWidget* parent = nullptr);
    virtual void appendPropertiesInfo(QString& infoText);

protected:
    QList<QPair<QString, std::function<DataStructure*()>>> dataStructures;

    QStringList hiddenProperties;
};

#endif // DATA_STRUCTURE_BUILDER_H
