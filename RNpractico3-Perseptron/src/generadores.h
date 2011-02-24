#ifndef GENERADORES_H
#define GENERADORES_H

#include <stdlib.h>
#include <math.h>

typedef unsigned long int unlong;

unlong
mzran(void);

void
mzran_set(unlong x, unlong y, unlong z, unlong m);

float
mzran_uni(void);

unlong
mzran13(void);

void
mzran13_set(unlong x, unlong y, unlong z, unlong n);

float
mzran13_uni(void);

float
ran0(long *idum);

float
ran1(long *idum);

float
ran2(long *idum);

float
ran3(long *idum);

#endif /*GENERADORES_H*/
