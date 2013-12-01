/*
 * File:   pca.c
 *
 * Principal Component Analysis
 *
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/6/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#include "pca.h"
#include "../lib/generadores.h"
#include "../lib/otraLibMas.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

struct sPCA {
    int numIn, numOut;
    double *in, *out, **W, nu;
    long *sem;
    char *strFile;
};

pca_t pca_create(int numIn, int numOut, double nu, char* strFile, long* sem) {
    pca_t res = NULL;
    if (sem == NULL) {
        printf("ERROOR!!! Mal inicializada la red. Semilla Nula.\n");
    } else if (0 < numOut && numOut < numIn) {
        res = (pca_t) calloc(1, sizeof (struct sPCA));
        if (res) {
            int i = 0;
            res->sem = sem;
            if (nu <= .0) res->nu = .1;
            else res->nu = nu;
            res->numIn = numIn;
            res->numOut = numOut;
            i = strlen(strFile);
            res->strFile = (char*) calloc(i + 5, sizeof (char));
            res->strFile = strncpy(res->strFile, strFile, i + 1);
            res->out = (double *) calloc(numOut, sizeof (double));
            res->in = (double *) calloc(numIn, sizeof (double));
            res->W = (double **) calloc(numOut, sizeof (double *));
            if (res->W && res->out && res->in) {
                int ok = 1, j = 0;
                for (j = 0; j < numIn; j++)
                    res->in[j] = .0;
                for (i = 0; ok && i < numOut; i++) {
                    res->out[i] = .0;
                    res->W[i] = NULL;
                    res->W[i] = (double *) calloc(numIn, sizeof (double));
                    if (res->W[i]) {
                        for (j = 0; j < numIn; j++) {
                            res->W[i][j] = (ran2(sem) - .5) / (double) (numIn * 3 + 1);
                        }
                    } else {
                        ok = 0;
                        res = pca_destroy(res);
                    }
                }
            } else {
                res = pca_destroy(res);
            }
        }
    } else if (strFile != NULL) {
        char *fromFile = NULL;
        FILE *file = NULL;
        /**
         * Aca leer del archivo y guardarlo en el str
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
                res = pca_from_str(fromFile);
                free(fromFile);
                if (res) {
                    i = strlen(strFile);
                    res->strFile = (char*) calloc(i + 5, sizeof (char));
                    res->strFile = strncpy(res->strFile, strFile, i + 1);
                } else
                    printf("ERROR al armar la red desde el str\n");
            } else
                printf("ERROR cargando la red... Archivo vacio!\n");
            fclose(file);
        } else printf("FAIL!!! No puedo abrir el archivo o el archivo no existe!!\n");
    } else
        printf("ERROOR!!! Mal inicializada la red. Parametros incorrectos\n");
    return res;
}

pca_t pca_destroy(pca_t pca) {
    if (pca) {
        int i = 0;
        free(pca->out);
        free(pca->in);
        if (pca->W) {
            for (i = 0; i < pca->numOut; i++)
                if (pca->W[i])
                    free(pca->W[i]);
            free(pca->W);
        }
        free(pca);
    }
    return NULL;
}

int pca_save(pca_t pca, char* strFile) {
    int res = 2, i = 0;
    if (pca) {
        FILE *file = NULL;
        while (file == NULL && 0 < res) {
            if (strFile != NULL && (file = fopen(strFile, "w")) != NULL) {
                i = strlen(strFile);
                free(pca->strFile);
                pca->strFile = (char*) calloc(i + 5, sizeof (char));
                pca->strFile = strncpy(pca->strFile, strFile, i + 1);
            } else {
                if (pca->strFile) {
                    i = strlen(pca->strFile);
                    strFile = (char*) calloc(i + 5, sizeof (char));
                    strFile = strncpy(strFile, pca->strFile, i + 1);
                }
                res--;
            }
        }
        if (strFile != NULL && 0 < res) {
            char *toFile = NULL;
            int lacum = 0, l = 0;
            toFile = pca_to_str(pca);
            lacum = strlen(toFile) + 1;
            l = fwrite(toFile, sizeof (char), lacum, file);
            free(toFile);
            if (lacum != l) {
                printf("ERROR guardando la red... Archivo incompleto!\n");
                res = 0;
            }
            fclose(file);
        } else
            printf("FAIL!!! No puedo abrir el archivo o el archivo no existe!!\n");
    } else {
        res = 0;
        printf("La red es Nula!\n");
    }
    return res;
}

/**
 * Dado un strRed crea una red perc exactamente como la describe strRed
 */
pca_t pca_from_str(char *strPCA) {
    pca_t res = NULL;
    if (strPCA) {
        int offset = 0, maxLen = 0, numOut = 0, numIn = 0, i = 0, j = 0;
        long sem = -13;
        double nu = .0;
        char *strFile = NULL;
        maxLen = strlen(strPCA);
        strFile = (char*) calloc(500, sizeof (char));
        sscanf(strPCA, " File: %s\nNumOut: %i\tNumIn: %i\tnu: %lg",
                strFile, &numOut, &numIn, &nu);
        res = pca_create(numIn, numOut, nu, strFile, &sem);
        free(strFile);
        for (i = 0; i < numOut && offset < maxLen; i++) {
            offset = strFindEndIn(strPCA, "\t[", maxLen, offset);
            for (j = 0; j < numIn && offset < maxLen; j++) {
                sscanf(&(strPCA[offset]), " %lg", &(res->W[i][j]));
                offset = strFindEndIn(strPCA, ",", maxLen, offset);
            }
        }
    }
    return res;
}

char *pca_to_str(pca_t pca) {
    char *res = NULL;
    int l = 0;
    if (pca) {
        l = 575;
        res = (char *) calloc(l, sizeof (char));
        if (res) {
            int i = 0, j = 0;
            l = sprintf(res, "File: %s\nNumOut: %i\tNumIn: %i\tnu: %g\n",
                    pca->strFile, pca->numOut, pca->numIn, pca->nu);
            for (i = 0; i < pca->numOut && res; i++) {
                res = myVarStrCat(res, "%i\t[ ", i);
                for (j = 0; j < pca->numIn && res; j++)
                    if (j < pca->numIn - 1)
                        res = myVarStrCat(res, "%g,\t", pca->W[i][j]);
                    else
                        res = myVarStrCat(res, "%g ]\n", pca->W[i][j]);
            }
        }
    }
    return res;
}

int pca_get_num_In(pca_t pca) {
    if (pca)
        return pca->numIn;
    else
        return 0;
}

int pca_get_num_Out(pca_t pca) {
    if (pca)
        return pca->numOut;
    else
        return 0;
}

double pca_get_nu(pca_t pca) {
    if (pca)
        return pca->nu;
    else
        return .1;
}

double *pca_eval(pca_t pca, double* in) {
    double *res = NULL;
    if (pca) {
        int i = 0, j = 0;
        if (in)
            for (j = 0; j < pca->numIn; j++)
                pca->in[j] = in[j];
        for (i = 0; i < pca->numOut; i++) {
            pca->out[i] = .0;
            for (j = 0; j < pca->numIn; j++)
                pca->out[i] = pca->out[i] + (pca->W[i][j] * pca->in[j]);
        }
        res = (double *) calloc(pca->numOut, sizeof (double));
        if (res)
            for (i = 0; i < pca->numOut; i++)
                res[i] = pca->out[i];
    }
    return res;
}

double* pca_aprender(pca_t pca, double nu) {
    double* res = NULL;
    if (pca) {
        int i = 0, j = 0;
        double *d = (double*) calloc(pca->numIn, sizeof (double));
        if (.0 < nu) pca->nu = nu;
        for (i = 0; i < pca->numOut; i++)
            for (j = 0; j < pca->numIn; j++) {
                if (i) d[j] = d[j] + pca->out[i] * pca->W[i][j];
                else d[j] = pca->out[i] * pca->W[i][j];
                pca->W[i][j] = pca->W[i][j]+(pca->nu * pca->out[i]*
                        (pca->in[j] - d[j]));
            }
        res = pca_eval(pca, NULL);
        free(d);
    }
    return res;
}
