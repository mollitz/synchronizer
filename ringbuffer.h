#ifndef RINGBUFFER_H
#define RINGBUFFER_H

typedef struct Ringbuffer{
    int putI;
    int getI;
    int elements;
    int bytesPerBuffer;
    unsigned char *buffer;
    bool aware;
    Ringbuffer(int elements, int bytesPerBuffer) {
        this->elements = elements;
        this->bytesPerBuffer = bytesPerBuffer;
        this->buffer = (unsigned char*)calloc(elements, bytesPerBuffer);
        aware = false;
        putI = getI = 0;
    }
    /**
      * Writes bytesPerBuffer bytes into the buffer
      *
      * \param buffer The buffer to read from
      */
    bool addElement(unsigned const char *inputBuffer) {
        if(aware)
            return false;
        memcpy(buffer+(bytesPerBuffer*putI), inputBuffer, bytesPerBuffer);
        putI = (putI+1)%elements;
        if(putI == getI)
            aware = true;
        return true;
    }
    /**
      * Reads bytesPerBuffer bytes from the buffer
      */
    bool getElement(unsigned char *outputBuffer) {
        if(getI == putI && !aware) 
            return false;
        memcpy(outputBuffer, buffer+(bytesPerBuffer*getI), bytesPerBuffer);
        getI = (getI+1)%elements;
        aware = false;
        return true;
    }
} Ringbuffer;

#endif
