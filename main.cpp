#include <fftw3.h>
#include <QApplication>
#include "demo.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);


    MainWidget plot;
    plot.show();

    return a.exec();

}

