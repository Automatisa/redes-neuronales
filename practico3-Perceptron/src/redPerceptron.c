/*
 * File:   capaPerceptron.c
 *
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#include "redPerceptron.h"
#include "neuronaPerceptron.h"
#include "generadores.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

typedef struct sCapaPerc *cperc_t;

struct sCapaPerc {
    int neuralIn, neuralOut, tipo;
    cperc_t nextCap, prevCap;
    double *out, *delta, **W, **dWmomento;
};

double dabs(double x);
cperc_t cperc_create(int neuralIn, int neuralOut, int tipo, cperc_t nextC, cperc_t prevC, long* sem);
cperc_t cperc_destroy(cperc_t cap);
cperc_t cperc_from_str(char* strCap);
char* myVarStrCat(char* s, const char* format, ...);
char* cperc_to_str(cperc_t cap);
double* cperc_eval(cperc_t cap);
void cperc_apreder(cperc_t cap, double *deseado, int conMomento, double nu);

double dabs(double x) {
    if (x < 0.0) return -x;
    return x;
}

/**
 * TODO: TEEEENEEEER EEEN CUENTAAAAAA!!!!!!!!!!!
 * La neurona de entrada 0 sienpre es -1.
 * Poner un lugar mas a la entrada.
 */
cperc_t cperc_create(int neuralIn, int neuralOut, int tipo, cperc_t nextC, cperc_t prevC, long *sem) {
    cperc_t res = NULL;
    res = (cperc_t) calloc(1, sizeof (struct sCapaPerc));
    if (res) {
        int i = 0;
        res->neuralIn = neuralIn;
        res->neuralOut = neuralOut;
        res->tipo = tipo;
        res->nextCap = nextC;
        res->prevCap = prevC;
        res->out = (double*) calloc(neuralOut, sizeof (double));
        res->delta = (double*) calloc(neuralOut, sizeof (double));
        res->W = (double**) calloc(neuralOut, sizeof (double*));
        res->dWmomento = (double**) calloc(neuralOut, sizeof (double*));
        if (res->W && res->dWmomento && res->out && res->delta) {
            int ok = 1;
            for (i = 0; ok && i < neuralOut; i++) {
                res->delta[i] = 0.0;
                res->out[i] = 0.0;
                res->W[i] = NULL;
                res->dWmomento[i] = NULL;
                res->W[i] = (double*) calloc(neuralIn + 1, sizeof (double));
                res->dWmomento[i] = (double*) calloc(neuralIn + 1, sizeof (double));
                if (res->W[i] && res->dWmomento[i]) {
                    int j = 0;
                    for (j = 0; j < neuralIn + 1; j++) {
                        res->W[i][j] = (ran2(sem) - .5) / (double) (neuralIn + 1);
                        res->dWmomento[i][j] = 0.0;
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
        /*printf("aca comiensa el bolonqui\n");*/
        free(cap->delta);
        free(cap->out);
        if (cap->W) {
            for (i = 0; i < cap->neuralOut; i++)
                if (cap->W[i]) free(cap->W[i]);
            free(cap->W);
        }
        if (cap->dWmomento) {
            for (i = 0; i < cap->neuralOut; i++)
                if (cap->dWmomento[i]) free(cap->dWmomento[i]);
            free(cap->dWmomento);
        }
        /*printf("aparentemente aca todo bien\n");*/
        if (cap->nextCap)
            cap->nextCap->prevCap = NULL;
        cperc_destroy(cap->nextCap);
        cperc_destroy(cap->prevCap);
        free(cap);
    }
    return NULL;
}

cperc_t cperc_from_str(char* strCap) {
    return NULL;
}

char* myVarStrCat(char* s, const char* format, ...) {
    char *tmp = NULL, *var = NULL;
    int l = -1;
    va_list vl;
    va_start(vl, format);
    var = (char*) calloc(strlen(format) + 500, sizeof (char));
    vsprintf(var, format, vl);

    va_end(vl);
    if (s && var) {
        l = strlen(var);
        tmp = calloc(l + 500, sizeof (char));
        l = 1 + sprintf(tmp, "%s", var);
        s = (char*) realloc(s, (sizeof (char)) * (l + 1000 + strlen(tmp)));
        /**/s = strcat(s, tmp); /**/
        free(tmp);
    }
    free(var);
    return s;
}

char* cperc_to_str(cperc_t cap) {
    char* res = NULL;
    int l = 0;
    if (cap) {
        l = 275;
        res = (char*) calloc(l, sizeof (char));
        if (res) {
            int i = 0, j = 0;
            l = sprintf(res, "Tipo: %i\nNumOut: %i\tNumIn: %i\n"
                    , cap->tipo, cap->neuralOut, cap->neuralIn);
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

double* cperc_eval(cperc_t cap) {
    /* aca calcular la G() -> en V[i] */
    /* y calcular G'() -> en err[i] derivada de G() */
    double *res = NULL;
    if (cap) {
        if (cap->prevCap) {
            int i = 0, j = 0;
            for (i = 0; i < cap->neuralOut; i++) {
                cap->out[i] = -cap->W[i][cap->neuralIn];
                for (j = 0; j < cap->neuralIn; j++)
                    cap->out[i] = cap->out[i] +(cap->W[i][j] * cap->prevCap->out[j]);
                cap->delta[i] = dg(2.0 * cap->out[i], cap->tipo);
                /* TODO: Evaluar posibilidad de si es tipo 0
                 * y es la ultima capa, usar out=sig(g)
                 * de lo contrario arreglar el problema de satisfaccion
                 * en el aprendizage a nivel red y no a nivel capa
                 * TODO: ver que aca el beta es fijo.
                 */
                cap->out[i] = g(2.0 * cap->out[i], cap->tipo);
            }
        }
        if (cap->nextCap)
            res = cperc_eval(cap->nextCap);
        else {
            int i = 0;
            res = (double*) calloc(cap->neuralOut, sizeof (double));
            if (res)
                for (i = 0; i < cap->neuralOut; i++)
                    res[i] = cap->out[i];
        }
    }
    return res;
}

void cperc_apreder(cperc_t cap, double *deseado, int conMomento, double nu) {
    if (cap && cap->prevCap) {
        int fail = 0, i = 0, j = 0;
        if (cap->nextCap) {
            /*aca va lo de la capa no final*/
            cperc_t nc = cap->nextCap;
            for (i = 0; i < cap->neuralOut; i++) {
                double d = 0.0;
                for (j = 0; j < nc->neuralOut; j++)
                    d = d + nc->W[j][i] * nc->delta[j];
                cap->delta[i] = cap->delta[i] * d;
            }
        } else {
            if (deseado) {
                /*aca va lo de la capa final*/
                for (i = 0; i < cap->neuralOut; i++)
                    cap->delta[i] = cap->delta[i] * (deseado[i] - cap->out[i]);
            } else {
                fail = 1;
            }
        }
        if (!fail) {
            double alfa = 10.0;
            if (!conMomento) alfa = 0.0;
            cperc_apreder(cap->prevCap, NULL, conMomento, nu);
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
                cap->dWmomento[i][j] = nu * (-cap->delta[i] + alfa * cap->dWmomento[i][j]);
                cap->W[i][j] = cap->W[i][j] + cap->dWmomento[i][j];
                for (j = 0; j < cap->neuralIn; j++) {
                    cap->dWmomento[i][j] = nu * (cap->prevCap->out[j] * cap->delta[i] + alfa * cap->dWmomento[i][j]);
                    cap->W[i][j] = cap->W[i][j] + cap->dWmomento[i][j];
                }
            }/**/
        }
    }
}

struct sRedPerc {
    int numCap;
    long *sem;
    double nu, *lastErr;
    cperc_t first, last;
    char* strFile;
};

/**
 * Crea una red perc con <numCap> capas
 * y con las catidades de neuronas por capa dadas en <neurPorCapa>
 * Inicializa las neuronas aleatoriamente con pesos en [.5;-.5]
 * indice 0 primera capa ultimo, ultima capa
 * Si no se desea guardar la red se pasa strFile==NULL
 * strFile indica donde guardar la red.
 * si numCap<1 o neurPorCapa==NULL, se cargara la red desde strFile
 * si (numCap<1 o neurPorCapa==NULL) Y strFile==NULL, se devoñvera NULL
 * y se imprimira por pantalla el error.
 */
rperc_t rperc_create(int numCap, int* neurPorCapa, int* tipos, char* strFile, long *sem) {
    rperc_t res = NULL;
    if (0 < numCap && neurPorCapa != NULL && tipos != NULL) {
        res = (rperc_t) calloc(1, sizeof (struct sRedPerc));
        if (res) {
            int i = 0;
            res->strFile = strFile;
            res->numCap = numCap;
            res->nu = .0476;
            res->sem = sem;
            res->first = cperc_create(0, neurPorCapa[0], 0, NULL, NULL, sem);
            res->last = res->first;
            res->lastErr = (double*) calloc(neurPorCapa[numCap - 1], sizeof (double));
            if (res->lastErr)
                if (res->first)
                    for (i = 0; res && i < numCap; i++) {
                        res->last->nextCap = cperc_create(neurPorCapa[i], neurPorCapa[i + 1], tipos[i + 1], NULL, res->last, sem);
                        if (res->last->nextCap)
                            res->last = res->last->nextCap;
                        else res = rperc_destroy(res);
                    } else res = rperc_destroy(res);
            else res = rperc_destroy(res);
        }
    } else if (strFile != NULL) {
        char* fromFile = NULL;
        /**
         * TODO: Aca leer del archivo y guardarlo en el str
         */
        res = rperc_from_str(fromFile);
    } else printf("ERROOR!!! Mal inicializada la red.\n");
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
rperc_t rperc_from_str(char* strRed) {
    return NULL;
}

/**
 * Pasa la red perc a str, guerdando todos los detalles de la red
 * Separa el str en lineas, cada linea es unaa capa.
 */
char* rperc_to_str(rperc_t red) {
    char* res = NULL;
    int l = 75;
    if (red) {
        if (red->strFile)
            l += strlen(red->strFile);
        res = (char*) calloc(l, sizeof (char));
        if (res) {
            cperc_t c = red->first;
            int i = 0;
            l = sprintf(res, "File: %s\nNumCapas: %i\tnu: %g\nRed:\n"
                    , red->strFile, red->numCap, red->nu);
            while (c && res) {
                char* tmp = NULL;
                tmp = cperc_to_str(c);
                res = myVarStrCat(res, "Cap Num: %i\n", i);
                res = myVarStrCat(res, tmp);
                c = c->nextCap;
                free(tmp);
                i++;
            }
        }
    } else printf("ups\n");
    return res;
}

int rperc_get_num_In(rperc_t red) {
    if (red && red->first)
        return red->first->neuralOut;
    else return 0;
}

int rperc_get_num_Out(rperc_t red) {
    if (red && red->last)
        return red->last->neuralOut;
    else return 0;
}

/**
 * El resultado es un array de double,
 * len(res)==rperc_get_num_Out(red)
 * (Si la ultima capa es de neuronas binarias el resultado es {1,-1})
 * in es la entrada a evaluar por la red,
 * len(in)==rperc_get_num_In(red).
 */
double* rperc_eval(rperc_t red, double* in) {
    double* res = NULL;
    if (red && red->first && in) {
        int i = 0;
        for (i = 0; i < rperc_get_num_In(red); i++) {
            red->first->out[i] = in[i];
        }
        res = cperc_eval(red->first);
    } else printf("No se puede evaluaaar!!!\nPor:\nred: %i\tfirstCap: %i\t"
            "in: %i\n", red == NULL, red->first == NULL, in == NULL);
    return res;
}

void rperc_aprender_online(rperc_t red, int intRestantes, int numDeIntentos, int cantDatos, double** in, double** deseado, int conMomento) {
    if (red && deseado && in && deseado[cantDatos - 1] && in[cantDatos - 1]) {
        int myIntRestantes = intRestantes, key = 1, i = 0, j = 0;
        double promErr = 0.0, promErrAnt = 0.0, *res = NULL;
        for (i = 0; i < cantDatos; i++) {
            key = 1;
            myIntRestantes = intRestantes;
            free(rperc_eval(red, in[i]));
            while (key && 0 < myIntRestantes) {
                myIntRestantes--;
                promErr = 0.0;
                cperc_apreder(red->last, deseado[i], conMomento, red->nu);
                res = rperc_eval(red, in[i]);
                for (j = 0; j < rperc_get_num_Out(red); j++)
                    promErr = promErr + dabs(res[j] - deseado[i][j]);
                promErr = promErr / j;
                key = (1.0 - (.9 * ((double) myIntRestantes) / ((double)
                        numDeIntentos)) < promErr);
                free(res);
                if (promErr < promErrAnt && red->nu < .048) {
                    red->nu = red->nu * 1.0003;
                } else if (promErrAnt < promErr && .025 < red->nu) {
                    red->nu = red->nu * .91;
                }
                promErrAnt = promErr;
            }
        }
    }
}

void rperc_aprender_batch(rperc_t red, int cantDatos, double** in, double** deseado, int conMomento) {
    if (red && deseado && in && deseado[cantDatos - 1] && in[cantDatos - 1]) {
        int i = 0;
        /*red->nu = .0476;*/
        for (i = 0; i < cantDatos; i++) {
            free(rperc_eval(red, in[i]));
            /* aca ver que se podria ir variando el nu de manera decreciente
             * de manera que cada ves aprenda mas detalladamente y no olvide
             * otras cosas... a medida que la func rperc_aprender avanza.
             * TODO: hacer depender el nu de la cantidad de conceptos aprendido
             * o del tiempo (cantidad de ciclos de aprendizages), en modo batch.
             */
            cperc_apreder(red->last, deseado[i], conMomento, red->nu);
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
int rperc_aprender(rperc_t red, int numDeIntentos, int cantDatos, double** in, double** deseado, int conMomento, metodo aprenderPor) {
    int conceptosFaltantes = cantDatos, intRestantes = numDeIntentos, i = 0;
    if (red && deseado && in && deseado[cantDatos - 1] && in[cantDatos - 1]) {
        double **entradas = NULL, **salidas = NULL, *res = NULL;
        entradas = (double**) calloc(cantDatos, sizeof (double*));
        salidas = (double**) calloc(cantDatos, sizeof (double*));
        if (entradas && salidas) {
            int concFaltAnt = cantDatos;
            for (i = 0; i < cantDatos; i++) {
                entradas[i] = in[i];
                salidas[i] = deseado[i];
            }
            while (0 < conceptosFaltantes && 0 < intRestantes) {
                /** /if (((faltantes == 1 && (anteriores != faltantes)) || fin == 1) && 4 < cantDatos) {
                    char *str = rperc_to_str(red);
                    printf("faltantes: %i\tfin: %i\n", faltantes,fin); 
                    printf("%s", str);
                    free(str);
                }/ **/
                intRestantes--;
                switch (aprenderPor) {
                    case online:
                        rperc_aprender_online(red, intRestantes, numDeIntentos, conceptosFaltantes, entradas, salidas, conMomento);
                    default:
                        rperc_aprender_batch(red, /** /cantDatos/ **//**/conceptosFaltantes/**/, entradas, salidas, conMomento);
                }
                conceptosFaltantes = 0;
                for (i = 0; i < cantDatos; i++) {
                    int j = 0, fail = 0;
                    res = rperc_eval(red, in[i]);
                    for (j = 0; j < rperc_get_num_Out(red) && !fail; j++)
                        /**
                         * TODO: esto deberia ser aparte, dependiendo de que tipo
                         * son las neuronas de la capa de salida.
                         * Esto deberia ser un llamado a funcion onda
                         * "neuronaperc.h"
                         * double errNeuronaperc(int tipo,int numDeIntentos,int intRestantes,double epsilon);
                         */
                        fail |= (1.0 - (.9 * ((double) intRestantes) / ((double)
                            numDeIntentos)) < dabs(res[j] - deseado[i][j]));
                    free(res);
                    if (fail) {
                        /**/entradas[conceptosFaltantes] = in[i];
                        salidas[conceptosFaltantes] = deseado[i]; /**/
                        conceptosFaltantes++;
                    }
                }/**/
                for (i = /** /cantDatos/ **//**/conceptosFaltantes/**/; 0 < i; i--) {
                    int r = i * ran2(red->sem);
                    double* tmp = NULL;
                    tmp = entradas[i - 1];
                    entradas[i - 1] = entradas[r];
                    entradas[r] = tmp;
                    tmp = salidas[i - 1];
                    salidas[i - 1] = salidas[r];
                    salidas[r] = tmp;
                }/**/
                if (conceptosFaltantes < concFaltAnt && red->nu < .048) {
                    red->nu = red->nu * 1.0003;
                } else if (concFaltAnt < conceptosFaltantes && .025 < red->nu) {
                    red->nu = red->nu * .91;
                }/**/
                concFaltAnt = conceptosFaltantes;
            }
            free(entradas);
            free(salidas);
        }
    }
    return intRestantes;
}
