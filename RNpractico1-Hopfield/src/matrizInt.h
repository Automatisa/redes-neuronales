/* 
 * File:   matrizInt.h
 * Author: nahuel
 *
 * Created on 14 de septiembre de 2010, 20:38
 */

#ifndef MATRIZINT_H
#define	MATRIZINT_H

#ifdef	__cplusplus
extern "C" {
#endif
    typedef struct smatrizInt *matrizInt_t;

    int mi_ncol(matrizInt_t W);
    int mi_nfila(matrizInt_t W);
    matrizInt_t mi_create(int nfila, int ncol);
    int mi_get(matrizInt_t W, int fila, int col);
    int mi_set(matrizInt_t W, int fila, int col, int val);
#ifdef	__cplusplus
}
#endif

#endif	/* MATRIZINT_H */

