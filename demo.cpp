#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "synchronizer.h"
#include "demo.h"
#include "ringbuffer.h"
#include "recorder.h"


#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <QApplication>
#include <QTimer>
#include <QtGui>
#include <sys/time.h>


MainWidget::~MainWidget() {
    //delete markers
    free(buffer);
    free(ringbuffer.buffer);
    freeFingerprint(fingerprint);
}
void MainWidget::processData() {
    static struct timeval lastTime;
    struct timeval currentTime;
    while(ringbuffer.getElement(buffer)) {
        gettimeofday(&currentTime, NULL);
        qDebug() << currentTime.tv_usec - lastTime.tv_usec;
        qDebug() << "got data";
        processSamples(buffer, fingerprint);
        for(int j=0; j<257; j++) {
            x[j] = j;
            y[j] = sqrt(pow(fingerprint->out[j][0],2)+pow(fingerprint->out[j][1],2));
            curve->setRawSamples(x,y,257);
            Peaks peaks = fingerprint->peaks[fingerprint->numPeaks-1];
            for(int i=0; i<peaks.numPeaks; i++) {
                int xVal = peaks.peaks[i];
                markers[i]->setValue(xVal, y[xVal]);
            }
            replot();
        }
        qDebug() << "displaying";
        gettimeofday(&lastTime, NULL);
    }

}
MainWidget::MainWidget(QWidget *parent) :
    QwtPlot(parent),
    recorder(sampleRate, framesPerBuffer, 1, this)
{
    FingerprintConfiguration configuration; configuration.maxNumberPeaks = 4; configuration.fftPoints = framesPerBuffer; configuration.sampleBytes = sampleBytes;

    setAxisScale(yLeft, 0, 1);
    setAxisScale(yRight, 0, 1);
    curve = new QwtPlotCurve();
    curve->attach(this);
    markers = (QwtPlotMarker**)malloc(configuration.maxNumberPeaks*sizeof(QwtPlotMarker*));
    for(int i=0; i<configuration.maxNumberPeaks; i++) {
        QwtSymbol *sym = new QwtSymbol(QwtSymbol::Diamond,QBrush(Qt::red),QPen(Qt::red),QSize(5,5));
        markers[i] = new QwtPlotMarker();
        markers[i]->setSymbol(sym);
        markers[i]->attach(this);
    }

    connect(&recorder, SIGNAL(dataAvailable(unsigned char*)), this, SLOT(receiveData(unsigned char*)));
    //QTimer *mediaFileTimer = new QTimer(this);


    buffer = (unsigned char*)malloc(framesPerBuffer*sampleBytes);
    fingerprint = initFingerprint(configuration);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(processData()));
    timer->start(1);
}

void MainWidget::receiveData(unsigned char *buffer) {
    if(!ringbuffer.addElement(buffer)) {
        qDebug() << "dumping input due to full buffer";
    }
}

void MainWidget::getDataFromMediaFile() {

}
