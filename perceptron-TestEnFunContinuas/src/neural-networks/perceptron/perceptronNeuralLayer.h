/*
 * perceptronNeuralLayer.h
 *
 *  Created on: 28/12/2012
 *      Author: nahuel
 * $ License: GPL v3 $
 */

#ifndef PERCEPTRONNEURALLAYER_H
#define PERCEPTRONNEURALLAYER_H

#include "neuronFunction.h"

typedef struct sLayerPerc *percl_t;

percl_t percl_create(int neuralIn, int neuralOut, neuralType type,
		percl_t nextL, percl_t prevL, long *sem);
percl_t percl_create_MultiTypes(int neuralIn, int neuralOut,
		int neuralTypesByLayer, int* numNeuronOfType, neuralType* types,
		percl_t nextL, percl_t prevL, long *sem);
percl_t percl_create_with_memory(int neuralIn, int neuralOut, neuralType type,
		percl_t nextL, percl_t prevL, double** W);
percl_t percl_destroy(percl_t layer);
percl_t percl_from_str(char *strLayer, percl_t nextL, percl_t prevL, int maxLen,
		int *offset);
char *percl_to_str(percl_t layer);

int percl_get_num_Out(percl_t layer);
percl_t percl_get_nextLayer(percl_t layer);
percl_t percl_get_prevLayer(percl_t layer);

int percl_set_nextLayer(percl_t layer, percl_t nextL);
int percl_set_In(percl_t layer, int numIn, double* in);

/**
 * False by default
 * PruningMode is not implemented yet.
 */
int percl_setPruningMode(percl_t layer, int boolConfirm);

percl_t percl_add_extension_layer(percl_t layer, double copyPercent);
percl_t percl_add_neuron(percl_t layer);

double *percl_eval(percl_t layer, int derived);
void percl_learn(percl_t layer, double *desired, double alphaMoment, double nu);

#endif /* PERCEPTRONNEURALLAYER_H */
