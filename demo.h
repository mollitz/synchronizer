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
        const char *filename = "./examplemono32000.wav";
        const int sampleRate = 32000;
        static const int framesPerBuffer = 2048; //Equals to fftPoints
        const int sampleBytes = 2;
        double index; //save the current position of the song
        double x[framesPerBuffer/2+1], y[framesPerBuffer/2+1];
        unsigned char *buffer;
        FingerprintConfiguration configuration;
        Fingerprint *syncFingerprint, *originalFingerprint;
        Ringbuffer ringbuffer = Ringbuffer(10, framesPerBuffer*sampleBytes);
        Recorder recorder;
        Mp3Decoder mp3Decoder, mp3Decoder3;
        QwtPlotCurve *curve;
        QwtPlotMarker **markers;
        void calculate();
    protected:
        virtual void keyPressEvent(QKeyEvent *keyEvent);
    protected slots:
        void fillMusicBuffer();

    public:
        ~MainWidget();
        public slots:
        void processData();
        void tmpOrig();
        void receiveData(unsigned char*);
        void getDataFromMediaFile();
    public:
        MainWidget(QWidget *parent = 0);
};

#endif
