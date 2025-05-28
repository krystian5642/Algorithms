#ifndef PROPERTIES_DIALOG_H
#define PROPERTIES_DIALOG_H

#include <QDialog>

class PropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PropertiesDialog(QObject* object);

private:
    void setupUi(QObject* object);
};

#endif // PROPERTIES_DIALOG_H
