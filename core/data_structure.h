#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <QObject>

class DataStructure : public QObject
{
    Q_OBJECT
public:
    explicit DataStructure(QObject *parent = nullptr);
    virtual ~DataStructure() = 0;

    virtual void print() const { };
};

#endif // DATA_STRUCTURE_H
