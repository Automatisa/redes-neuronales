/* 
 * File:   capaPerseptron.h
 * Author: eric
 *
 * Created on 22 de enero de 2011, 19:22
 */

#ifndef _REDPERSEPTRON_H
#define	_REDPERSEPTRON_H

#ifdef	__cplusplus
extern "C" {
#endif
    typedef struct sRedPers *rpers_t;

    typedef enum {
        batch, online
    } metodo;

    /**
     * Crea una red pers con <numCap> capas
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
    rpers_t rpers_create(int numCap, int* neurPorCapa, int* tipos, char* strFile, long* sem);
    rpers_t rpers_destroy(rpers_t red);
    /**
     * Dado un strRed crea una red pers exactamente como la describe strRed
     */
    rpers_t rpers_from_str(char* strRed);
    /**
     * Pasa la red pers a str, guerdando todos los detalles de la red
     * Separa el str en lineas, cada linea es una capa.
     */
    char* rpers_to_str(rpers_t red);

    int rpers_get_num_In(rpers_t red);
    int rpers_get_num_Out(rpers_t red);
    /**
     * El resultado es un array de double,
     * len(res)==rpers_get_num_Out(red)
     * (Si la ultima capa es de neuronas binarias el resultado es {1,-1})
     * in es la entrada a evaluar por la red,
     * len(in)==rpers_get_num_In(red).
     */
    double* rpers_eval(rpers_t red, double* in);
    /**
     * La red aprende el ultimo valor evaluado
     * deseado es un array de doubles que refleja el resultado que deberia
     * haber teniedo neurona a neurona de sallida para el ultimo in evaluado
     * len(deseado)==rpers_get_num_Out(red)
     * maxErr es el maximo error permtido... (ya vamos a ver en que y donde)
     */
    int rpers_aprender(rpers_t red, int numDeIntentos, int cantDatos, double** in, double** deseado, int conMomento, metodo aprenderPor);
    void rpers_aprender_online(rpers_t red, int numDeIntentos, int cantDatos, double** in, double** deseado, int conMomento);
    void rpers_aprender_batch(rpers_t red, int cantDatos, double** in, double** deseado, int conMomento);

#ifdef	__cplusplus
}
#endif

#endif	/* _REDPERSEPTRON_H */

