#include "neurona.h"

/**
 * Notar que esta es la neurona de hopfield...
 * En hDiluido la neurona tendria que hacer algo como:
 * 
    int h = 0, j = 0;
    for (j = 0; j < mb_ncol(W)(esto seria conectividad de la neurona); j++)
        h += mi_get(W, i, j) * mb_get_sig(S, 0, Vecino(i,j));
    mb_set(S, 0, i, (0 <= h));
 */

void neuronaDet(int i, matrizBit_t S, matrizInt_t W) {
    int h = 0, j = 0;
    for (j = 0; j < mb_ncol(S); j++)
        h += mi_get(W, i, j) * mb_get_sig(S, 0, j);
    mb_set(S, 0, i, (0 <= h));
}

void neuronaEst(int i, matrizBit_t S, matrizInt_t W) {
    int h = 0, j = 0;
    static long sem, t;
    if (t != 15) {
        sem = -41;
        t = 15;
    }
    for (j = 0; j < mb_ncol(S); j++)
        h += mi_get(W, i, j) * mb_get_sig(S, 0, j);
    mb_set(S, 0, i, (ran2(&sem) <= (1.0 + tanh(betaNeurona * h)) / 2.0));
}

