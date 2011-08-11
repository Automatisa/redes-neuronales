/*
 * File:   redPerceptron.h
 *
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#ifndef _REDPERCEPTRON_H
#define _REDPERCEPTRON_H

#ifdef  __cplusplus
extern "C" {
#endif
    typedef struct sRedPerc *rperc_t;

    typedef enum {
        batch, online
    } metodo;

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
            double* muRetenContex, char* strFile, long* sem);
    rperc_t rperc_destroy(rperc_t red);
    /**
     * Dado un strRed crea una red perc exactamente como la describe strRed
     */
    rperc_t rperc_from_str(char* strRed);
    /**
     * Pasa la red perc a str, guerdando todos los detalles de la red
     * Separa el str en lineas, cada linea es una capa.
     */
    char* rperc_to_str(rperc_t red);

    int rperc_get_num_Context(rperc_t red);
    int rperc_get_num_In(rperc_t red);
    int rperc_get_num_Out(rperc_t red);
    int rperc_get_numCap(rperc_t red);

    double rperc_get_nu(rperc_t red);
    double rperc_get_maxNu(rperc_t red);
    double rperc_get_minNu(rperc_t red);
    double rperc_get_upNu(rperc_t red);
    double rperc_get_downNu(rperc_t red);

    double rperc_get_alfaMomento(rperc_t red);

    int rperc_set_alfaMomento(rperc_t red, double newAlfaMomento);
    int rperc_set_allNu(rperc_t red, double newNu, double newMaxNu,
            double newMinNu, double newUpNu, double newDownNu);
    int rperc_set_porcErrContext(rperc_t red, double porc);
    int rperc_set_rectaError(rperc_t red, int totIntentos, double minErr,
            double maxErr, int tipoErr);
    /**
     * El resultado es un array de double,
     * len(res)==rperc_get_num_Out(red)
     * (Si la ultima capa es de neuronas binarias el resultado es {1,-1})
     * in es la entrada a evaluar por la red,
     * len(in)==rperc_get_num_In(red).
     */
    double* rperc_eval(rperc_t red, double* in);
    /**
     * La red aprende el ultimo valor evaluado
     * deseado es un array de doubles que refleja el resultado que deberia
     * haber tenido, neurona a neurona de sallida, para el ultimo in evaluado
     * len(deseado)==rperc_get_num_Out(red)
     * maxErr es el maximo error permtido... (ya vamos a ver en que y donde)
     */
    int rperc_aprender(rperc_t red, int cantDatos, double** in, double** deseado,
            metodo aprenderPor);

#ifdef  __cplusplus
}
#endif

#endif  /* _REDPERCEPTRON_H */

