/* 
 * File:   capaPerceptron.h
 *
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#ifndef _REDPERCEPTRON_H
#define	_REDPERCEPTRON_H

#ifdef	__cplusplus
extern "C" {
#endif
    typedef struct sRedPerc *rperc_t;

    typedef enum {
        batch, online
    } metodo;

    /**
     * Crea una red perc con <numCap> capas
     * y con las catidades de neuronas por capa dadas en <neurPorCapa>
     * <tipos> indica de que tipo es cada capa
     * Inicializa las neuronas aleatoriamente con pesos en [.5;-.5]
     * indice 0 primera capa ultimo, ultima capa
     * Si no se desea guardar la red se pasa strFile==NULL
     * strFile indica donde guardar la red.
     * si numCap<1 o neurPorCapa==NULL, se cargara la red desde strFile
     * si (numCap<1 o neurPorCapa==NULL) Y strFile==NULL, se devoñvera NULL
     * y se imprimira por pantalla el error.
     */
    rperc_t rperc_create(int numCap, int* neurPorCapa, int* tipos, char* strFile, long* sem);
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

    int rperc_get_num_In(rperc_t red);
    int rperc_get_num_Out(rperc_t red);
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
     * haber teniedo neurona a neurona de sallida para el ultimo in evaluado
     * len(deseado)==rperc_get_num_Out(red)
     * maxErr es el maximo error permtido... (ya vamos a ver en que y donde)
     */
    int rperc_aprender(rperc_t red, int numDeIntentos, int cantDatos, double** in, double** deseado, int conMomento, metodo aprenderPor);
    void rperc_aprender_online(rperc_t red, int intRestantes, int numDeIntentos, int cantDatos, double** in, double** deseado, int conMomento);
    void rperc_aprender_batch(rperc_t red, int cantDatos, double** in, double** deseado, int conMomento);

#ifdef	__cplusplus
}
#endif

#endif	/* _REDPERCEPTRON_H */

