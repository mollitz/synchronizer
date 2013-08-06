#include "recorder.h"
#include <QDebug>

Recorder::Recorder(int sampleRate, int framesPerBuffer, int numChannels, QObject *parent) : QObject(parent) {
    PaError err = paNoError;

    //Init Buffer.
    err = Pa_Initialize();
    if( err != paNoError ) {
        qDebug() << "Error initializing PortAudio\n";
    }

    //Get default input device:
    err = Pa_OpenDefaultStream(&stream,
            numChannels,
            0,
            paInt16,
            sampleRate,
            framesPerBuffer,
            inputStreamCallback,
            (void*)this);

    if(err != paNoError) {
        qDebug() << "Couldnt init stream";
    }
    err = Pa_StartStream(stream);
    if(err != paNoError) {
        qDebug() << "Couldnt start stream";
    }
}
Recorder::~Recorder() {
    Pa_StopStream(stream);
}

int Recorder::inputStreamCallback( const void *input,
                      void *output,
                      unsigned long frameCount,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *recorderObject) {
    Recorder *ro = (Recorder*)recorderObject;
    ro->emitDataAvailable((unsigned char*)input);
    return paContinue;
}

void Recorder::emitDataAvailable(unsigned char* buffer) {
    emit dataAvailable(buffer);
}
