/*
 * File:   pca.h
 *
 * Principal Component Analysis
 *
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/6/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#ifndef _PCA_H
#define _PCA_H

#ifdef  __cplusplus
extern "C" {
#endif
    typedef struct sPCA *pca_t;

    /**
     * Crea una red de una capa simple de neuronas para Analisis de Componenetes
     * Principales, con las catidades de neuronas de entrada y salida dadas en
     * <numIn>, <numOut> y todas las neuronas son lineales. numIn >> numOut.
     * Inicializa las neuronas aleatoriamente con pesos W en [.1;-.1]
     * nu sera la tasa de cambio en el aprendizaje, si nu<=0.0 entonces nu=.1
     * Si no se desea guardar la red se pasa strFile==NULL
     * strFile indica donde guardar la red.
     * si numIn<0 o numOut<0, se cargara la red desde strFile
     * si (numIn<0 o numOut<0) Y strFile==NULL, se devoñvera NULL
     * y se imprimira por pantalla el error.
     */
    pca_t pca_create(int numIn, int numOut, double nu, char* strFile, long* sem);
    pca_t pca_destroy(pca_t pca);
    int pca_save(pca_t pca, char* strFile);
    /**
     * Dado un strPCA crea una red PCA exactamente como la describe strPCA
     */
    pca_t pca_from_str(char* strPCA);
    /**
     * Pasa la red PCA a str, guerdando todos los detalles de la red.
     */
    char* pca_to_str(pca_t pca);

    int pca_get_num_In(pca_t pca);
    int pca_get_num_Out(pca_t pca);
    double pca_get_nu(pca_t pca);
    /**
     * El resultado es un array de double,
     * len(res)==pca_get_num_Out(red)
     * (El resultado es [1,-1])
     * in es la entrada a evaluar por la red,
     * len(in)==pca_get_num_In(red).
     */
    double* pca_eval(pca_t pca, double* in);
    /**
     * La red aprende el ultimo valor in evaluado con tasa de cambio nu.
     * Si nu<=0.0 se usa el ultimo 0.0<nu presentado como tasa de aprendizaje.
     * La red devuelve el resultado de pca_eval(pca,in) luego del aprendizaje.
     *
     * Usamos la regla de aprendizage de Sanger, por lo tanto los pesos obtenidos
     * daran las direcciones de las componentes principales en cada salida de
     * forma ordenada, out[0] == 1°CompPrinc,...., out[N-1] == N°CompPrinc.
     */
    double* pca_aprender(pca_t pca, double nu);

#ifdef  __cplusplus
}
#endif

#endif  /* _PCA_H */

