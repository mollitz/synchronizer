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
            numChannels,
            paInt16,
            sampleRate,
            framesPerBuffer,
            inputStreamCallback,
            (void*)this);

    if(err != paNoError) {
        qDebug() << "Couldnt init stream";
    }
    start();
    outputBuffer = (short *)calloc(framesPerBuffer, 2);
}

Recorder::~Recorder() {
    stop();
    free(outputBuffer);
}

void Recorder::stop() {
    Pa_StopStream(stream);
}
void Recorder::start() {
    PaError err = Pa_StartStream(stream);
    if(err != paNoError) {
        qDebug() << "Couldnt start stream";
    }
}
short *Recorder::getBuffer() {
    return outputBuffer;
}

void Recorder::setBuffer(short *buffer, int numBytes) {
    memcpy(outputBuffer, buffer, numBytes);
}


int Recorder::inputStreamCallback( const void *input,
                      void *output,
                      unsigned long frameCount,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *recorderObject) {
    Recorder *ro = (Recorder*)recorderObject;
    ro->emitDataAvailable((unsigned char*)input);
    memcpy(output, ro->getBuffer(), frameCount*2);
    return paContinue;
}

void Recorder::emitDataAvailable(unsigned char* buffer) {
    emit dataAvailable(buffer);
}
