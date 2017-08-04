#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include "tcb.h"



#define IMAGE_DATA_PTR     (*(Data*)captureData).imageDataPtr
#define IMAGE_DATA_RAW_PTR (*(Data*)captureData).imageDataRawPtr
#define REAL_X_PTR         (*(Data*)captureData).xPtr
#define IMAG_W_PTR  	   (*(Data*)captureData).wPtr


void fft(COMPLEX*, COMPLEX*, unsigned int);
int terminal0;


void imageCapture(void* captureData) {
    unsigned int f;
    unsigned int fs = 8;
    unsigned int m_index;
    unsigned int N;
    unsigned int real[256];
    unsigned int imag[256];
    unsigned int frequency[16];
    int maxMagSqu = 0;
    int MagSqu = 0;
    
    FILE* ain5;
    int i;
    clock_t start_t, end_t;
    for(i = 0; i < 256; i++) {
    	ain5 = fopen("/sys/devices/ocp.3/helper.15/AIN5", "r");
		fseek(ain5,0,SEEK_SET); // go to beginning of buffer
	    fscanf(ain5, "%u", &real[i]); // write analog value to buffer
    	fclose(ain5);
    }

    //fft(&real[0], &img[0], 8);

    for (i=0; i<256;i++) {
    	&real[i] = *(REAL_X_PTR+i);
    }
    for (i=0; i<256;i++) {
    	&imag[i] = *(IMAG_W_PTR+i);
    }

    //find the index which corresponds to the maximum magnitude
    for (i=0; i<256;i++) {
    	MagSqu = real[i]*real[i]+imag[i]*imag[i];
    	if (MagSqu > maxMagSqu) {
    		maxMagSqu = MagSqu;
    		m_index = i;
    	}
    }

    N = 1 << 8;
    f=fs*m_index/N;

    //The 16 most recent frequency values retained in a buffer
    for(i=15; i>0, i--) {
        frequency[i] = frequency[i-1];
    }
    frequency[0] = f;
    dprintf(terminal0, "Image frequency is %f\n", f);

    //initialize the input imaginary buffer to zero
    for (i=0; i<256; i++) {
        imag[i] = 0;
    }

    return NULL;
}


void fft(COMPLEX *x, COMPLEX *w, unsigned int m)
{
	 COMPLEX u,temp,tm;
	 COMPLEX *xi,*xip,*xj,*wptr;

	 unsigned int n;

	 unsigned int i,j,k,l,le,windex;

	 float arg,w_real,w_imag,wrecur_real,wrecur_imag,wtemp_real;

	 /* n = 2**m = fft length */

	 n = 1 << m;
	 le = n/2;


	 /* calculate the w values recursively */

	 arg = 4.0*atan(1.0)/le;         /* PI/le calculation */
	 wrecur_real = w_real = cos(arg);
	 wrecur_imag = w_imag = -sin(arg);
	 xj = w;
	 for (j = 1 ; j < le ; j++) {
		xj->real = (float)wrecur_real;
		xj->imag = (float)wrecur_imag;
		xj++;
		wtemp_real = wrecur_real*w_real - wrecur_imag*w_imag;
		wrecur_imag = wrecur_real*w_imag + wrecur_imag*w_real;
		wrecur_real = wtemp_real;
	 }


/* start fft */

	 le = n;
    windex = 1;
    for (l = 0 ; l < m ; l++) {
        le = le/2;

/* first iteration with no multiplies */

        for(i = 0 ; i < n ; i = i + 2*le) {
            xi = x + i;
            xip = xi + le;
            temp.real = xi->real + xip->real;
            temp.imag = xi->imag + xip->imag;
            xip->real = xi->real - xip->real;
            xip->imag = xi->imag - xip->imag;
            *xi = temp;
        }

/* remaining iterations use stored w */

        wptr = w + windex - 1;
        for (j = 1 ; j < le ; j++) {
            u = *wptr;
            for (i = j ; i < n ; i = i + 2*le) {
                xi = x + i;
                xip = xi + le;
                temp.real = xi->real + xip->real;
                temp.imag = xi->imag + xip->imag;
                tm.real = xi->real - xip->real;
					 tm.imag = xi->imag - xip->imag;
                xip->real = tm.real*u.real - tm.imag*u.imag;
                xip->imag = tm.real*u.imag + tm.imag*u.real;
                *xi = temp;
            }
            wptr = wptr + windex;
        }
        windex = 2*windex;
    }            

/* rearrange data by bit reversing */

    j = 0;
    for (i = 1 ; i < (n-1) ; i++) {
        k = n/2;
        while(k <= j) {
            j = j - k;
            k = k/2;
        }
        j = j + k;
        if (i < j) {
            xi = x + i;
            xj = x + j;
            temp = *xj;
            *xj = *xi;
            *xi = temp;
        }
    }
}


