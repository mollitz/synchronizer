#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_marker.h>

#include <qwt_symbol.h>
#include "recorder.h"
#include "synchronizer.h"
#include "ringbuffer.h"
#include "mp3decoder.h"


class MainWidget : public QwtPlot {
    Q_OBJECT
    private:
        const char *filename = "/data/Musik/Alben/Brandon Flowers/Flamingo/08 - Crossfire.ogg";
        const int sampleRate = 16000;
        const int framesPerBuffer = 512; //Equals to fftPoints
        const int sampleBytes = 2;
        double x[257], y[257];
        unsigned char *buffer;
        Fingerprint *fingerprint;
        Ringbuffer ringbuffer = Ringbuffer(10, framesPerBuffer*sampleBytes);
        Recorder recorder;
        Mp3Decoder mp3Decoder;
        QwtPlotCurve *curve;
        QwtPlotMarker **markers;

    public:
        ~MainWidget();
        public slots:
            void processData();
        void receiveData(unsigned char*);
        void getDataFromMediaFile();
    public:
        MainWidget(QWidget *parent = 0);
};

#endif
