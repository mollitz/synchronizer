/* Test file testing the application */
#include <math.h>
#include <string.h>
#include "synchronizer.h"
#include <stdio.h>
#include <stdlib.h>

#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <QApplication>



/**
  * generate a sinus wave with given parameters and 16 bit signed int per sample
  */
unsigned char *generateSinus(int frequency, int sampleRate, double secs) {
    unsigned short *buf =static_cast<unsigned short*> (malloc(sampleRate*secs*2));
    int i;
    for(i=0; i<sampleRate*secs; i++) {
        buf[i] = sin((i*2.0*M_PI*frequency)/sampleRate)*pow(2,15);
    }
    return (unsigned char *)buf;
}
void testsinus() {
    int frequencies[] = {83, 132, 242, 442, 2930};
    int sampleRates[] = {8000, 16000, 220500, 41000, 48000};
    int fftPoints[] = {128,256,512,1024,2048};

    const double secs = 5.0;
    unsigned int fI;
    for(fI=0; fI<sizeof(frequencies)/sizeof(frequencies[0]); fI++) {
        unsigned int sI;
        for(sI=0; sI<sizeof(sampleRates)/sizeof(sampleRates[0]); sI++) {
            unsigned int pI;
            for(pI=0;pI<sizeof(fftPoints)/sizeof(fftPoints[0]);pI++) {
                unsigned char *buf = generateSinus(frequencies[fI], sampleRates[sI], secs);
                //Now the API call to Synchronizer :)
                struct FingerprintConfiguration configuration; configuration.maxNumberPeaks = 5; configuration.fftPoints=fftPoints[pI]; configuration.sampleBytes = 2;
                struct Fingerprint *fingerprint = initFingerprint(configuration);
                unsigned int cI;
                for(cI=0; cI+(fftPoints[pI]*2-1)<secs*sampleRates[sI]*2; cI+=fftPoints[pI]*2) {
                    processSamples(buf+cI, fingerprint);
                }
                //And now dump the shit
                printf("Calculated fft of sinus wave with frequency: %d Hz, samplerate: %d samples/sec, points per fft: %d\n", frequencies[fI], sampleRates[sI], fftPoints[pI]);
                printf("Dumping values: \n");
                unsigned int dI;
                for(dI=0;dI<fingerprint->numPeaks; dI++) {
                    unsigned int asdf;
                    for(asdf=0; asdf<fingerprint->peaks[dI].numPeaks; asdf++) {
                        printf("%d ", fingerprint->peaks[dI].peaks[asdf]);
                    }
                    printf("\n");
                }

                //Free buffer!!
                freeFingerprint(fingerprint);
                free(buf);
            }
        }
    }
}
void fillValues(double *x, double *y, int fftPoints) {
    int sampleRate=32000;
    int freq = 1090;
    double secs = 5.0;
    unsigned char *buf = generateSinus(freq, sampleRate, secs);
    //Now the API call to Synchronizer :)
    struct FingerprintConfiguration configuration; configuration.maxNumberPeaks = 4; configuration.fftPoints=fftPoints; configuration.sampleBytes = 2;
    struct Fingerprint *fingerprint = initFingerprint(configuration);
    processSamples(buf, fingerprint);

    unsigned int cI;
    for(cI=0; cI+(fftPoints*2-1)<secs*sampleRate*2; cI+=fftPoints*2) {
        processSamples(buf+cI, fingerprint);
        
        unsigned int i;
        for(i=0; i<(fftPoints/2)+1; i++) {
            x[i] = i;
            y[i] = sqrt(pow(fingerprint->out[i][0],2)+pow(fingerprint->out[i][1],2));
        }
        break;
    }
    //Free buffer!!
    freeFingerprint(fingerprint);
    free(buf);

}
void testall() {
    testsinus();
    //testdoublesinuswithoffset();
    //testoddvalues();
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    double x[257];
    double y[257];

    fillValues(x,y, 512);
    //testall();


    QwtPlot plot;
    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setRawSamples(x,y,257);
    curve->attach(&plot);
    plot.show();

    return a.exec();
    return 0;
}
