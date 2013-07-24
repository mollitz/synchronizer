#include "synchronizer.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <fftw3.h>

Fingerprint *initFingerprint(FingerprintConfiguration configuration) {
    Fingerprint *fingerprint = (Fingerprint*)calloc(1, sizeof(Fingerprint));
    if(fingerprint == NULL) {
        fprintf(stderr, "Error initializing Fingerprint buffer with malloc. "); //Get String from other include-file//exit??
        return NULL;
    }
    fingerprint->configuration = configuration;
    fingerprint->in = (double*)fftw_alloc_real(configuration.fftPoints);
    fingerprint->out = (fftw_complex*)fftw_alloc_complex(configuration.fftPoints/2 +1);
    fingerprint->plan = fftw_plan_dft_r2c_1d(configuration.fftPoints, fingerprint->in, fingerprint->out, FFTW_ESTIMATE);
    return fingerprint;
}

/**
  * Helper function to calculate absolute value
  */
/*double abs(fftw_complex in) {
    return sqrt(in[0]*in[0]+in[1]*in[1]);
}*/

double myabs(fftw_complex value) {
    //Compare abs and myabs;
    double val = sqrt(value[0]*value[0] + value[1] + value[1]);
    return val;
}

/**
  * TODO: this comment
  */
void processSamples(unsigned char *buffer, Fingerprint *fingerprint) {
    const FingerprintConfiguration configuration = fingerprint->configuration;
    //First check for space in array
    fingerprint->numPeaks++;
    if(fingerprint->numPeaks == 1) {
        fingerprint->peaks = (Peaks*)calloc(1, sizeof(Peaks));
    }
    else {
        fingerprint->peaks = (Peaks*)realloc(fingerprint->peaks, sizeof(Peaks)*fingerprint->numPeaks);
    }
    //Transform input buffer
    int i;
    if(configuration.sampleBytes != 2)
    {
        printf("Only 2 bytes samples supported. sorry.");
        exit(-1);
    }
    //Load values and apply Hann-Windows
    //fingerprint->in[0] = 0.5*(1+cos((2.0/(configuration.fftPoints-1))))*M_PI*((double)((short*)buffer)[0]);
    for(i=0; i<configuration.fftPoints; i++) {
        //fingerprint->in[i] = 0.5*(1-cos((2.0/(configuration.fftPoints-1))))*M_PI*((double)((short*)buffer)[i]);
        fingerprint->in[i] = (double)((short*)buffer)[i];
    }

    //Calculate FFT
    fftw_execute(fingerprint->plan);
    //Post-Calculation (convert complex to real; averaging?)

    //Get highest values
    int *peaksArray = (int*)malloc(sizeof(int) * configuration.maxNumberPeaks); //TODO if maxNumberPeaks will be 0. BE CAREFUL!
    for(i=0; i<configuration.maxNumberPeaks; i++) {
        peaksArray[i] = -1;
    }
    for(i=0; i<(configuration.fftPoints/2)+1; i++) {
        int j;
        for(j=0; j<configuration.maxNumberPeaks; j++) {
            if(peaksArray[j] == -1 || myabs(fingerprint->out[peaksArray[j]]) < myabs(fingerprint->out[i])) {
                if(peaksArray[j] != -1) //only optimization
                    memmove(peaksArray+j+1,peaksArray+j,sizeof(int)*(configuration.maxNumberPeaks-(j+1)));
                peaksArray[j] = i;
                break;
            }
        }
    }
    fingerprint->peaks[fingerprint->numPeaks-1].peaks = peaksArray;
    fingerprint->peaks[fingerprint->numPeaks-1].numPeaks = configuration.maxNumberPeaks;
}

int calculateDifference(Fingerprint *original, Fingerprint *sync) {
    //No-one will understand this ever ever.. :D
    // TODO Check their configurations to see if they are equal or simliar and maybe convert something?
    Peaks *freqs = (Peaks *)calloc(original->configuration.fftPoints/2+1, sizeof(Peaks));
    int i;
    for(i=0; i<original->numPeaks; i++) {
        int j;
        for(j=0; j<original->peaks[i].numPeaks; j++)
        {
            int freq = original->peaks[i].peaks[j];
            freqs[freq].numPeaks++;
            if(freqs[freq].numPeaks == 1) {
                freqs[freq].peaks = (int*)calloc(1, sizeof(int));
            }
            else {
                freqs[freq].peaks = (int*)realloc(freqs[freq].peaks, freqs[freq].numPeaks*sizeof(int));
            }
            freqs[freq].peaks[freqs[freq].numPeaks-1] = i;
        }
    }
    const int diffCount = 1000;
    int *differences = (int*)calloc(2*diffCount, sizeof(int)); //use as this differences[1000][2] [i][0] is difference [i][1] is count
    for(i=0; i<sync->numPeaks; i++) {
        int j;
        for(j=0; j<sync->peaks[i].numPeaks; j++) {
            int freq = sync->peaks[i].peaks[j];
            int k;
            for(k=0; k<freqs[freq].numPeaks; k++) {
                int dif = freqs[freq].peaks[k]-i;
                //add differnce to differnces
                if(dif == 0) //we need 0 to know if field is used or not... anyway it would be unusual to sync to perfect synced sounds...
                    continue;
                int c;
                for(c=0; c<diffCount; c++) {
                    if(differences[2*c] ==  dif) {
                        differences[2*c+1]++;
                        break;
                    }
                    if(differences[2*c] == 0) {
                        differences[2*c] = freq;
                        differences[2*c+1] = 1;
                        break;
                    }
                }
                if(c==diffCount); //Means that the array is full and there was another diffPoint found...
            }
        }
    }
    int max = 0;
    for(i=1; i<diffCount; i++) {
        if(differences[2*i+1] > differences[2*max+1])
            max = i;
    }
    return differences[2*max];

    free(differences);
    //Free all stuff!
    for(i = 0;i<original->configuration.fftPoints/2+1; i++) {
        if(freqs[i].numPeaks > 0)
            free(freqs[i].peaks); //maybe set peaks to zero?
    }
    free(freqs);
    free(differences);
}


//Free all resource of the fingerprint
void freeFingerprint(Fingerprint *fingerprint) {
    int i;
    for(i=0; i<fingerprint->numPeaks; i++) {
        free(fingerprint->peaks[i].peaks);
    }
    fftw_free(fingerprint->out);
    fftw_free(fingerprint->in);
    fftw_destroy_plan(fingerprint->plan);
    free(fingerprint->peaks);
    free(fingerprint);
}

