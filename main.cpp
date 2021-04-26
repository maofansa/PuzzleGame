#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(images);

    QApplication app(argc, argv);
    MainWindow window;
    window.loadImage(QStringLiteral(":/images/colorImg.bmp"), QStringLiteral(":/images/greyImg.bmp"));
    window.show();
    return app.exec();
}

