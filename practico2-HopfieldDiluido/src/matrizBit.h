/* 
 * File:   matrizBit.h
 * Author: nahuel
 *
 * Created on 14 de septiembre de 2010, 20:36
 */

#ifndef MATRIZBIT_H
#define	MATRIZBIT_H

#ifdef	__cplusplus
extern "C" {
#endif
    typedef struct smatrizBit *matrizBit_t;

    matrizBit_t mb_create(int nfila, int ncol);
    matrizBit_t mb_destroy(matrizBit_t S);
    int mb_cap(void);
    int mb_ncol(matrizBit_t S);
    int mb_nfila(matrizBit_t S);
    int mb_get_sig(matrizBit_t S, int fila, int col);
    int mb_set(matrizBit_t S, int fila, int col, int b);
    int mb_SetRandFila(matrizBit_t S, int fila);
    int mb_SumProdSignos(matrizBit_t S1, int fila1, matrizBit_t S2, int fila2);
#ifdef	__cplusplus
}
#endif

#endif	/* MATRIZBIT_H */

