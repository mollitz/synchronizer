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
        const int sampleRate = 44100;
        static const int framesPerBuffer = 1024; //Equals to fftPoints
        const int sampleBytes = 2;
        double x[framesPerBuffer/2+1], y[framesPerBuffer/2+1];
        unsigned char *buffer;
        FingerprintConfiguration configuration;
        Fingerprint *syncFingerprint, *originalFingerprint;
        Ringbuffer ringbuffer = Ringbuffer(10, framesPerBuffer*sampleBytes);
        Recorder recorder;
        Mp3Decoder mp3Decoder;
        QwtPlotCurve *curve;
        QwtPlotMarker **markers;
        void calculate();
    protected:
        virtual void keyPressEvent(QKeyEvent *keyEvent);

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
