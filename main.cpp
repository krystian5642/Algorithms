#include "widgets/main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion");
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
