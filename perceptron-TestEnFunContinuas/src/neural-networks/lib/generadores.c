#include <stdlib.h>
#include <math.h>

#include "generadores.h"

/*Las siguientes constantes se utilizan en los generadores ran0, ran1, ran2 y ran3*/
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

#define IM1 2147483563
#define IM2 2147483399
#define AM1 (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NDIV1 (1+IMM1/NTAB)

#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)


/*Las siguientes variables globales se utilizan en los generadores mzran y mzran13*/
unlong X = 521288629;
unlong Y = 362436069;
unlong Z = 16163801;
unlong C = 1;
unlong N = 1131199209;
unlong M = 1131199229;

/*GENERADOR 1: MZRAN*/
unlong mzran(void) {
    long int s;

    s = X - Z;
    if (s < 0)
        s = s + 2147483579;

    X = Y;
    Y = Z;
    Z = s;
    M = 69069 * M + 1013904243;
    s = s + M;

    return s;
}

void mzran_set(unlong x, unlong y, unlong z, unlong m) {
    X = x;
    Y = y;
    Z = z;
    M = m;
}

/*Para devolver numeros aleatorios en (0,1)* /
float mzran_uni(void) {
    unsigned long int p = 4294967295;
    float div = (1.0 / p);

    return mzran() * div;
    ;
}/ * This code is bothering here.  */

/*GENERADOR 2: MZRAN13*/

unlong mzran13(void) {
    long int s;

    if (Y > X + C) {
        s = Y - (X + C);
        C = 0;
    } else {
        s = Y - (X + C) - 18;
        C = 1;
    }

    X = Y;
    Y = Z;
    Z = s;
    N = 69069 * N + 1013904243;
    s = s + N;

    return s;
}

void mzran13_set(unlong x, unlong y, unlong z, unlong n) {
    X = x;
    Y = y;
    Z = z;
    N = N;
}

/*Para devolver numeros aleatorios en (0,1)* /
float mzran13_uni(void) {
    unsigned long int p = 4294967295;
    float div = (1.0 / p);

    return mzran13() * div;
}/ * This code is bothering here.  */


/*GENERADOR 3: RAN0*/

/*
 * "Minimal" random number generator of Park and Miller. Returns a uniform random deviate
 * between 0.0 and 1.0. Set or reset idum to any integer value (except the unlikely value MASK)
 * to initialize the sequence; idum must not be altered between calls for successive deviates in
 * a sequence.
 */

float ran0(long *idum) {
    long k;
    float ans;

    *idum ^= MASK;
    k = (*idum) / IQ;
    *idum = IA * (*idum - k * IQ) - IR*k;
    if (*idum < 0) *idum += IM;
    ans = AM * (*idum);
    *idum ^= MASK;

    return ans;
}



/*GENERADOR 4: RAN1*/

/*
 * "Minimal" random number generator of Park and Miller with Bays-Durham shuffle and added
 * safeguards. Returns a uniform random deviate between 0.0 and 1.0 (exclusive of the endpoint
 * values). Call with idum a negative integer to initialize; thereafter, do not alter idum between
 * successive deviates in a sequence. RNMX should approximate the largest floating value that is
 * less than 1.
 */

float ran1(long *idum) {
    int j;
    long k;
    static long iy = 0;
    static long iv[NTAB];
    float temp;

    /*Inicializacion*/
    if (*idum <= 0 || !iy) {
        if (-(*idum) < 1) *idum = 1;
        else *idum = -(*idum);

        for (j = NTAB + 7; j >= 0; j--) {
            k = (*idum) / IQ;
            *idum = IA * (*idum - k * IQ) - IR*k;
            if (*idum < 0) *idum += IM;

            if (j < NTAB) iv[j] = *idum;
        }

        iy = iv[0];
    }

    k = (*idum) / IQ;
    *idum = IA * (*idum - k * IQ) - IR*k;
    if (*idum < 0) *idum += IM;

    j = iy / NDIV;
    iy = iv[j];
    iv[j] = *idum;

    if ((temp = AM * iy) > RNMX) return RNMX;

    return temp;
}



/*GENERADOR 5: RAN2*/

/*
 * Long period (> 2 × 1018) random number generator of L'Ecuyer with Bays-Durham shuffle
 * and added safeguards. Returns a uniform random deviate between 0.0 and 1.0 (exclusive of
 * the endpoint values). Call with idum a negative integer to initialize; thereafter, do not alter
 * idum between successive deviates in a sequence. RNMX should approximate the largest floating
 * value that is less than 1.
 */

float ran2(long *idum) {
    int j;
    long k;
    static long idum2 = 123456789;
    static long iy = 0;
    static long iv[NTAB];
    float temp;

    /*Inicializacion*/
    if (*idum <= 0) {
        if (-(*idum) < 1) *idum = 1;
        else *idum = -(*idum);
        idum2 = (*idum);

        for (j = NTAB + 7; j >= 0; j--) {
            k = (*idum) / IQ1;
            *idum = IA1 * (*idum - k * IQ1) - k*IR1;
            if (*idum < 0) *idum += IM1;

            if (j < NTAB) iv[j] = *idum;
        }

        iy = iv[0];
    }

    k = (*idum) / IQ1;
    *idum = IA1 * (*idum - k * IQ1) - k*IR1;
    if (*idum < 0) *idum += IM1;

    k = idum2 / IQ2;
    idum2 = IA2 * (idum2 - k * IQ2) - k*IR2;
    if (idum2 < 0) idum2 += IM2;

    j = iy / NDIV1;
    iy = iv[j] - idum2; /*Combinacion de los dos generadores!*/
    iv[j] = *idum;

    if (iy < 1) iy += IMM1;

    if ((temp = AM1 * iy) > RNMX) return RNMX;

    return temp;
}



/*GENERADOR 6: RAN3*/

/*
 * Returns a uniform random deviate between 0.0 and 1.0. Set idum to any negative value to
 * initialize or reinitialize the sequence.
 */

float ran3(long *idum) {
    static int inext, inextp;
    static long ma[56];
    static int iff = 0;
    long mj, mk;
    int i, ii, k;

    /*Inicializacion*/
    if (*idum < 0 || iff == 0) {
        iff = 1;
        mj = labs(MSEED - labs(*idum));
        mj %= MBIG;
        ma[55] = mj;
        mk = 1;

        for (i = 1; i <= 54; i++) {
            ii = (21 * i) % 55;
            ma[ii] = mk;
            mk = mj - mk;
            if (mk < MZ) mk += MBIG;
            mj = ma[ii];
        }

        for (k = 1; k <= 4; k++) {
            for (i = 1; i <= 55; i++) {
                ma[i] -= ma[1 + (i + 30) % 55];
                if (ma[i] < MZ) ma[i] += MBIG;
            }
        }

        inext = 0;
        inextp = 31;
        *idum = 1;
    }

    if (++inext == 56) inext = 1;
    if (++inextp == 56) inextp = 1;
    mj = ma[inext] - ma[inextp];
    if (mj < MZ) mj += MBIG;
    ma[inext] = mj;

    return mj*FAC;
}
