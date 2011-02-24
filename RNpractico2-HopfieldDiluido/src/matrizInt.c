#include "matrizInt.h"
#include <stdlib.h>

struct smatrizInt {
    int ncol;
    int nfila;
    int* matriz;
};

int mi_iabs(int i);

int mi_iabs(int i) {
    if (i < 0) return -i;
    else return i;
}

matrizInt_t mi_create(int nfila, int ncol) {
    matrizInt_t res = NULL;
    nfila = mi_iabs(nfila);
    ncol = mi_iabs(ncol);
    res = (matrizInt_t) calloc(1, sizeof (struct smatrizInt));
    if (res) {
        res->ncol = ncol;
        res->nfila = nfila;
        res->matriz = (int*) calloc((ncol * nfila), sizeof (int));
    }
    return res;
}

int mi_ncol(matrizInt_t W) {
    if (W) return W->ncol;
    else return -1;
}

int mi_nfila(matrizInt_t W) {
    if (W) return W->nfila;
    else return -1;
}

int mi_get(matrizInt_t W, int fila, int col) {
    int res = -1;
    if (fila < W->nfila && col < W->ncol)
        res = W-> matriz[(fila * W->ncol) + col];
    return res;
}

int mi_set(matrizInt_t W, int fila, int col, int val) {
    int res = val;
    if (W && W->matriz && fila < W->nfila && col < W->ncol) {
        W->matriz[(fila * W->ncol) + col] = val;
    } else {
        res = -1;
    }
    return res;
}
