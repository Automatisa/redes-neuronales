#include <stdio.h>
#include "neurona.h"
#include "generadores.h"
#include <string.h>
#include <math.h>



int eNee = 416, p = 20, paso = 2, Estocastica = 0;
/**
 * BetaNeurona = 1/T
 */
double T = 0.4, betaNeurona = 2.5;


matrizBit_t inicializar(matrizBit_t S, matrizBit_t XI, int j);
int* inicializarMe(int* m, matrizBit_t XI, matrizBit_t S);
matrizBit_t memoria(void);
int* hopfield(matrizBit_t XI, int* m, int mu);
double medir_hopfield(void);

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
    static long sem = -71;
    if (XI) {
        matrizBit_t S = mb_create(1, eNee);
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
                int h = 0, Sold = mb_get_sig(S, 0, i);
                for (j = 0; j < p; j++)
                    h = h + (mb_get_sig(XI, i, j) * m[j]);
                h = h - (mb_get_sig(S, 0, i) * p);
                if (Estocastica) mb_set(S, 0, i, (ran2(&sem) <= (1.0 + tanh(((float) h) / (T * (float) eNee)))/2.0));
                else mb_set(S, 0, i, (0 <= h));
                for (j = 0; j < p; j++) {
                    int cambio = (mb_get_sig(S, 0, i) - Sold);
                    fin *= !cambio;
                    m[j] = m[j] + (mb_get_sig(XI, i, j) * cambio);
                }
            }
        }
        mb_destroy(S);
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
    acumVar = acumVar / (double) (j * eNee * eNee);
    if (Estocastica) printf("%f\t%f\n", T, media);
    else printf("%f\t%f\t%f\n", (float) p / (float) eNee, media, sqrt(acumVar - pow(media, 2.0)));
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
            paso = eNee / 208;
        } else if (i + 1 < argc && (!strcmp(argv[i], "-P") || !strcmp(argv[i], "-p"))) {
            i++;
            p = atoi(argv[i]);
        } else if (i + 1 < argc && (!strcmp(argv[i], "-E") || !strcmp(argv[i], "-e"))) {
            i++;
            Estocastica = atoi(argv[i]);
        } else {
            printf("No me gusto el \"%s\"\nDebe ingresar:\n\t-N[n] <num_de_neuronas>\n\t-P[p] <num_de_conceptos>\n\t-E[e] <(0->Det)|(1->Estocasticas)>\n", argv[i]);
            todo_bien = 0;
        }
        i++;
    }
    if (todo_bien) {
        if (Estocastica) {
            int k = 0;
            T = 0.0;
            for (k = 0; k < 12; k++) {
                T = T + 0.1;
                medir_hopfield();
            }
        } else for (p = paso; p <= eNee * 0.5; p += paso) medir_hopfield();
    }
    return EXIT_SUCCESS;
}
