/*
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#include "neural-networks/lib/generadores.h"
#include "neural-networks/lib/otraLibMas.h"
#include "neural-networks/perceptron/perceptronNeuralNet.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

void pasarABitsYParidad(int cantMuest, int intATransformar, double* in, double* res);
void mezclameLasCartas(int *inEnInt, int p, long* sem);
int ejer1(int tamIn, int conMomento, int printRN);
int ejer2(int hypORln, int printRN);
int ejer3(void);

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
    	percnn_t red = NULL;
        int numNeuronas[3], paso = pow(2, tamIn - 3)
                , totalCombinaciones = pow(2, tamIn);
        neuralType tipoNeuronas[3];
        for (i = 0; i < 260; i++) {
            res[i] = (double*) calloc(1, sizeof (double));
            in[i] = (double*) calloc(8, sizeof (double));
        }
        numNeuronas[0] = tamIn;
        numNeuronas[1] = tamIn;
        numNeuronas[2] = 1;
        tipoNeuronas[0] = tanhyp;
        tipoNeuronas[1] = tanhyp;
        tipoNeuronas[2] = tanhyp;
        for (p = paso; p < totalCombinaciones; p = p + paso) {
            int ok = 2200, numDeIntentos = 500;
            acum = 0.0;
            tot = 0.0;
            mezclameLasCartas(inEnInt, totalCombinaciones, &sem);
            for (j = 0; j < 5000 && ok; j++) {
                int resto = ((j * p) % totalCombinaciones);
                if (totalCombinaciones <= resto + p)
                    mezclameLasCartas(inEnInt, totalCombinaciones, &sem);
                red = percnn_destroy(red);
                red = percnn_create(2, numNeuronas, tipoNeuronas, NULL, NULL, &sem);
                if (red) {
                    if (conMomento);
                    else percnn_set_alphaMoment(red, .0);
                    for (k = 0; k < p; k++)
                        pasarABitsYParidad(tamIn, inEnInt[k + resto], in[k], res[k]);
                    percnn_set_lineError(red, numDeIntentos, .45, .999, 0);
                    if (percnn_learn(red, p, in, res, batch)) {
                        tot = tot + 1.0;
                        pasarABitsYParidad(tamIn, inEnInt[k], in[0], res[0]);
                        test = percnn_eval(red, in[0]);
                        /** /if (dabs(res[0][0] - test[0]) < 1.0) acum = acum + 1.0;/ **/
						acum = acum + (dabs(res[0][0] - test[0]) / 2.0);
                        free(test);
                        if (j == 4999 && printRN) {
                            char *str = percnn_to_str(red);
                            printf("%s\n", str);
                            free(str);
                        }
                        red = percnn_destroy(red);
                    } else if (numDeIntentos < 5500) numDeIntentos += 2;
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

int ejer2(int hypORln, int printRN) {
    int i = 0, p = 20, conMomento = 1;
    double **res = NULL, **in = NULL, *test = NULL;
    long sem = -131;
    res = (double**) calloc(p, sizeof (double*));
    in = (double**) calloc(p, sizeof (double*));
    if (res && in) {
    	percnn_t red = NULL;
        int numNeuronas[3];
        neuralType tipoNeuronas[3];
        for (i = 0; i < p; i++) {
            res[i] = (double*) calloc(1, sizeof (double));
            in[i] = (double*) calloc(1, sizeof (double));
        }
        numNeuronas[0] = 1;
        numNeuronas[2] = 1;
        tipoNeuronas[0] = tanhyp;
        tipoNeuronas[1] = tanhyp;
        tipoNeuronas[2] = lineal;
        for (p = 5; p < 24; p *= 2) {
            if (hypORln) {
                numNeuronas[1] = 4;
                for (i = 0; i < p; i++) {
                    in[i][0] = 1.0 + ran2(&sem)*4.0;
                    res[i][0] = 1.0 / in[i][0];
                }
            } else {
                numNeuronas[1] = 6;
                for (i = 0; i < p; i++) {
                    in[i][0] = 1.0 + ran2(&sem)*4.0;
                    res[i][0] = log(in[i][0]);
                }
            }
            red = percnn_create(2, numNeuronas, tipoNeuronas, NULL, NULL, &sem);
            if (red) {
                double err = ((double) (numNeuronas[1] - 2)) / 200.0;
                int numIntentos = 0;
                if (!conMomento) percnn_set_alphaMoment(red, .0);
                percnn_set_lineError(red, 100000, err, err, 0);
                numIntentos = percnn_learn(red, p, in, res, online);
                if (numIntentos) {
                    if (p < 10) {
                        printf("f(x):\n\n");
                        for (i = 0; i < 100; i++) {
                            double x = 1.0 + ((double) i)*.04;
                            if (hypORln) printf("%f\t%f\n", x, 1.0 / x);
                            else printf("%f\t%f\n", x, log(x));
                        }
                    }
                    printf("\nIntentos: %i\nred(x): x in [1,5]: (solo %i puntos aleatorios)\n\n", numIntentos, p);
                    for (i = 0; i < p; i++) {
                        test = percnn_eval(red, in[i]);
                        printf("%f\t%f\n", in[i][0], test[0]);
                        free(test);
                    }
                    printf("\nred(x): x in [1,5]:\n\n");
                    for (i = 0; i < 100; i++) {
                        double x = 1.0 + ((double) i)*.04;
                        test = percnn_eval(red, &x);
                        printf("%f\t%f\n", x, test[0]);
                        free(test);
                    }
                    if (printRN) {
                        char *str = percnn_to_str(red);
                        printf("%s\n\n\n", str);
                        free(str);
                    }
                } else printf("No aprendí :-S :'(  TT__TT\n");
                red = percnn_destroy(red);
            } else printf("OO OOO\n");
        }
        p = 20;
        for (i = 0; i < p; i++) {
            free(res[i]);
            free(in[i]);
        }
        free(res);
        free(in);
    }
    return 0;
}

int ejer3(void) {
    int i = 0, j = 0, p = 8, conMomento = 1;
    double **res = NULL, **in = NULL;
    long sem = -131;
    res = (double**) calloc(p, sizeof (double*));
    in = (double**) calloc(p, sizeof (double*));
    if (res && in) {
    	percnn_t red = NULL;
        char strFiles[2][60] = {"../dat/redCodificadoraPract3Ejer3-8a3a8.rnp", "../dat/redCodificadoraPract3Ejer3-8a5a8.rnp"};
        int numNeuronas[3];
        neuralType tipoNeuronas[3];
        for (i = 0; i < p; i++) {
            res[i] = (double*) calloc(p, sizeof (double));
            in[i] = (double*) calloc(p, sizeof (double));
        }
        for (i = 0; i < p; i++)
            for (j = 0; j < p; j++) {
                if (i == j) {
                    in[i][j] = 1.0;
                    res[i][j] = 1.0;
                } else {
                    in[i][j] = -1.0;
                    res[i][j] = -1.0;
                }
            }
        numNeuronas[0] = 8;
        numNeuronas[2] = 8;
        tipoNeuronas[0] = tanhyp;
        tipoNeuronas[1] = tanhyp;
        tipoNeuronas[2] = tanhyp;
        for (p = 0; p < 2; p++) {
            numNeuronas[1] = 3 + p * 2;
            red = percnn_create(2, numNeuronas, tipoNeuronas, NULL, strFiles[p], &sem);
            if (red) {
                int numIntentos = 0;
                if (!conMomento) percnn_set_alphaMoment(red, .0);
                percnn_set_lineError(red, 100000, .015, .0199, 0);
                numIntentos = percnn_learn(red, 8, in, res, online);
                if (numIntentos) {
                    char *str = NULL;
                    str = percnn_to_str(red);
                    percnn_save_plaintext(red, NULL);
                    printf("Num de Intentos: %i\n%s\n\n", numIntentos, str);
                    free(str);
                } else printf("No aprendí :-S :'(  TT__TT\n");
                red = percnn_destroy(red);
            } else printf("OO OOO\n");
        }
        p = 8;
        for (i = 0; i < p; i++) {
            free(res[i]);
            free(in[i]);
        }
        free(res);
        free(in);
    }
    return 0;
}

int main(int argc, char** argv) {
    int i = 1, ejer = 0, todo_bien = 1, momento = 0, tam = 1, printRN = 0;
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
        } else if (!strcmp(argv[i], "-P") || !strcmp(argv[i], "-p") ||
                !strcmp(argv[i], "-V") || !strcmp(argv[i], "-v")) {
            i++;
            printRN = 1;
        } else {
            printf("No me gusto el \"%s\"\nDebe ingresar:\n\t-E[n] <num_de_ejer>\n\n", argv[i]);
            todo_bien = 0;
        }
        i++;
    }
    if (todo_bien) {
        switch (ejer) {
            case 2:
                ejer2(tam, printRN);
                break;
            case 3:
                ejer3();
                break;
            default:
                if (tam < 4) tam = 4;
                ejer1(tam, momento, printRN);
                break;
        }
    }
    return EXIT_SUCCESS;
}
