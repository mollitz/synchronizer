#include "mp3decoder.h"


Mp3Decoder::Mp3Decoder(const char *filename, QObject *parent) : QObject(parent) {
    sfFile = sf_open(filename, SFM_READ, &sfInfo);
    //now read file info from sfInfo
    printf("Frames: %d\nSamplerate: %d\nChannels: %d\nFormat: %x\nSections:%d\nSeekable:%d\n", sfInfo.frames, sfInfo.samplerate,sfInfo.channels,sfInfo.format,sfInfo.sections ,sfInfo.seekable);
}
Mp3Decoder::~Mp3Decoder() {
    sf_close(sfFile);
}

void Mp3Decoder::close() {
    sf_close(sfFile);
    sfFile = NULL;
}
int Mp3Decoder::getMonoFrames(int numFrames, void *buffer) {
    void *tmpBuffer = malloc(2*numFrames*2);
    int readFrames = sf_readf_short(sfFile, (short*)tmpBuffer, numFrames);
    for(int i=0; i<readFrames;i++) {
        memcpy(buffer+2*i, tmpBuffer+4*i, 2);
    }
    return readFrames;
}
int Mp3Decoder::getRawFrames(int numFrames, void *buffer) {
    return sf_readf_short(sfFile, (short*)buffer, numFrames);
}
