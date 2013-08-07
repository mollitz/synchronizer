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


MainWidget::~MainWidget() {
    //delete markers
    free(buffer);
    free(ringbuffer.buffer);
    if(syncFingerprint)
        freeFingerprint(syncFingerprint);
    freeFingerprint(originalFingerprint);
}
void MainWidget::processData() {
    while(ringbuffer.getElement(buffer)) {
        qDebug() << "got data";
        processSamples(buffer, syncFingerprint);
        for(int j=0; j<framesPerBuffer/2+1; j++) {
            x[j] = j;
            y[j] = sqrt(pow(syncFingerprint->out[j][0],2)+pow(syncFingerprint->out[j][1],2));
            curve->setRawSamples(x,y,framesPerBuffer/2+1);
            Peaks peaks = syncFingerprint->peaks[syncFingerprint->numPeaks-1];
            for(int i=0; i<peaks.numPeaks; i++) {
                int xVal = peaks.peaks[i];
                markers[i]->setValue(xVal, y[xVal]);
            }
        }
        replot();
        qDebug() << "displaying";
    }
}
void MainWidget::fillMusicBuffer() {
    mp3Decoder3.getRawFrames(framesPerBuffer, buffer);
    recorder.setBuffer((short *)buffer, framesPerBuffer*2);
}
void MainWidget::tmpOrig() {
    mp3Decoder3.getRawFrames(framesPerBuffer, buffer);
    recorder.setBuffer((short *)buffer, framesPerBuffer*2);
    qDebug() << "got data";
    processSamples(buffer, syncFingerprint);
    for(int j=0; j<framesPerBuffer/2+1; j++) {
        x[j] = j;
        y[j] = sqrt(pow(syncFingerprint->out[j][0],2)+pow(syncFingerprint->out[j][1],2));
        curve->setRawSamples(x,y,framesPerBuffer/2+1);
        Peaks peaks = syncFingerprint->peaks[syncFingerprint->numPeaks-1];
        for(int i=0; i<peaks.numPeaks; i++) {
            int xVal = peaks.peaks[i];
            markers[i]->setValue(xVal, y[xVal]);
        }
    }
    replot();
    qDebug() << "displaying";
}
MainWidget::MainWidget(QWidget *parent) :
    QwtPlot(parent),
    recorder(sampleRate, framesPerBuffer, 1, this),
    mp3Decoder(filename, this),
    mp3Decoder3(filename, this)
{

    configuration.maxNumberPeaks = 8; configuration.fftPoints = framesPerBuffer; configuration.sampleBytes = sampleBytes;

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

    buffer = (unsigned char*)malloc(framesPerBuffer*sampleBytes);
    originalFingerprint = initFingerprint(configuration);

    while(mp3Decoder.getRawFrames(framesPerBuffer, buffer) == framesPerBuffer) {
        processSamples(buffer, originalFingerprint);
    }
    /*syncFingerprint = initFingerprint(configuration);
      Mp3Decoder mp3Decoder2(filename, this);
      int i=0;
      mp3Decoder2.getRawFrames(750, buffer);
      while(mp3Decoder2.getRawFrames(framesPerBuffer, buffer) == framesPerBuffer) {
      if(i>750)
      processSamples(buffer, syncFingerprint);
      if(i>1050)
      break;
      i++;
      }
      calculate();
      freeFingerprint(syncFingerprint);
      syncFingerprint = NULL;
      qDebug() << "parsed originalFingerprint. go for mic now";*/

}

void MainWidget::calculate() {
    int max;
    int *diffs = calculateDifference(originalFingerprint, syncFingerprint, &max);
    double *xx = (double*)malloc(sizeof(double)*1000);
    double *yy = (double*)malloc(sizeof(double)*1000);
    qDebug() << "max at " << max << "in seconds: " << max*framesPerBuffer/(float)sampleRate;
    index = max*(framesPerBuffer/(float)sampleRate);
    mp3Decoder3.seek(15000+(syncFingerprint->numPeaks+max)*framesPerBuffer);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fillMusicBuffer()));
    timer->start((1000.0*framesPerBuffer)/sampleRate);


    for(int j=0; j<1000; j++) {
        xx[j] = diffs[2*j];
        yy[j] = diffs[2*j+1];
    }
    curve->setRawSamples(xx,yy,1000);
    setAxisAutoScale(yLeft);
    setAxisAutoScale(yRight);

    replot();
    free(xx);
    free(yy);
    free(diffs);

}
void MainWidget::keyPressEvent(QKeyEvent *keyEvent) {
    QTimer *syncTimer;
    switch(keyEvent->key()) {
        case Qt::Key_E:
            disconnect(&recorder, SIGNAL(dataAvailable(unsigned char*)), this, SLOT(receiveData(unsigned char*)));
            qDebug() << "startdiff";
            calculate();
            qDebug() << "enddiff";
            freeFingerprint(syncFingerprint);
            syncFingerprint = NULL;
            break;
        case Qt::Key_O:
            syncFingerprint = initFingerprint(configuration);
            syncTimer = new QTimer(this);
            connect(syncTimer, SIGNAL(timeout()), this, SLOT(tmpOrig()));
            syncTimer->start((1000.0*framesPerBuffer)/sampleRate);
            break;
        case Qt::Key_S:
            syncFingerprint = initFingerprint(configuration);
            connect(&recorder, SIGNAL(dataAvailable(unsigned char*)), this, SLOT(receiveData(unsigned char*)));
            syncTimer = new QTimer(this);
            connect(syncTimer, SIGNAL(timeout()), this, SLOT(processData()));
            syncTimer->start(1);
            break;
        case Qt::Key_Plus:
            mp3Decoder3.seekAdd(1000);
            break;
        case Qt::Key_Minus:
            mp3Decoder3.seekAdd(-1000);
            break;
    }
}

void MainWidget::receiveData(unsigned char *buffer) {
    if(!ringbuffer.addElement(buffer)) {
        qDebug() << "dumping input due to full buffer";
    }
}

void MainWidget::getDataFromMediaFile() {
    qDebug() << "read Bytes: " << mp3Decoder.getRawFrames(framesPerBuffer, buffer);
    if(!ringbuffer.addElement(buffer)) {
        qDebug() << "dumping input due to full buffer";
    }
}
