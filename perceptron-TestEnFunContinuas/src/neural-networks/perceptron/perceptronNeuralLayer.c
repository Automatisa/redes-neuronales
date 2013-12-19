/*
 * perceptronNeuralLayer.c
 *
 *  Created on: 28/12/2012
 *      Author: nahuel
 * $ License: GPL v3 $
 */

#include "perceptronNeuralLayer.h"

#include "../lib/generadores.h"
#include "../lib/otraLibMas.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct sLayerPerc {
	int neuralIn, neuralOut, numTypes, *neuronOfTypes;
	int pruningMode;
	neuralType* types;
	percl_t nextLayer, prevLayer;
	double *out, *delta, **W, **dWmoment;
};

neuralType percl_get_typeOfNeuron(percl_t layer, int index);

neuralType percl_get_typeOfNeuron(percl_t layer, int index) {
	neuralType res = tanhyp;
	if (layer && layer->neuronOfTypes && layer->types) {
		int sumNumNeuType = 0, i = 0;
		for (i = 0; i < layer->numTypes && sumNumNeuType <= index; i++) {
			sumNumNeuType += layer->neuronOfTypes[i];
			res = layer->types[i];
		}
	}
	return res;
}

/**
 * TODO: TEEEENEEEER EEEN CUENTAAAAAA!!!!!!!!!!!
 * La neurona de entrada 0 sienpre es -1.
 * Poner un lugar mas a la entrada.
 */
percl_t percl_create(int neuralIn, int neuralOut, neuralType type,
		percl_t nextL, percl_t prevL, long *sem) {
	percl_t res = NULL;
	res = (percl_t) calloc(1, sizeof(struct sLayerPerc));
	if (res) {
		int i = 0;
		res->neuralIn = neuralIn;
		res->neuralOut = neuralOut;
		res->numTypes = 1;
		res->pruningMode = 0;
		res->neuronOfTypes = calloc(res->numTypes, sizeof(int));
		res->neuronOfTypes[0] = neuralOut;
		res->types = calloc(res->numTypes, sizeof(neuralType));
		res->types[0] = type;
		res->nextLayer = nextL;
		res->prevLayer = prevL;
		res->out = (double *) calloc(neuralOut, sizeof(double));
		res->delta = (double *) calloc(neuralOut, sizeof(double));
		res->W = (double **) calloc(neuralOut, sizeof(double *));
		res->dWmoment = (double **) calloc(neuralOut, sizeof(double *));
		if (res->W && res->dWmoment && res->out && res->delta) {
			int ok = 1;
			for (i = 0; ok && i < neuralOut; i++) {
				res->delta[i] = .0;
				res->out[i] = .0;
				res->W[i] = NULL;
				res->dWmoment[i] = NULL;
				res->W[i] = (double *) calloc(neuralIn + 1, sizeof(double));
				res->dWmoment[i] = (double *) calloc(neuralIn + 1,
						sizeof(double));
				if (res->W[i] && res->dWmoment[i]) {
					int j = 0;
					for (j = 0; j < neuralIn + 1; j++) {
						res->W[i][j] = (ran2(sem) - .5)
								/ (double) (neuralIn + 1);
						res->dWmoment[i][j] = .0;
					}
				} else {
					ok = 0;
					res = percl_destroy(res);
				}
			}
		} else {
			res = percl_destroy(res);
		}
	}
	return res;
}

percl_t percl_create_MultiTypes(int neuralIn, int neuralOut,
		int neuralTypesByLayer, int* numNeuronOfType, neuralType* types,
		percl_t nextL, percl_t prevL, long *sem) {
	percl_t res = NULL;
	res = (percl_t) calloc(1, sizeof(struct sLayerPerc));
	if (res) {
		int i = 0;
		res->neuralIn = neuralIn;
		res->neuralOut = neuralOut;
		res->numTypes = neuralTypesByLayer;
		res->neuronOfTypes = calloc(res->numTypes, sizeof(int));
		res->types = calloc(res->numTypes, sizeof(neuralType));
		for (i = 0; i < res->numTypes; i++) {
			res->neuronOfTypes[i] = numNeuronOfType[i];
			res->types[i] = types[i];
		}
		res->nextLayer = nextL;
		res->prevLayer = prevL;
		res->out = (double *) calloc(neuralOut, sizeof(double));
		res->delta = (double *) calloc(neuralOut, sizeof(double));
		res->W = (double **) calloc(neuralOut, sizeof(double *));
		res->dWmoment = (double **) calloc(neuralOut, sizeof(double *));
		if (res->W && res->dWmoment && res->out && res->delta) {
			int ok = 1;
			for (i = 0; ok && i < neuralOut; i++) {
				res->delta[i] = .0;
				res->out[i] = .0;
				res->W[i] = NULL;
				res->dWmoment[i] = NULL;
				res->W[i] = (double *) calloc(neuralIn + 1, sizeof(double));
				res->dWmoment[i] = (double *) calloc(neuralIn + 1,
						sizeof(double));
				if (res->W[i] && res->dWmoment[i]) {
					int j = 0;
					for (j = 0; j < neuralIn + 1; j++) {
						res->W[i][j] = (ran2(sem) - .5)
								/ (double) (neuralIn + 1);
						res->dWmoment[i][j] = .0;
					}
				} else {
					ok = 0;
					res = percl_destroy(res);
				}
			}
		} else {
			res = percl_destroy(res);
		}
	}
	return res;
}

percl_t percl_create_with_memory(int neuralIn, int neuralOut, neuralType type,
		percl_t nextL, percl_t prevL, double** W) {
	percl_t res = NULL;
	res = (percl_t) calloc(1, sizeof(struct sLayerPerc));
	if (res) {
		int i = 0;
		res->neuralIn = neuralIn;
		res->neuralOut = neuralOut;
		res->numTypes = 1;
		res->neuronOfTypes = calloc(res->numTypes, sizeof(int));
		res->neuronOfTypes[0] = neuralOut;
		res->types = calloc(res->numTypes, sizeof(neuralType));
		res->types[0] = type;
		res->nextLayer = nextL;
		res->prevLayer = prevL;
		res->out = (double *) calloc(neuralOut, sizeof(double));
		res->delta = (double *) calloc(neuralOut, sizeof(double));
		res->W = W;
		res->dWmoment = (double **) calloc(neuralOut, sizeof(double *));
		if (res->W && res->dWmoment && res->out && res->delta) {
			int ok = 1;
			for (i = 0; ok && i < neuralOut; i++) {
				res->delta[i] = .0;
				res->out[i] = .0;
				res->dWmoment[i] = NULL;
				res->dWmoment[i] = (double *) calloc(neuralIn + 1,
						sizeof(double));
				if (res->dWmoment[i]) {
					int j = 0;
					for (j = 0; j < neuralIn + 1; j++) {
						res->dWmoment[i][j] = .0;
					}
				} else {
					ok = 0;
					res = percl_destroy(res);
				}
			}
		} else {
			res = percl_destroy(res);
		}
	}
	return res;
}

percl_t percl_destroy(percl_t layer) {
	if (layer) {
		int i = 0;
		free(layer->delta);
		free(layer->out);
		free(layer->neuronOfTypes);
		free(layer->types);
		if (layer->W) {
			for (i = 0; i < layer->neuralOut; i++)
				if (layer->W[i])
					free(layer->W[i]);
			free(layer->W);
		}
		if (layer->dWmoment) {
			for (i = 0; i < layer->neuralOut; i++)
				if (layer->dWmoment[i])
					free(layer->dWmoment[i]);
			free(layer->dWmoment);
		}
		if (layer->nextLayer)
			layer->nextLayer->prevLayer = NULL;
		percl_destroy(layer->nextLayer);
		percl_destroy(layer->prevLayer);
		free(layer);
	}
	return NULL ;
}

percl_t percl_from_str(char *strLayer, percl_t nextL, percl_t prevL, int maxLen,
		int *offset) {
	percl_t res = NULL;
	/* TODO: Fix It (Multi Type)*/
	*offset = strFindEndIn(strLayer, "Type:", maxLen, *offset);
	if (*offset < maxLen) {
		int numOut = 0, numIn = 0, i = 0, j = 0;
		neuralType type = tanhyp;
		long sem = -13;
		sscanf(&(strLayer[*offset]), " %i\nNumOut: %i\tNumIn: %i\n",
				(int*) &type, &numOut, &numIn);
		res = percl_create(numIn, numOut, type, nextL, prevL, &sem);
		for (i = 0; i < numOut && *offset < maxLen; i++) {
			*offset = strFindEndIn(strLayer, "\t[", maxLen, *offset);
			for (j = 0; j <= numIn && *offset < maxLen; j++)
				if (j == numIn) {
					sscanf(&(strLayer[*offset]), " UMBRAL: %lg",
							&(res->W[i][j]));
				} else {
					sscanf(&(strLayer[*offset]), " %lg", &(res->W[i][j]));
					*offset = strFindEndIn(strLayer, ",", maxLen, *offset);
				}
		}
	}
	return res;
}

char *percl_to_str(percl_t layer) {
	char *res = NULL;
	int l = 0;
	if (layer) {
		l = 275;
		res = (char *) calloc(l, sizeof(char));
		if (res) {
			int i = 0, j = 0;
			/* TODO: Fix It (Multi Type)*/
			l = sprintf(res, "Type: %i\nNumOut: %i\tNumIn: %i\n",
					layer->types[0], layer->neuralOut, layer->neuralIn);
			for (i = 0; i < layer->neuralOut && res; i++) {
				res = myVarStrCat(res, "%i\t[ ", i);
				for (j = 0; j <= layer->neuralIn && res; j++)
					if (j == layer->neuralIn) {
						res = myVarStrCat(res, "UMBRAL: %g ]\n",
								layer->W[i][j]);
					} else {
						res = myVarStrCat(res, "%g,\t", layer->W[i][j]);
					}
			}
		}
	}
	return res;
}

int percl_get_num_Out(percl_t layer) {
	int res = 0;
	if (layer)
		res = layer->neuralOut;
	return res;
}

percl_t percl_get_nextLayer(percl_t layer) {
	percl_t res = NULL;
	if (layer)
		res = layer->nextLayer;
	return res;
}

percl_t percl_get_prevLayer(percl_t layer) {
	percl_t res = NULL;
	if (layer)
		res = layer->prevLayer;
	return res;
}

int percl_set_nextLayer(percl_t layer, percl_t nextL) {
	int res = 1;
	if (layer && nextL) {
		layer->nextLayer = nextL;
		res = 0;
	}
	return res;
}

int percl_set_In(percl_t layer, int numIn, double* in) {
	int res = 1;
	if (layer) {
		if (layer->prevLayer)
			res = percl_set_In(layer->prevLayer, numIn, in);
		else if (layer->neuralOut == numIn && in) {
			int i = 0;
			for (i = 0; i < numIn; i++)
				layer->out[i] = in[i];
			res = 0;
		}
	}
	return res;
}

/**
 * False by default
 * PruningMode is not implemented yet.
 */
int percl_setPruningMode(percl_t layer, int boolConfirm) {
	int res = 0;
	if(layer) {
		layer->pruningMode = boolConfirm;
		if(layer->nextLayer) {
			res = percl_setPruningMode(layer->nextLayer, boolConfirm);
		} else {
			res = 1;
		}
	}
	return res;
}

percl_t percl_add_extension_layer(percl_t layer, double copyPercent) {
	percl_t res = NULL;
	res = (percl_t) calloc(1, sizeof(struct sLayerPerc));
	if (res) {
		long sem = -13;
		int i = 0;
		res->neuralIn = layer->neuralOut;
		res->neuralOut = layer->neuralOut;
		res->numTypes = layer->numTypes;
		res->pruningMode = layer->pruningMode;
		res->neuronOfTypes = calloc(res->numTypes, sizeof(int));
		res->types = calloc(res->numTypes, sizeof(neuralType));
		for (i = 0; i < res->numTypes; i++) {
			res->neuronOfTypes[i] = layer->neuronOfTypes[i];
			res->types[i] = layer->types[i];
		}
		res->nextLayer = layer->nextLayer;
		res->prevLayer = layer;
		res->out = (double *) calloc(layer->neuralOut, sizeof(double));
		res->delta = (double *) calloc(layer->neuralOut, sizeof(double));
		res->W = (double **) calloc(layer->neuralOut, sizeof(double *));
		res->dWmoment = (double **) calloc(layer->neuralOut, sizeof(double *));
		if (res->W && res->dWmoment && res->out && res->delta) {
			int ok = 1;
			int neuralIn = layer->neuralOut;
			for (i = 0; ok && i < layer->neuralOut; i++) {
				res->delta[i] = .0;
				res->out[i] = .0;
				res->W[i] = NULL;
				res->dWmoment[i] = NULL;
				res->W[i] = (double *) calloc(neuralIn + 1, sizeof(double));
				res->dWmoment[i] = (double *) calloc(neuralIn + 1,
						sizeof(double));
				if (res->W[i] && res->dWmoment[i]) {
					int j = 0;
					for (j = 0; j < neuralIn + 1; j++) {
						if (j == i) {
							res->W[i][j] = copyPercent/* * layer->W[i][j]*/;
						} else {
							res->W[i][j] = (1.0 - copyPercent)
									* (ran2(&sem) - .5)
									/ (double) (neuralIn + 1);
						}
						res->dWmoment[i][j] = .0;
					}
				} else {
					ok = 0;
					res = percl_destroy(res);
				}
			}
		} else {
			res = percl_destroy(res);
		}
	}
	if (res) {
		if (layer->nextLayer) {
			layer->nextLayer->prevLayer = res;
		}
		layer->nextLayer = res;
	}
	return res;
}

percl_t percl_add_neuron(percl_t layer) {
	if (layer) {
		long sem = -71;
		int i = 0;
		if (layer->types[layer->numTypes - 1] == tanhyp) {
			layer->neuronOfTypes[layer->numTypes - 1]++;
		} else {
			layer->types = (neuralType*) realloc(layer->types,
					sizeof(neuralType) * (layer->numTypes + 1));
			layer->types[layer->numTypes] = tanhyp;
			layer->neuronOfTypes = (int*) realloc(layer->neuronOfTypes,
					sizeof(int) * (layer->numTypes + 1));
			layer->neuronOfTypes[layer->numTypes] = 1;
			layer->numTypes++;
		}
		layer->W = (double**) realloc(layer->W,
				sizeof(double*) * (layer->neuralOut + 1));
		layer->dWmoment = (double**) realloc(layer->dWmoment,
				sizeof(double*) * (layer->neuralOut + 1));
		layer->W[layer->neuralOut] = (double*) calloc(layer->neuralIn + 1,
				sizeof(double));
		layer->dWmoment[layer->neuralOut] = (double*) calloc(
				layer->neuralIn + 1, sizeof(double));
		for (i = 0; i < layer->neuralIn + 1; i++) {
			layer->W[layer->neuralOut][i] = (ran2(&sem) - .5)
					/ (double) (layer->neuralIn + 1);
			layer->dWmoment[layer->neuralOut][i] = .0;
		}
		layer->out = (double *) realloc(layer->out,
				(layer->neuralOut + 1) * sizeof(double));
		layer->delta = (double *) realloc(layer->delta,
				(layer->neuralOut + 1) * sizeof(double));
		layer->delta[layer->neuralOut] = .0;
		layer->out[layer->neuralOut] = .0;

		layer->neuralOut++;
		if (layer->nextLayer) {
			percl_t nextLayer = layer->nextLayer;
			nextLayer->neuralIn++;
			for (i = 0; i < nextLayer->neuralOut; i++) {
				nextLayer->W[i] = (double*) realloc(nextLayer->W[i],
						(nextLayer->neuralIn + 1) * sizeof(double));
				nextLayer->dWmoment[i] = (double*) realloc(
						nextLayer->dWmoment[i],
						(nextLayer->neuralIn + 1) * sizeof(double));
				nextLayer->W[i][nextLayer->neuralIn] =
						nextLayer->W[i][nextLayer->neuralIn - 1];
				nextLayer->dWmoment[i][nextLayer->neuralIn] =
						nextLayer->dWmoment[i][nextLayer->neuralIn - 1];
				nextLayer->W[i][nextLayer->neuralIn - 1] = (ran2(&sem) - .5)
						/ (double) (nextLayer->neuralIn + 1);
				nextLayer->dWmoment[i][nextLayer->neuralIn - 1] = .0;
			}
		}
	}
	return layer;
}

double *percl_eval(percl_t layer, int derived) {
	/* aca calcular la G() -> en V[i] */
	/* y calcular G'() -> en err[i] derivada de G() */
	double *res = NULL;
	if (layer) {
		if (layer->prevLayer) {
			int i = 0, j = 0;
			for (i = 0; i < layer->neuralOut; i++) {/*
				printf("Val Neurona AUX W[%i][%i]=%g y PrevLayerOut[%i]=%g",i,layer->neuralIn,layer->W[i][layer->neuralIn],layer->neuralIn,-1.0);*/
				layer->out[i] = -layer->W[i][layer->neuralIn];
				for (j = 0; j < layer->neuralIn; j++) {/*
					printf("Val W[%i][%i]=%g y PrevLayerOut[%i]=%g",i,j,layer->W[i][j],j,layer->prevLayer->out[j]);*/
					layer->out[i] = layer->out[i]
							+ (layer->W[i][j] * layer->prevLayer->out[j]);
				}
				if (derived)
					layer->delta[i] = dg(/** /2.0 * / **/layer->out[i],
							percl_get_typeOfNeuron(layer, i));
				/* TODO: Evaluar posibilidad de si es type 0
				 * y es la ultima capa, usar out=sig(g)
				 * de lo contrario arreglar el problema de satisfaccion
				 * en el aprendizage a nivel net y no a nivel capa
				 * TODO: ver que aca el beta es fijo.
				 */
				layer->out[i] = g(/** /2.0 * / **/layer->out[i],
						percl_get_typeOfNeuron(layer, i));
			}
		}
		if (layer->nextLayer)
			res = percl_eval(layer->nextLayer, derived);
		else {
			int i = 0;
			res = (double *) calloc(layer->neuralOut, sizeof(double));
			if (res)
				for (i = 0; i < layer->neuralOut; i++)
					res[i] = layer->out[i];
		}
	}
	return res;
}

void percl_learn(percl_t layer, double *desired, double alphaMoment, double nu) {
	if (layer && layer->prevLayer) {
		int fail = 0, i = 0, j = 0;
		if (layer->nextLayer) {
			/*aca va lo de la capa no final */
			percl_t nl = layer->nextLayer;
			for (i = 0; i < layer->neuralOut; i++) {
				double d = .0;
				for (j = 0; j < nl->neuralOut; j++)
					d = d + nl->W[j][i] * nl->delta[j];
				layer->delta[i] = layer->delta[i] * d;
			}
		} else {
			if (desired) {
				/*aca va lo de la capa final */
				for (i = 0; i < layer->neuralOut; i++)
					layer->delta[i] = layer->delta[i]
							* (desired[i] - layer->out[i]);
			} else {
				fail = 1;
			}
		}
		if (!fail) {
			percl_learn(layer->prevLayer, NULL, alphaMoment, nu);
			/**
			 * aca biene el famoso nu del dW, el alfa de dWmomentum
			 * Agregar tambien a y b de nu adaptativo.
			 * TODO: ver seccion de funcion de costo alternativas (apuntes y libro)
			 * nu optimo segun el libro ~ .0476
			 * alfa segun el libro ~ .5 (~ 10*nu)
			 * para este nu usaremos a=.0003 y b=.09
			 */
			for (i = 0; i < layer->neuralOut; i++) {
				j = layer->neuralIn;
				layer->dWmoment[i][j] = nu * -layer->delta[i]
						+ alphaMoment * layer->dWmoment[i][j];
				layer->W[i][j] = layer->W[i][j] + layer->dWmoment[i][j];
				for (j = 0; j < layer->neuralIn; j++) {
					layer->dWmoment[i][j] = nu * layer->prevLayer->out[j]
							* layer->delta[i]
							+ alphaMoment * layer->dWmoment[i][j];
					layer->W[i][j] = layer->W[i][j] + layer->dWmoment[i][j];
				}
			}
			/**/
		}
	}
}
