#ifndef NEURONAPERS_H
#define NEURONAPERS_H

/*
#include "matrizBit.h"
#include "matrizInt.h"

void neuronaDet(int i, matrizBit_t S, matrizInt_t W);
void neuronaEst(int i, matrizBit_t S, matrizInt_t W);

extern double betaNeurona;*/

double g(double h,int tipo);
/**
 * derivada de g
 */
double dg(double h,int tipo);

#endif
