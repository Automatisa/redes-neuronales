#include "matrizBit.h"
#include "generadores.h"
#include <stdlib.h>
#define cap 32

struct smatrizBit {
    int ncol;
    int cfila;
    int nfila;
    long* matrizB;
};

int mb_iabs(int i);
int mb_contBitSig(long x, int ncol, int b);
int mb_index(matrizBit_t S, int fila, int col);
int mb_filtro(long c, int index);

int mb_cap(void) {
    return cap;
}

int mb_iabs(int i) {
    if (i < 0) return -i;
    else return i;
}

matrizBit_t mb_create(int nfila, int ncol) {
    matrizBit_t res = NULL;
    nfila = mb_iabs(nfila);
    ncol = mb_iabs(ncol);
    res = (matrizBit_t) calloc(1, sizeof (struct smatrizBit));
    if (res) {
        res->ncol = ncol;
        res->nfila = nfila;
        res->cfila = ((ncol / cap) + 1);
        res->matrizB = (long*) calloc(nfila * res->cfila, sizeof (long));
    }
    return res;
}

matrizBit_t mb_destroy(matrizBit_t S) {
    if (S && S->matrizB) {
        free(S->matrizB);
        free(S);
    }
    return NULL;
}

int mb_ncol(matrizBit_t S) {
    if (S) return S->ncol;
    else return -1;
}

int mb_nfila(matrizBit_t S) {
    if (S) return S->nfila;
    else return -1;
}

int mb_filtro(long c, int bitIndex) {
    return (c >> bitIndex) &1;
}

int mb_index(matrizBit_t S, int fila, int col) {
    int res = -1;
    if (S && S->matrizB && fila < S->nfila && col < S->ncol)
        res = (fila * S->cfila)+(col / cap);
    return res;
}

int mb_get_sig(matrizBit_t S, int fila, int col) {
    int res = -1, i = mb_index(S, fila, col);
    if (-1 < i) {
        res = res + 2 * mb_filtro(S->matrizB[i], col % cap);
    }
    return res;
}

int mb_set(matrizBit_t S, int fila, int col, int b) {
    int res = b, i = mb_index(S, fila, col);
    if (-1 < i) {
        if (0 < b) {
            S->matrizB[i] |= (1 << (col % cap));
        } else {
            S->matrizB[i] &= ~(1 << (col % cap));
        }
    } else {
        res = -1;
    }
    return res;
}

int mb_SetRandFila(matrizBit_t S, int fila) {
    int mu = 0, rangof = mb_index(S, fila, 0), fin = mb_index(S, fila, S->ncol - 1) + 1;
    if (-1 < rangof) {
        for (mu = rangof; mu < fin; mu++)
            S->matrizB[mu] = mzran13();
    }
    return rangof;
}

int mb_contBitSig(long x, int ncol, int b) {
    int count, i = 0;
    if (!b) {
        for (count = 0; 0 < x; x >>= 1)
            if (x & 1) count++;
            else count--;
    } else {
        int ucol = ((ncol - 1) % cap) + 1;
        for (count = 0; 0 < x && i < ucol; x >>= 1) {
            i++;
            if (x & 1) count++;
            else count--;
        }
    }
    return count;
}

int mb_SumProdSignos(matrizBit_t S1, int fila1, matrizBit_t S2, int fila2) {
    /**ESTA Func esta para atraaaaaaaaaaaaaaaaaaaas.... VER TODO DE NUEVOOOO!!!!!;*/
    int res = 0, mu = 0;
    int rangof1 = mb_index(S1, fila1, 0), fin = mb_index(S1, fila1, S1->ncol - 1) + 1;
    int rangof2 = mb_index(S2, fila2, 0);
    if (-1 < rangof1 && -1 < rangof2 && S1->ncol == S2->ncol) {
        int ofsetFila = rangof2 - rangof1;
        for (mu = rangof1; mu < fin; mu++)
            res += mb_contBitSig(~(S1->matrizB[mu]^S2->matrizB[mu + ofsetFila]), S1->ncol, (mu == fin - 1));
    } else {
        res = -1;
    }
    return res;
}
