/*
 * File:   perceptronNeuralNet.h
 *
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#ifndef _PERCEPTRONNEURALNET_H
#define _PERCEPTRONNEURALNET_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "perceptronNeuralLayer.h"

typedef struct sPercNeuralNet *percnn_t;

typedef enum {
	batch, online
}method;

typedef enum {
	sig, delta
}tMode;

/**
 * Crea una net perc con <numLayers> capas
 * y con las catidades de neuronas por capa dadas en <neuronsByLayer>
 * <types> indica de que type es cada capa. Sem es un puntero a un long
 * primo y negativo, para inicializar la semilla del random.
 * muRetenContex indica la retencion de cada capa de contexto, si
 * muRetenContex[i] esta fuera de [0.0;1.0]; luego la capa[i] no es
 * retroalimentada; muRetenContex[i] cercano a 0.0, olvida rapido, cercano a
 * 1.0, memoria perdura.
 * Inicializa las neuronas aleatoriamente con pesos en [.5;-.5]
 * indice 0 primera capa ultimo, ultima capa
 * Si no se desea guardar la net se pasa strFile==NULL
 * strFile indica donde guardar la net.
 * si numLayers<1 o neuronsByLayer==NULL, se cargara la net desde strFile
 * si (numLayers<1 o neuronsByLayer==NULL) Y strFile==NULL, se devolvera NULL
 * y se imprimira por pantalla el error.
 */
percnn_t percnn_create(int numLayers, int* neuronsByLayer, neuralType* types,
		double* muRetenContex, char* strFile, long* sem);
percnn_t percnn_create_with_neuronLayerMultiTypes(int numLayers,
		int* neuronsByLayer, int* neuralTypesByLayer, int** numNeuronOfType,
		neuralType** types, double* muRetenContex, char* strFile, long* sem);
percnn_t percnn_create_with_memory(int numLayers, int* neuronsByLayer,
		neuralType* types, double* muRetenContex, char* strFile, long* sem,
		double*** W);
percnn_t percnn_destroy(percnn_t net);
int percnn_save_plaintext(percnn_t net, char* strFile);
/**
 * Dado un strNeuralNet crea una net perc exactamente como la describe strNeuralNet
 */
percnn_t percnn_from_str(char* strNeuralNet);
/**
 * Pasa la net perc a str, guerdando todos los detalles de la net
 * Separa el str en lineas, cada linea es una capa.
 */
char* percnn_to_str(percnn_t net);

int percnn_get_num_Context(percnn_t net);
int percnn_get_num_In(percnn_t net);
int percnn_get_num_Out(percnn_t net);
int percnn_get_numLayers(percnn_t net);

double percnn_get_nu(percnn_t net);
double percnn_get_maxNu(percnn_t net);
double percnn_get_minNu(percnn_t net);
double percnn_get_upNu(percnn_t net);
double percnn_get_downNu(percnn_t net);

double percnn_get_alphaMoment(percnn_t net);
int percnn_get_numVIContext(percnn_t net);

int percnn_set_alphaMoment(percnn_t net, double newAlphaMoment);
int percnn_set_allNu(percnn_t net, double newNu, double newMaxNu, double newMinNu,
		double newUpNu, double newDownNu);
int percnn_set_percErrContext(percnn_t net, double porc);
int percnn_set_lineError(percnn_t net, long totalAttemps, double minErr,
		double maxErr, int typeErr);
/**
 * El resultado es un array de double,
 * len(res)==percnn_get_num_Out(net)
 * (Si la ultima capa es de neuronas binarias el resultado es {1,-1})
 * in es la entrada a evaluar por la net,
 * len(in)==percnn_get_num_In(net).
 */
double* percnn_eval(percnn_t net, double* in);
/**
 * La net aprende el ultimo valor evaluado
 * desired es un array de doubles que refleja el resultado que deberia
 * haber tenido, neurona a neurona de sallida, para el ultimo in evaluado
 * len(desired)==percnn_get_num_Out(net)
 * maxErr es el maximo error permtido... (ya vamos a ver en que y donde)
 */
int percnn_learn(percnn_t net, int numData, double** in, double** desired,
		method learnBy);
int percnn_test(percnn_t net, int numData, double** in, double** desired,
		tMode testBy);

/**
 * False by default
 * If there not a hidden layer, then create one.
 * Add neurons only on last hidden layer.
 * New neurons are the same type of the output neurons or tanh if grow.
 */
int percnn_setAdaptativelyGrowMode(percnn_t net, int boolConfirm);
int percnn_getAdaptativelyGrowMode(percnn_t net);

int percnn_setAdaptativelyGrowParameters(percnn_t net, double fvuTol,
		double stopRatioNeurons, double stopRatioLayer, double copyRatio);

/**
 * False by default
 * PruningMode is not implemented yet.
 */
int percnn_setPruningMode(percnn_t net, int boolConfirm);

#ifdef  __cplusplus
}
#endif

#endif  /* _PERCEPTRONNEURALNET_H */
