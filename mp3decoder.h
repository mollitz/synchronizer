#ifndef MP3DECODER_H
#define MP3DECODER_H
#include <QObject>
#include <stdio.h>
#include <sndfile.h>

class Mp3Decoder : public QObject {
    Q_OBJECT
    private:
        SF_INFO sfInfo;
        SNDFILE *sfFile;

    public:
        Mp3Decoder(const char *filename, QObject *parent = 0);
        ~Mp3Decoder();
        void close();
        /* Saves numFrames frames in buffer. Returns the number of the actual frames read (in case of EOF e.g.) or -1 in case of error. */
        int getMonoFrames(int numFrames, void *buffer);
};

#endif
