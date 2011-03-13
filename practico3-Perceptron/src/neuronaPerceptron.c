/*
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#include "neuronaPerceptron.h"
#include <math.h>

double sig(double x);

double sig(double x) {
    if (x < 0.0) return -x;
    return x;
}

double g(double h, int tipo) {
    switch (tipo) {
        case 1:
            return h;
        case 2:
            /*sig((exp(h) - exp(-h)) / (exp(h) + exp(-h)))*/
            return sig(tanh(h));
        default:
            /*(exp(h) - exp(-h)) / (exp(h) + exp(-h))*/
            return tanh(h);
    }
}

/**
 * derivada de g
 */
double dg(double h, int tipo) {
    switch (tipo) {
        case 1:
            return 1.0;
        case 2:
            return 1.0 - pow(tanh(h), 2.0);
        default:
            return 1.0 - pow(tanh(h), 2.0);
    }
}
