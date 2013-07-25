#ifndef MP3DECODER_H
#define MP3DECODER_H
#include <QObject>
#include <stdio.h>
#include <sndfile.h>

class Mp3Decoder : public QObject {
    Q_OBJECT
    private:
        const char *filename = "/home/mollitz/Musik/Lieder/Blumentopf - Da draussen (feat. Roger Reckless).mp3";

    public:
        Mp3Decoder(QObject *parent = 0);

};

#endif
