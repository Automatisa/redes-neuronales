#include <stdio.h>
#include "neurona.h"
#include "generadores.h"
#include <string.h>
#include <math.h>


int eNee = 2500, p = 0, conec = 20;
/**
 * Beta de neurona=1/t
 */
double betaNeurona = .5;

matrizBit_t inicializar(matrizBit_t S, matrizBit_t XI, int j);
int* inicializarMe(int* m, matrizBit_t XI, matrizBit_t S);
matrizBit_t memoria(void);
int* hopfield(matrizBit_t XI, int* m, int mu);
double medir_hopfield(void);
matrizInt_t selecVecino(void);
matrizInt_t aprenderXI(matrizBit_t XI, matrizInt_t v);
int es_buen_vecino(matrizInt_t v, int i, int j, int vecN);

int es_buen_vecino(matrizInt_t v, int i, int j, int vecN) {
    int res = (i == vecN), k = 0;
    for (k = 0; !res && k < j; k++)
        res = (mi_get(v, i, k) == vecN);
    return res;
}

matrizInt_t selecVecino(void) {
    matrizInt_t v = NULL;
    int i = 0, j = 0;
    long sem = -41;
    v = mi_create(eNee, conec);
    if (v) {
        for (i = 0; i < eNee; i++) {
            for (j = 0; j < conec; j++) {
                int vecN = (int) (eNee * ran2(&sem));
                while (es_buen_vecino(v, i, j, vecN))
                    vecN = (int) (eNee * ran2(&sem));
                mi_set(v, i, j, vecN);
            }
        }
    }
    return v;
}

matrizInt_t aprenderXI(matrizBit_t XI, matrizInt_t v) {
    matrizInt_t w = NULL;
    int i = 0, j = 0, k = 0;
    w = mi_create(eNee, conec);
    if (w) {
        for (i = 0; i < eNee; i++)
            for (j = 0; j < conec; j++) {
                mi_set(w, i, j, 0);
                for (k = 0; k < p; k++)
                    mi_set(w, i, j, mi_get(w, i, j)
                        + (mb_get_sig(XI, i, k)
                        * mb_get_sig(XI, mi_get(v, i, j), k)));
            }
    }
    return w;
}

int* inicializarMe(int* m, matrizBit_t XI, matrizBit_t S) {
    int i = 0, j = 0;
    for (j = 0; j < p; j++) {
        m[j] = 0;
        for (i = 0; i < eNee; i++)
            m[j] = m[j] + (mb_get_sig(XI, i, j) * mb_get_sig(S, 0, i));
    }
    return m;
}

matrizBit_t inicializar(matrizBit_t S, matrizBit_t XI, int j) {
    if (XI) {
        int i = 0;
        long sem = -73;
        if (!S) S = mb_create(1, eNee);
        for (i = 0; i < eNee; i++) {
            if (ran2(&sem) < 0.15) mb_set(S, 0, i, -mb_get_sig(XI, i, j));
            else mb_set(S, 0, i, mb_get_sig(XI, i, j));
        }
    }
    return S;
}

matrizBit_t memoria() {
    int i = 0;
    matrizBit_t XI = mb_create(eNee, p);
    for (i = 0; i < eNee; i++)
        mb_SetRandFila(XI, i);
    return XI;
}

int* hopfield(matrizBit_t XI, int* m, int mu) {
    int i = 0, j = 0, x = 0, fin = 0, k = 0;
    if (XI) {
        matrizBit_t S = mb_create(1, eNee);
        if (S) {
            matrizInt_t vecino = NULL, w = NULL;
            /* TODO: clavar sinapsis de vecinos */
            vecino = selecVecino();
            if (vecino) {
                /* TODO: clavar sinapsis de los W pesos vs XI */
/**
* TODO: VER QUE LOS PESOS SON ENTEROS...
* POSIBLE FUENTE DE ERRORES!!!
 */
                w = aprenderXI(XI, vecino);
                if (w) {
                    S = inicializar(S, XI, mu);
                    m = inicializarMe(m, XI, S);
                    for (x = 0; x < eNee / 20 && !fin; x++) {
                        fin = 1;
                        if (!k && x == 20) {
                            k = 1;
                            x = 0;
                            m = inicializarMe(m, XI, S);
                        }
                        for (i = 0; i < eNee; i++) {
                            int h = 0, Sold = mb_get_sig(S, 0, i), cambio = 0;
                            for (j = 0; j < conec; j++)
                                h = h + (mb_get_sig(S, 0, mi_get(vecino, i, j)) * mi_get(w, i, j));
                            mb_set(S, 0, i, (0 <= h));
                            cambio = (mb_get_sig(S, 0, i) - Sold);
                            fin *= !cambio;
                            for (j = 0; j < p; j++) {
                                m[j] = m[j] + (mb_get_sig(XI, i, j) * cambio);
                            }
                        }
                    }
                }
            }
            mb_destroy(S);
        }
    }
    return m;
}

double medir_hopfield() {
    int j = 0;
    double media = 0.0, acumVar = 0.0;
    int* m = (int*) calloc(p, sizeof (int));
    matrizBit_t XI = NULL;
    XI = memoria();
    for (j = 0; j < 30; j++) {
        m = hopfield(XI, m, j % p);
        media = media + (float) m[j % p];
        acumVar = acumVar + pow((float) m[j % p], 2.0);
    }
    media = media / (double) (j * eNee);
    acumVar = ((acumVar / (double) j) / (double) eNee) / (double) eNee;
    printf("%f\t%f\t%f\n", (float) p / (float) conec, media, sqrt(acumVar - pow(media, 2.0)));
    free(m);
    XI = mb_destroy(XI);
    return media;
}

int main(int argc, char** argv) {
    int i = 1, todo_bien = 1;
    while (todo_bien && i < argc) {
        if (i + 1 < argc && (!strcmp(argv[i], "-N") || !strcmp(argv[i], "-n"))) {
            i++;
            eNee = atoi(argv[i]);
        } else {
            printf("No me gusto el \"%s\"\nDebe ingresar:\n\t-N[n] <num_de_neuronas>\n\n", argv[i]);
            todo_bien = 0;
        }
        i++;
    }
    if (todo_bien) {
        for (p = 1; p <= 20; p++) medir_hopfield();
    }
    return EXIT_SUCCESS;
}
