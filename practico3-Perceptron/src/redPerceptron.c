/**
 * File:   capaPerceptron.c
 *
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#include "redPerceptron.h"
#include "neuronaPerceptron.h"
#include "../lib/generadores.h"
#include "../lib/otraLibMas.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct sCapaPerc *cperc_t;

struct sCapaPerc {
    int neuralIn, neuralOut, tipo;
    cperc_t nextCap, prevCap;
    double *out, *delta, **W, **dWmomento;
};

cperc_t cperc_create(int neuralIn, int neuralOut, int tipo, cperc_t nextC,
        cperc_t prevC, long *sem);
cperc_t cperc_destroy(cperc_t cap);
cperc_t cperc_from_str(char *strCap);
char *cperc_to_str(cperc_t cap);
double *cperc_eval(cperc_t cap, int hacerDerivada);
void cperc_aprender(cperc_t cap, double *deseado, double alfaMomento, double nu);
double *rperc_evalX(rperc_t red, double *in, int aprenderONo);
double *rperc_evalParaAprender(rperc_t red, double **in, int *selec, int point);
void rperc_adaptarNu(rperc_t red, double newError);
int rperc_rectaError(rperc_t red, int len, double* res, double* deseado,
        double *retErr);
void rperc_aprender_online(rperc_t red, int cantDatos, double **in,
        double **deseado, int *selec);
void rperc_aprender_batch(rperc_t red, int cantDatos, double **in,
        double **deseado, int *selec);

/**
 * TODO: TEEEENEEEER EEEN CUENTAAAAAA!!!!!!!!!!!
 * La neurona de entrada 0 sienpre es -1.
 * Poner un lugar mas a la entrada.
 */
cperc_t cperc_create(int neuralIn, int neuralOut, int tipo, cperc_t nextC,
        cperc_t prevC, long *sem) {
    cperc_t res = NULL;
    res = (cperc_t) calloc(1, sizeof (struct sCapaPerc));
    if (res) {
        int i = 0;
        res->neuralIn = neuralIn;
        res->neuralOut = neuralOut;
        res->tipo = tipo;
        res->nextCap = nextC;
        res->prevCap = prevC;
        res->out = (double *) calloc(neuralOut, sizeof (double));
        res->delta = (double *) calloc(neuralOut, sizeof (double));
        res->W = (double **) calloc(neuralOut, sizeof (double *));
        res->dWmomento = (double **) calloc(neuralOut, sizeof (double *));
        if (res->W && res->dWmomento && res->out && res->delta) {
            int ok = 1;
            for (i = 0; ok && i < neuralOut; i++) {
                res->delta[i] = .0;
                res->out[i] = .0;
                res->W[i] = NULL;
                res->dWmomento[i] = NULL;
                res->W[i] = (double *) calloc(neuralIn + 1, sizeof (double));
                res->dWmomento[i] = (double *) calloc(neuralIn + 1, sizeof (double));
                if (res->W[i] && res->dWmomento[i]) {
                    int j = 0;
                    for (j = 0; j < neuralIn + 1; j++) {
                        res->W[i][j] = (ran2(sem) - .5) / (double) (neuralIn + 1);
                        res->dWmomento[i][j] = .0;
                    }
                } else {
                    ok = 0;
                    res = cperc_destroy(res);
                }
            }
        } else {
            res = cperc_destroy(res);
        }
    }
    return res;
}

cperc_t cperc_destroy(cperc_t cap) {
    if (cap) {
        int i = 0;
        free(cap->delta);
        free(cap->out);
        if (cap->W) {
            for (i = 0; i < cap->neuralOut; i++)
                if (cap->W[i])
                    free(cap->W[i]);
            free(cap->W);
        }
        if (cap->dWmomento) {
            for (i = 0; i < cap->neuralOut; i++)
                if (cap->dWmomento[i])
                    free(cap->dWmomento[i]);
            free(cap->dWmomento);
        }
        if (cap->nextCap)
            cap->nextCap->prevCap = NULL;
        cperc_destroy(cap->nextCap);
        cperc_destroy(cap->prevCap);
        free(cap);
    }
    return NULL;
}

cperc_t cperc_from_str(char *strCap) {
    return NULL;
}

char *cperc_to_str(cperc_t cap) {
    char *res = NULL;
    int l = 0;
    if (cap) {
        l = 275;
        res = (char *) calloc(l, sizeof (char));
        if (res) {
            int i = 0, j = 0;
            l = sprintf(res, "Tipo: %i\nNumOut: %i\tNumIn: %i\n", cap->tipo,
                    cap->neuralOut, cap->neuralIn);
            for (i = 0; i < cap->neuralOut && res; i++) {
                res = myVarStrCat(res, "%i\t[ ", i);
                for (j = 0; j <= cap->neuralIn && res; j++)
                    if (j == cap->neuralIn) {
                        res = myVarStrCat(res, "UMBRAL: %g ]\n", cap->W[i][j]);
                    } else {
                        res = myVarStrCat(res, "%g,\t", cap->W[i][j]);
                    }
            }
        }
    }
    return res;
}

double *cperc_eval(cperc_t cap, int hacerDerivada) {
    /* aca calcular la G() -> en V[i] */
    /* y calcular G'() -> en err[i] derivada de G() */
    double *res = NULL;
    if (cap) {
        if (cap->prevCap) {
            int i = 0, j = 0;
            for (i = 0; i < cap->neuralOut; i++) {
                cap->out[i] = -cap->W[i][cap->neuralIn];
                for (j = 0; j < cap->neuralIn; j++)
                    cap->out[i] = cap->out[i] + (cap->W[i][j] * cap->prevCap->out[j]);
                if (hacerDerivada)
                    cap->delta[i] = dg(/*2.0 **/ cap->out[i], cap->tipo);
                /* TODO: Evaluar posibilidad de si es tipo 0
                 * y es la ultima capa, usar out=sig(g)
                 * de lo contrario arreglar el problema de satisfaccion
                 * en el aprendizage a nivel red y no a nivel capa
                 * TODO: ver que aca el beta es fijo.
                 */
                cap->out[i] = g(/*2.0 **/ cap->out[i], cap->tipo);
            }
        }
        if (cap->nextCap)
            res = cperc_eval(cap->nextCap, hacerDerivada);
        else {
            int i = 0;
            res = (double *) calloc(cap->neuralOut, sizeof (double));
            if (res)
                for (i = 0; i < cap->neuralOut; i++)
                    res[i] = cap->out[i];
        }
    }
    return res;
}

void cperc_aprender(cperc_t cap, double *deseado, double alfaMomento, double nu) {
    if (cap && cap->prevCap) {
        int fail = 0, i = 0, j = 0;
        if (cap->nextCap) {
            /*aca va lo de la capa no final */
            cperc_t nc = cap->nextCap;
            for (i = 0; i < cap->neuralOut; i++) {
                double d = .0;
                for (j = 0; j < nc->neuralOut; j++)
                    d = d + nc->W[j][i] * nc->delta[j];
                cap->delta[i] = cap->delta[i] * d;
            }
        } else {
            if (deseado) {
                /*aca va lo de la capa final */
                for (i = 0; i < cap->neuralOut; i++)
                    cap->delta[i] = cap->delta[i] * (deseado[i] - cap->out[i]);
            } else {
                fail = 1;
            }
        }
        if (!fail) {
            cperc_aprender(cap->prevCap, NULL, alfaMomento, nu);
            /**
             * aca biene el famoso nu del dW, el alfa de dWmomentum
             * Agregar tambien a y b de nu adaptativo.
             * TODO: ver seccion de funcion de costo alternativas (apuntes y libro)
             * nu optimo segun el libro ~ .0476
             * alfa segun el libro ~ .5 (~ 10*nu)
             * para este nu usaremos a=.0003 y b=.09
             */
            for (i = 0; i < cap->neuralOut; i++) {
                j = cap->neuralIn;
                cap->dWmomento[i][j] = nu * -cap->delta[i] + alfaMomento
                        * cap->dWmomento[i][j];
                cap->W[i][j] = cap->W[i][j] + cap->dWmomento[i][j];
                for (j = 0; j < cap->neuralIn; j++) {
                    cap->dWmomento[i][j] = nu * cap->prevCap->out[j]
                            * cap->delta[i] + alfaMomento * cap->dWmomento[i][j];
                    cap->W[i][j] = cap->W[i][j] + cap->dWmomento[i][j];
                }
            }
            /**/
        }
    }
}

struct sRedPerc {
    int numCap, numRealIn, tipoErr, totIntentos, intRestantes, descending,
            calentContext;
    long *sem;
    double nu, alfaMomento, oldError, maxNu, minNu, upNu, downNu, *lastErr,
            *muRetenContex, maxErr, minErr;
    cperc_t first, last;
    char *strFile;
};

/**
 * Crea una red perc con <numCap> capas
 * y con las catidades de neuronas por capa dadas en <neurPorCapa>
 * <tipos> indica de que tipo es cada capa. Sem es un puntero a un long
 * primo y negativo, para inicializar la semilla del random.
 * muRetenContex indica la retencion de cada capa de contexto, si
 * muRetenContex[i] esta fuera de [0.0;1.0]; luego la capa[i] no es
 * retroalimentada; muRetenContex[i] cercano a 0.0, olvida rapido, cercano a
 * 1.0, memoria perdura.
 * Inicializa las neuronas aleatoriamente con pesos en [.5;-.5]
 * indice 0 primera capa ultimo, ultima capa
 * Si no se desea guardar la red se pasa strFile==NULL
 * strFile indica donde guardar la red.
 * si numCap<1 o neurPorCapa==NULL, se cargara la red desde strFile
 * si (numCap<1 o neurPorCapa==NULL) Y strFile==NULL, se devolvera NULL
 * y se imprimira por pantalla el error.
 */
rperc_t rperc_create(int numCap, int* neurPorCapa, int* tipos,
        double* muRetenContex, char* strFile, long* sem) {
    rperc_t res = NULL;
    if (0 < numCap && neurPorCapa != NULL && tipos != NULL) {
        res = (rperc_t) calloc(1, sizeof (struct sRedPerc));
        if (res) {
            int i = 0, neurInMasContext = neurPorCapa[0], neurInBackup = neurPorCapa[0];
            res->numRealIn = neurInMasContext;
            if (muRetenContex) {
                double maxRetCont = .0;
                res->muRetenContex = (double *) calloc(numCap + 1, sizeof (double));
                for (i = 0; i < numCap + 1; i++) {
                    res->muRetenContex[i] = muRetenContex[i];
                    if (0.0 <= muRetenContex[i] && muRetenContex[i] <= 1.0) {
                        if (maxRetCont < muRetenContex[i])maxRetCont = muRetenContex[i];
                        neurInMasContext += neurPorCapa[i];
                    }
                }
                res->calentContext = 1 + log(.05) / log(maxRetCont);
            } else {
                res->muRetenContex = NULL;
                res->calentContext = 0;
            }
            neurPorCapa[0] = neurInMasContext;
            res->strFile = strFile;
            res->numCap = numCap;
            res->nu = .0476;
            res->alfaMomento = .5;
            res->maxNu = .048;
            res->minNu = .025;
            res->upNu = .0001;
            res->downNu = .91;
            res->oldError = .0;
            res->tipoErr = 0;
            res->totIntentos = 300;
            res->intRestantes = res->totIntentos;
            res->descending = 1;
            res->maxErr = 1.0;
            res->minErr = .1;
            res->sem = sem;
            res->first = cperc_create(0, neurPorCapa[0], 0, NULL, NULL, sem);
            res->last = res->first;
            res->lastErr = (double *) calloc(neurPorCapa[numCap - 1], sizeof (double));
            if (res->lastErr)
                if (res->first)
                    for (i = 0; res && i < numCap; i++) {
                        res->last->nextCap = cperc_create(neurPorCapa[i],
                                neurPorCapa[i + 1], tipos[i + 1], NULL, res->last, sem);
                        if (res->last->nextCap)
                            res->last = res->last->nextCap;
                        else
                            res = rperc_destroy(res);
                    } else
                    res = rperc_destroy(res);
            else
                res = rperc_destroy(res);
            neurPorCapa[0] = neurInBackup;
        }
    } else if (strFile != NULL) {
        char *fromFile = NULL;
        FILE *file = NULL;
        /**
         * TODO: Aca leer del archivo y guardarlo en el str
         */
        file = fopen(strFile, "r");
        if (file) {
            char tmp[515];
            int i = 0, lacum = 0, l = 0;
            while (0 < l || i == 0) {
                int ltemp = lacum;
                l = fread(tmp, sizeof (char), 512, file);
                lacum += l;
                fromFile = (char *) realloc(fromFile, (lacum + 1) * sizeof (char));
                for (i = 0; i < l; i++)
                    fromFile[i + ltemp] = tmp[i];
                i = 1;
            }
            if (lacum && fromFile) {
                fromFile[lacum] = '\0';
                res = rperc_from_str(fromFile);
                free(fromFile);
                if (res)
                    res->strFile = strFile;
                else
                    printf("ERROR al armar la red desde el str\n");
            } else
                printf("ERROR cargando la red... Archivo vacio!\n");
            fclose(file);
        }
    } else
        printf("ERROOR!!! Mal inicializada la red.\n");
    return res;
}

rperc_t rperc_destroy(rperc_t red) {
    if (red) {
        cperc_destroy(red->first);
        free(red->strFile);
        free(red->lastErr);
        free(red);
    }
    return NULL;
}

/**
 * Dado un strRed crea una red perc exactamente como la describe strRed
 */
rperc_t rperc_from_str(char *strRed) {
    return NULL;
}

/**
 * Pasa la red perc a str, guardando todos los detalles de la red
 * Separa el str en lineas, cada linea es una capa.
 */
char *rperc_to_str(rperc_t red) {
    char *res = NULL;
    int l = 75;
    if (red) {
        if (red->strFile)
            l += strlen(red->strFile);
        res = (char *) calloc(l, sizeof (char));
        if (res) {
            cperc_t cap = red->first;
            int i = 0;
            l = sprintf(res, "File: %s\nNumCapas: %i\tnu: %g\tmaxNu: %g\tminNu: %g\tupNu: %g\tdownNu: %g\talfaMomento: %g\nRed:\n",
                    red->strFile, red->numCap, red->nu, red->maxNu, red->minNu,
                    red->upNu, red->downNu, red->alfaMomento);
            while (cap && res) {
                char *tmp = NULL;
                tmp = cperc_to_str(cap);
                res = myVarStrCat(res, "Cap Num: %i\n", i);
                res = myVarStrCat(res, tmp);
                cap = cap->nextCap;
                free(tmp);
                i++;
            }
        }
    } else
        printf("ups\n");
    return res;
}

int rperc_get_num_Context(rperc_t red) {
    if (red && red->first)
        return red->first->neuralOut - red->numRealIn;
    return 0;
}

int rperc_get_num_In(rperc_t red) {
    if (red)
        return red->numRealIn;
    return 0;
}

int rperc_get_num_Out(rperc_t red) {
    if (red && red->last)
        return red->last->neuralOut;
    return 0;
}

int rperc_get_numCap(rperc_t red) {
    if (red)
        return red->numCap;
    return 0;
}

double rperc_get_nu(rperc_t red) {
    if (red)
        return red->nu;
    return .0476;
}

double rperc_get_maxNu(rperc_t red) {
    if (red)
        return red->maxNu;
    return .048;
}

double rperc_get_minNu(rperc_t red) {
    if (red)
        return red->minNu;
    return .025;
}

double rperc_get_upNu(rperc_t red) {
    if (red)
        return red->upNu;
    return .0001;
}

double rperc_get_downNu(rperc_t red) {
    if (red)
        return red->downNu;
    return .91;
}

double rperc_get_alfaMomento(rperc_t red) {
    if (red)
        return red->alfaMomento;
    return .5;
}

int rperc_set_alfaMomento(rperc_t red, double newAlfaMomento) {
    if (red && .0 < newAlfaMomento && newAlfaMomento < 1.0) {
        red->alfaMomento = newAlfaMomento;
        return 1;
    }
    return 0;
}

int rperc_set_allNu(rperc_t red, double newNu, double newMaxNu, double newMinNu,
        double newUpNu, double newDownNu) {
    if (red && .0 < newNu && newNu < 1.0 && newMinNu < newMaxNu && .0 < newMinNu
            && newMaxNu < 1.0 && .0 < newUpNu && newUpNu < 1.0 && .0 < newDownNu
            && newDownNu < 1.0) {
        red->nu = newNu;
        red->maxNu = newMaxNu;
        red->minNu = newMinNu;
        red->upNu = newUpNu;
        red->downNu = newDownNu;
        return 1;
    }
    return 0;
}

int rperc_set_porcErrContext(rperc_t red, double porc) {
    if (red && .0 < porc && porc < 1.0) {
        double maxRetCont = .0;
        int i = 0;
        for (i = 0; i < red->numCap + 1; i++)
            if (0.0 <= red->muRetenContex[i] && red->muRetenContex[i] <= 1.0
                    && maxRetCont < red->muRetenContex[i])
                maxRetCont = red->muRetenContex[i];
        red->calentContext = 1 + log(porc) / log(maxRetCont);
    }
    return 0;
}

int rperc_set_rectaError(rperc_t red, int totIntentos, double minErr,
        double maxErr, int tipoErr) {
    if (red) {
        minErr = dabs(minErr);
        maxErr = dabs(maxErr);
        if (maxErr < minErr) {
            double tmp = minErr;
            minErr = maxErr;
            maxErr = tmp;
        }
        if (minErr != .0) red->minErr = minErr;
        if (maxErr != .0) red->maxErr = maxErr;
        if (0 < totIntentos)red->totIntentos = totIntentos;
        else red->totIntentos = -totIntentos + 1;
        if (0 < tipoErr)red->tipoErr = tipoErr;
        else red->tipoErr = -tipoErr;
    }
    return 0;
}

/**
 * El resultado es un array de double,
 * len(res)==rperc_get_num_Out(red)
 * (Si la ultima capa es de neuronas binarias el resultado es {1,-1})
 * in es la entrada a evaluar por la red,
 * len(in)==rperc_get_num_In(red).
 */
double *rperc_eval(rperc_t red, double *in) {
    return rperc_evalX(red, in, 0);
}

double *rperc_evalParaAprender(rperc_t red, double **in, int *selec, int point) {
    int i = 0;
    if (red && rperc_get_num_Context(red) != 0) {
        if (selec[point] == 0) {
            cperc_t tempC = red->first;
            while (tempC) {
                for (i = 0; i < tempC->neuralOut; i++)
                    tempC->out[i] = .0;
                tempC = tempC->nextCap;
            }
        } else if (point == 0 || red->calentContext < selec[point] - selec[point - 1])
            for (i = red->calentContext; 0 < i; i--) {
                if (i <= selec[point])
                    free(rperc_eval(red, in[selec[point] - i]));
            } else if (selec[point - 1] < selec[point])
            for (i = selec[point - 1] + 1; i < selec[point]; i++)
                free(rperc_eval(red, in[i]));
    }
    return rperc_evalX(red, in[selec[point]], 1);
}

double *rperc_evalX(rperc_t red, double *in, int aprenderONo) {
    double *res = NULL;
    if (red && red->first && in) {
        int i = 0;
        cperc_t capaActual = red->first;
        for (i = 0; i < rperc_get_num_In(red); i++) {
            red->first->out[i] = in[i];
        }
        if (red->muRetenContex && rperc_get_num_Context(red))
            for (int j = 0; j < red->numCap + 1; j++) {
                if (0.0 <= red->muRetenContex[j] && red->muRetenContex[j] <= 1.0)
                    for (int k = 0; capaActual && i < red->first->neuralOut
                            && ((capaActual == red->first && k < rperc_get_num_In(red))
                            || (capaActual != red->first && k < capaActual->neuralOut)); k++) {
                        red->first->out[i] = red->first->out[i] * red->muRetenContex[j]
                                + capaActual->out[k];
                        i++;
                    }
                capaActual = capaActual->nextCap;
            }
        res = cperc_eval(red->first, aprenderONo);
    } else
        printf("No se puede evaluaaar!!!\nPor:\nred: %i\tfirstCap: %i\tin: %i\n",
            red == NULL, red->first == NULL, in == NULL);
    return res;
}

void rperc_adaptarNu(rperc_t red, double newError) {
    red->descending = 1;
    if (newError < red->oldError) {
        red->descending = 1;
        if (red->nu < red->maxNu) {
            red->nu = red->nu + red->upNu;
        }
    } else if (red->oldError < newError) {
        red->descending = 0;
        if (red->minNu < red->nu) {
            red->nu = red->nu * red->downNu;
        }
    }
    red->oldError = newError;
}

int rperc_rectaError(rperc_t red, int len, double* res, double* deseado,
        double *retErr) {
    int fail = 0, j;
    *retErr = .0;
    switch (red->tipoErr) {
        case 1:
            for (j = 0; j < len; j++)
                *retErr = *retErr + dabs(res[j] - deseado[j]);
            *retErr = *retErr / (double) j;
            fail = (red->maxErr - ((red->maxErr - red->minErr) * ((double) red->intRestantes)
                    / ((double) red->totIntentos)) < *retErr);
        default:
            for (j = 0; j < len; j++) {
                if (*retErr < dabs(res[j] - deseado[j])) *retErr = dabs(res[j] - deseado[j]);
                fail |= (red->maxErr - ((red->maxErr - red->minErr) * ((double) red->intRestantes)
                        / ((double) red->totIntentos)) < *retErr);
            }
    }
    return fail;
}

void rperc_aprender_online(rperc_t red, int cantDatos, double **in,
        double **deseado, int *selec) {
    if (red && deseado && in && selec && deseado[cantDatos - 1] && in[cantDatos - 1]) {
        double newError = .0, *res = NULL;
        for (int i = 0; i < cantDatos; i++) {
            int fail = 1, localIntRestantes = red->intRestantes;
            free(rperc_evalParaAprender(red, in, selec, i));
            while (fail && 0 < localIntRestantes) {
                localIntRestantes--;
                cperc_aprender(red->last, deseado[selec[i]],
                        ((double) red->descending) * red->alfaMomento, red->nu);
                res = rperc_evalParaAprender(red, in, selec, i);
                fail = rperc_rectaError(rperc_get_num_Out(red), res,
                        deseado[selec[i]], &newError);
                free(res);
                rperc_adaptarNu(red, newError);
            }
        }
    }
}

void rperc_aprender_batch(rperc_t red, int cantDatos, double **in,
        double **deseado, int *selec) {
    if (red && deseado && in && selec && deseado[cantDatos - 1] && in[cantDatos - 1]) {
        int i = 0;
        double conMomento_d;
        if (red->descending) conMomento_d = 1.0;
        else conMomento_d = .0;
        for (i = 0; i < cantDatos; i++) {
            free(rperc_evalParaAprender(red, in, selec, i));
            cperc_aprender(red->last, deseado[selec[i]], conMomento_d * red->alfaMomento, red->nu);
        }
    }
}

/**
 * La red aprende el ultimo valor evaluado
 * deseado es un array de doubles que refleja el resultado que deberia
 * haber teniedo neurona a neurona de sallida para el ultimo in evaluado
 * len(deseado)==rperc_get_num_Out(red)
 * maxErr es el maximo error permtido... (ya vamos a ver en que y donde)
 */
int rperc_aprender(rperc_t red, int cantDatos, double** in, double** deseado,
        metodo aprenderPor) {
    int conceptosFaltantes = cantDatos, i = 0;
    red->intRestantes = red->totIntentos;
    if (red && deseado && in && deseado[cantDatos - 1] && in[cantDatos - 1]) {
        double *res = NULL;
        int *selec = NULL;
        selec = (int *) calloc(cantDatos, sizeof (int));
        if (selec) {
            red->oldError = (double) cantDatos;
            for (i = 0; i < cantDatos; i++) {
                selec[i] = i;
            }
            while (0 < conceptosFaltantes && 0 < red->intRestantes) {
                red->intRestantes--;
                switch (aprenderPor) {
                    case online:
                        rperc_aprender_online(red, conceptosFaltantes, in,
                                deseado, selec);
                    default:
                        rperc_aprender_batch(red, conceptosFaltantes, in,
                                deseado, selec);
                }
                conceptosFaltantes = 0;
                for (i = 0; i < cantDatos; i++) {
                    int fail = 0;
                    double none;
                    res = rperc_eval(red, in[i]);
                    fail = rperc_rectaError(rperc_get_num_Out(red), res, deseado[i],
                            &none);
                    free(res);
                    if (fail) {
                        selec[conceptosFaltantes] = i;
                        conceptosFaltantes++;
                    }
                }
                rperc_adaptarNu(red, (double) conceptosFaltantes);
                for (i = conceptosFaltantes; 0 < i; i--) {
                    int r = i * ran2(red->sem), tmp = 0;
                    tmp = selec[i - 1];
                    selec[i - 1] = selec[r];
                    selec[r] = tmp;
                }
            }
            free(selec);
        }
    }
    return red->intRestantes;
}
