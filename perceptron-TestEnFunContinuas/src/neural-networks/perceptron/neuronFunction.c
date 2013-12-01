/*
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#include "neuronFunction.h"
#include <math.h>

double sig(double x);

double sig(double x) {
	if (x < 0.0)
		return -1.0;
	return 1.0;
}

double g(double h, neuralType tipo) {
	switch (tipo) {
	case lineal:
		return h;
	case seno:
		return sin(h);
	case pow2:
		return pow(h, 2.0);
	case hyp2:
		return 1.0 / (pow(h, 2.0) + .005);
	case ln2p:
		return log(pow(h, 2.0) + .05);
	default:
		/*(exp(h) - exp(-h)) / (exp(h) + exp(-h))*/
		return tanh(h);
	}
}

/**
 * derivada de g
 */
double dg(double h, neuralType tipo) {
	switch (tipo) {
	case lineal:
		return 1.0;
	case seno:
		return cos(h);
	case pow2:
		return 2.0 * h;
	case hyp2:
		return -(2.0 * h)
				/ (pow(h, 4.0) + .01 * pow(h, 2.0) + /** /1e-6/ **/2.5e-5/**/);
	case ln2p:
		return (2.0 * h) / (pow(h, 2.0) + .05);
	default:
		return 1.0 - pow(tanh(h), 2.0);
	}
}
