#ifndef NEURONA_H
#define NEURONA_H

#include "matrizBit.h"
#include "matrizInt.h"
#include <math.h>
#include "generadores.h"

void neuronaDet(int i, matrizBit_t S, matrizInt_t W);
void neuronaEst(int i, matrizBit_t S, matrizInt_t W);

extern double betaNeurona;

#endif
