/*
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#ifndef NEURONFUNCTION_H
#define NEURONFUNCTION_H

extern double betaNeuron;

typedef enum {
	tanhyp, lineal, hyp2, pow2, seno, ln2p
} neuralType;

double g(double h, neuralType tipo);
/**
 * derivada de g
 */
double dg(double h, neuralType tipo);

#endif
