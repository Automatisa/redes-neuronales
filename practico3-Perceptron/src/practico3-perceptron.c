/*
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#include "generadores.h"
#include "redPerceptron.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

double dabs2(double x);
void pasarABitsYParidad(int cantMuest, int intATransformar, double* in, double* res);
void mezclameLasCartas(int *inEnInt, int p, long* sem);
int ejer1(int tamIn, int conMomento, int printRN);
int ejer2(int tamIn, int cantMuest, int conMomento, int printRN);
int ejer3(int tamIn, int conMomento, int printRN);

double dabs2(double x) {
    if (x < 0.0) return -x;
    return x;
}

void mezclameLasCartas(int *inEnInt, int p, long* sem) {
    int i = 0;
    for (i = 0; i < p; i++)
        inEnInt[i] = i;
    for (i = p; 0 < i; i--) {
        int tmp = 0, j = ran2(sem) * i;
        tmp = inEnInt[j];
        inEnInt[j] = inEnInt[i - 1];
        inEnInt[i - 1] = tmp;
    }
}

void pasarABitsYParidad(int cantMuest, int intATransformar, double* in, double* res) {
    int i = 0;
    *res = -1.0;
    for (i = 0; i < cantMuest; i++) {
        if (intATransformar % 2) {
            in[i] = 1.0;
            if (0.0 < *res) *res = -1.0;
            else *res = 1.0;
        } else {
            in[i] = -1.0;
        }
        intATransformar = intATransformar / 2;
    }
}

int ejer1(int tamIn, int conMomento, int printRN) {
    int i = tamIn, p = 0, j = 0, k = 0, inEnInt[260];
    double **res = NULL, **in = NULL, *test = NULL, acum = 0.0, tot = 0.0;
    long sem = -31;
    res = (double**) calloc(260, sizeof (double*));
    in = (double**) calloc(260, sizeof (double*));
    if (res && in) {
        rperc_t red = NULL;
        int numNeuronas[3], tipoNuronas[3], paso = pow(2, tamIn - 3)
                , totalCombinaciones = pow(2, tamIn);
        for (i = 0; i < 260; i++) {
            res[i] = (double*) calloc(1, sizeof (double));
            in[i] = (double*) calloc(8, sizeof (double));
        }
        numNeuronas[0] = tamIn;
        numNeuronas[1] = tamIn;
        numNeuronas[2] = 1;
        tipoNuronas[0] = 0;
        tipoNuronas[1] = 0;
        tipoNuronas[2] = 0;
        for (p = paso; p < totalCombinaciones; p = p + paso) {
            int ok = 1000;
            acum = 0.0;
            tot = 0.0;
            mezclameLasCartas(inEnInt, totalCombinaciones, &sem);
            for (j = 0; j < 5000 && ok; j++) {
                int resto = ((j * p) % totalCombinaciones), numDeIntentos = 300;
                red = rperc_create(2, numNeuronas, tipoNuronas, NULL, &sem);
                if (totalCombinaciones <= resto + p)
                    mezclameLasCartas(inEnInt, totalCombinaciones, &sem);
                if (red) {
                    for (k = 0; k < p; k++)
                        pasarABitsYParidad(tamIn, inEnInt[k + resto], in[k], res[k]);
                    if (rperc_aprender(red, numDeIntentos, p, in, res, conMomento, batch)) {
                        tot = tot + 1.0;
                        pasarABitsYParidad(tamIn, inEnInt[k], in[0], res[0]);
                        test = rperc_eval(red, in[0]);
                        if (dabs2(res[0][0] - test[0]) < 1.0) acum = acum + 1.0;
                        free(test);
                        if (j == 4999 && printRN) {
                            char *str = rperc_to_str(red);
                            printf("%s\n", str);
                            free(str);
                        }
                        red = rperc_destroy(red);
                    } else if (numDeIntentos < 3100) numDeIntentos += 3;
                    else if (ok) ok--;
                } else printf("OO OOO\n");
            }
            if (ok) {
                acum = acum / tot;
                printf("%f\t%f\n", ((double) p) / ((double) totalCombinaciones), acum);
            } else printf("FAIL!!!\n");
        }
        for (i = 0; i < 260; i++) {
            free(res[i]);
            free(in[i]);
        }
        free(res);
        free(in);
    }
    return 0;
}

int ejer2(int tamIn, int cantMuest, int conMomento, int printRN) {
    int i = 0, p = 0, j = 0, k = 0, inEnInt[256/*cantMuest*/];
    double **res = NULL, **in = NULL, *test = NULL, acum = 0.0, tot = 0.0;
    long sem = -31;
    res = (double**) calloc(cantMuest, sizeof (double*));
    in = (double**) calloc(cantMuest, sizeof (double*));
    if (res && in) {
        rperc_t red = NULL;
        int numNeuronas[3], tipoNuronas[3]/*, paso = pow(2, cantMuest - 3)*/
                , totalCombinaciones = pow(2, cantMuest), ok = 1000;
        for (i = 0; i < cantMuest; i++) {
            res[i] = (double*) calloc(1, sizeof (double));
            in[i] = (double*) calloc(1, sizeof (double));
        }
        numNeuronas[0] = 1;
        if (tamIn)
            numNeuronas[1] = 4;
        else
            numNeuronas[1] = 6;
        numNeuronas[2] = 1;
        tipoNuronas[0] = 0;
        tipoNuronas[1] = 0;
        tipoNuronas[2] = 1;
        /** /for (p = 5; p < 21; p *= 2) {*/
            acum = 0.0;
            tot = 0.0;
            /* Esto aca no hace falta deberia ser f(x) x[1;5] */
            mezclameLasCartas(inEnInt, totalCombinaciones, &sem);
            for (j = 0; j < 5000 && ok; j++) {
                int resto = ((j * p) % totalCombinaciones), numDeIntentos = 300;
                red = rperc_create(2, numNeuronas, tipoNuronas, NULL, &sem);
                if (totalCombinaciones <= resto + p)
                    mezclameLasCartas(inEnInt, totalCombinaciones, &sem);
                if (red) {
                    for (k = 0; k < p; k++)
                        pasarABitsYParidad(cantMuest, inEnInt[k + resto], in[k], res[k]);
                    if (rperc_aprender(red, numDeIntentos, p, in, res, conMomento, online)) {
                        tot = tot + 1.0;
                        pasarABitsYParidad(cantMuest, inEnInt[k], in[0], res[0]);
                        test = rperc_eval(red, in[0]);
                        if (dabs2(res[0][0] - test[0]) < 1.0) acum = acum + 1.0;
                        free(test);
                        if (j == 4999 && printRN) {
                            char *str = rperc_to_str(red);
                            printf("%s\n", str);
                            free(str);
                        }
                        red = rperc_destroy(red);
                    } else if (numDeIntentos < 3100) numDeIntentos += 3;
                    else if (ok) ok--;
                } else printf("OO OOO\n");
            }
            if (ok) {
                acum = acum / tot;
                printf("%f\t%f\n", ((double) p) / ((double) totalCombinaciones), acum);
            } else printf("FAIL!!!\n");
        /*}*/
        for (i = 0; i < cantMuest; i++) {
            free(res[i]);
            free(in[i]);
        }
        free(res);
        free(in);
    }
    return 0;
}

int ejer3(int tamIn, int conMomento, int printRN) {
    return 0;
}

int main(int argc, char** argv) {
    int i = 1, ejer = 0, todo_bien = 1, momento = 0, tam = -1, printRN = 0, cantMuest = 0;
    while (todo_bien && i + 1 < argc) {
        if (!strcmp(argv[i], "-E") || !strcmp(argv[i], "-e")) {
            i++;
            ejer = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-N") || !strcmp(argv[i], "-n")) {
            i++;
            tam = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-M") || !strcmp(argv[i], "-m")) {
            i++;
            momento = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-K") || !strcmp(argv[i], "-k")) {
            i++;
            cantMuest = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-P") || !strcmp(argv[i], "-p") ||
                !strcmp(argv[i], "-V") || !strcmp(argv[i], "-v")) {
            i++;
            printRN = 1;
        } else {
            printf("No me gusto el \"%s\"\nDebe ingresar:\n\t-N[n] <num_de_ejer>\n\n", argv[i]);
            todo_bien = 0;
        }
        i++;
    }
    if (todo_bien) {
        switch (ejer) {
            case 2:
                ejer2(tam, cantMuest, momento, printRN);
                break;
            case 3:
                ejer3(tam, momento, printRN);
                break;
            default:
                if (tam < 4) tam = 4;
                ejer1(tam, momento, printRN);
                break;
        }
    }
    return EXIT_SUCCESS;
}
