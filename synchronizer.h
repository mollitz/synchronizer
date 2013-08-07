
#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <fftw3.h>
/**
  * Author license data blablabla
  */

//include fftw


//Always calculate in chunks

/**
  * struct containing information for the fingerprinting
  */
typedef struct FingerprintConfiguration {
    //int minNumberPeaks;
    int maxNumberPeaks;
    //float peakQualifier;
    int fftPoints;
    int sampleBytes;
} FingerprintConfiguration;

/**
  * struct containing peaks for a chunk of samples //Make it private!?
  */
typedef struct Peaks {
    //Number of peaks
    int *peaks;
    int numPeaks;
} Peaks;

/**
  * A fingerprint
  */
//also hold plan, fftPoints, ... make it more static due to performance
typedef struct Fingerprint {
    Peaks *peaks;
    int numPeaks;

    FingerprintConfiguration configuration;


    double *in;
    fftw_complex *out;
    fftw_plan_s *plan;
} Fingerprint;

/**
  * Fingerprint factory
  */
Fingerprint *initFingerprint(FingerprintConfiguration configuration);

/**
  * Fingerprint freeer. free EVERY FKN Pointer
  */
void freeFingerprint(Fingerprint *);

/**
  * \arg buffer The input PCM buffer. Must have apropriate length for given sampleBits and fftPoints (l = sampleBytes*fftPoints)
  * \arg sampleBytes How many bytes per sample
  * \arg fftPoints The number of fft points
  * \arg fftBuffer The buffer in which the peaks are safes as indices from 0 to fftPoints
  * \return Return the number of peaks. In this version it is maxNumberPeaks
  */
void processSamples(unsigned char *buffer, Fingerprint *fingerprint);

/**
  * Returns the difference index and a value telling the difference between average and peak(later..)  TODO
  * \arg original The Fingerprint of the original PCM
  * \arg sync The Fingerprint to find the differnce
  */
int *calculateDifference(Fingerprint *original, Fingerprint *sync, int *maxRtn);

#endif
