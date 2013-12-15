/**
 * File:   perceptronNeuralNet.h
 *
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#include "perceptronNeuralNet.h"
#include "../lib/generadores.h"
#include "../lib/otraLibMas.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

percnn_t percnn_create_base(int numLayers, int* neuronsByLayer,
		double* muRetenContex, char* strFile, long* sem);
percnn_t percnn_upload_from_plainTextFile(char* strFile);
double *percnn_evalX(percnn_t net, double *in, int learnOrNot);
double *percnn_evalToLearn(percnn_t net, double **in, int *selec, int point);
void percnn_adaptNu(percnn_t net, double newError);
int percnn_lineError(percnn_t net, int len, double* res, double* desired,
		double *retErr);
void percnn_learn_online(percnn_t net, int numData, double **in,
		double **desired, int *selec);
void percnn_learn_batch(percnn_t net, int numData, double **in,
		double **desired, int *selec);

struct sPercNeuralNet {
	int numLayers, numRealIn, typeErr, numVIContext;
	long *sem, totalAttemps, attempsRemaining;
	double nu, alphaMoment, oldError, maxNu, minNu, upNu, downNu, /** / *lastErr,/ **/
	*muRetenContex, maxErr, minErr;
	percl_t first, last;
	char *strFile;
};

percnn_t percnn_create_base(int numLayers, int* neuronsByLayer,
		double* muRetenContex, char* strFile, long* sem) {
	percnn_t res = NULL;
	if (0 < numLayers && neuronsByLayer != NULL ) {
		res = (percnn_t) calloc(1, sizeof(struct sPercNeuralNet));
		if (res) {
			int i = 0, neurInMasContext = neuronsByLayer[0];
			res->numRealIn = neurInMasContext;
			if (muRetenContex) {
				double maxRetCont = .0;
				res->muRetenContex = (double *) calloc(numLayers + 1,
						sizeof(double));
				for (i = 0; i < numLayers + 1; i++) {
					res->muRetenContex[i] = muRetenContex[i];
					if (.0 <= muRetenContex[i] && muRetenContex[i] < 1.0) {
						if (maxRetCont < muRetenContex[i])
							maxRetCont = muRetenContex[i];
						neurInMasContext += neuronsByLayer[i];
					}
				}
				/* Error del 5% */
				res->numVIContext = 1
						+ log(.05 * (1.0 - maxRetCont)) / log(maxRetCont);
			} else {
				res->muRetenContex = NULL;
				res->numVIContext = 0;
			}
			neuronsByLayer[0] = neurInMasContext;
			if (strFile == NULL )
				strFile = "redPerceptronSinArchivoDeDestino.rnp";
			i = strlen(strFile);
			res->strFile = (char*) calloc(i + 5, sizeof(char));
			res->strFile = strncpy(res->strFile, strFile, i + 1);
			res->numLayers = numLayers;
			res->nu = .0476;
			res->alphaMoment = .5;
			res->maxNu = .057;
			res->minNu = .027;
			res->upNu = .0001;
			res->downNu = .985;
			res->oldError = .0;
			res->typeErr = 0;
			res->totalAttemps = 300;
			res->attempsRemaining = res->totalAttemps;
			res->maxErr = .999;
			res->minErr = .1;
			res->sem = sem;
		}
	}
	return res;
}

percnn_t percnn_upload_from_plainTextFile(char* strFile) {
	percnn_t res = NULL;
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
			l = fread(tmp, sizeof(char), 512, file);
			lacum += l;
			fromFile = (char *) realloc(fromFile, (lacum + 1) * sizeof(char));
			for (i = 0; i < l; i++)
				fromFile[i + ltemp] = tmp[i];
			i = 1;
		}
		if (lacum && fromFile) {
			fromFile[lacum] = '\0';
			res = percnn_from_str(fromFile);
			free(fromFile);
			if (res) {
				i = strlen(strFile);
				res->strFile = (char*) calloc(i + 5, sizeof(char));
				res->strFile = strncpy(res->strFile, strFile, i + 1);
			} else
				printf("ERROR al armar la net desde el str\n");
		} else
			printf("ERROR cargando la net... Archivo vacio!\n");
		fclose(file);
	} else
		printf("FAIL!!! No puedo abrir el archivo o el archivo no existe!!\n");
	return res;
}

/**
 * Crea una net perc con <numLayers> capas
 * y con las catidades de neuronas por capa dadas en <neuronsByLayer>
 * <types> indica de que type es cada capa. Sem es un puntero a un long
 * primo y negativo, para inicializar la semilla del random.
 * muRetenContex indica la retencion de cada capa de contexto, si
 * muRetenContex[i] esta fuera de [.0;1.0]; luego la capa[i] no es
 * retroalimentada; muRetenContex[i] cercano a .0, olvida rapido, cercano a
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
		double* muRetenContex, char* strFile, long* sem) {
	percnn_t res = NULL;
	int neurInBackup = 0, i = 0;
	if (types != NULL ) {
		if (neuronsByLayer)
			neurInBackup = neuronsByLayer[0];
		res = percnn_create_base(numLayers, neuronsByLayer, muRetenContex,
				strFile, sem);
		if (res) {
			res->first = percl_create(0, neuronsByLayer[0], 0, NULL, NULL, sem);
			res->last = res->first; /** /
			 res->lastErr = (double *) calloc(neuronsByLayer[numLayers - 1], sizeof (double));
			 if (res->lastErr)/ **/
			if (res->first)
				for (i = 0; res && i < numLayers; i++) {
					percl_t newLast = percl_create(neuronsByLayer[i],
							neuronsByLayer[i + 1], types[i + 1], NULL,
							res->last, sem);
					if (newLast) {
						percl_set_nextLayer(res->last, newLast);
						res->last = newLast;
					} else
						res = percnn_destroy(res);
				}
			else
				res = percnn_destroy(res); /** /
				 else
				 res = percnn_destroy(res);/ **/
			neuronsByLayer[0] = neurInBackup;
		} else if (strFile != NULL ) {
			res = percnn_upload_from_plainTextFile(strFile);
		} else
			printf(
					"ERROOR!!! Mal inicializada la net. Parametros incorrectos\n");
		if (neuronsByLayer)
			neuronsByLayer[0] = neurInBackup;
	}
	return res;
}

percnn_t percnn_create_with_neuronLayerMultiTypes(int numLayers,
		int* neuronsByLayer, int* neuralTypesByLayer, int** numNeuronOfType,
		neuralType** types, double* muRetenContex, char* strFile, long* sem) {
	percnn_t res = NULL;
	int neurInBackup = 0, i = 0;
	if (types != NULL ) {
		if (neuronsByLayer)
			neurInBackup = neuronsByLayer[0];
		res = percnn_create_base(numLayers, neuronsByLayer, muRetenContex,
				strFile, sem);
		if (res) {
			res->first = percl_create_MultiTypes(0, neuronsByLayer[0],
					neuralTypesByLayer[0], numNeuronOfType[0], types[0], NULL,
					NULL, sem);
			res->last = res->first; /** /
			 res->lastErr = (double *) calloc(neuronsByLayer[numLayers - 1], sizeof (double));
			 if (res->lastErr)/ **/
			if (res->first)
				for (i = 0; res && i < numLayers; i++) {
					percl_t newLast = percl_create_MultiTypes(neuronsByLayer[i],
							neuronsByLayer[i + 1], neuralTypesByLayer[i + 1],
							numNeuronOfType[i + 1], types[i + 1], NULL,
							res->last, sem);
					if (newLast) {
						percl_set_nextLayer(res->last, newLast);
						res->last = newLast;
					} else
						res = percnn_destroy(res);
				}
			else
				res = percnn_destroy(res); /** /
				 else
				 res = percnn_destroy(res);/ **/
			neuronsByLayer[0] = neurInBackup;
		} else if (strFile != NULL ) {
			res = percnn_upload_from_plainTextFile(strFile);
		} else
			printf(
					"ERROOR!!! Mal inicializada la net. Parametros incorrectos\n");
		if (neuronsByLayer)
			neuronsByLayer[0] = neurInBackup;
	}
	return res;
}

percnn_t percnn_create_with_memory(int numLayers, int* neuronsByLayer,
		neuralType* types, double* muRetenContex, char* strFile, long* sem,
		double*** W) {
	percnn_t res = NULL;
	int neurInBackup = 0;
	if (types != NULL ) {
		if (neuronsByLayer)
			neurInBackup = neuronsByLayer[0];
		res = percnn_create_base(numLayers, neuronsByLayer, muRetenContex,
				strFile, sem);
		if (res) {
			int wOk = (W != NULL ), i = 0, j = 0;
			for (i = 0; wOk && i < numLayers; i++) {
				wOk = (W[i] != NULL );
				for (j = 0; wOk && j < neuronsByLayer[i + 1]; j++) {
					wOk = (W[i][j] != NULL && W[i][j][neuronsByLayer[i]]);
				}
			}
			if (wOk) {
				res->first = percl_create(0, neuronsByLayer[0], 0, NULL, NULL,
						sem);
				res->last = res->first;
				if (res->first)
					for (i = 0; res && i < numLayers; i++) {
						percl_t newLast = percl_create_with_memory(
								neuronsByLayer[i], neuronsByLayer[i + 1],
								types[i + 1], NULL, res->last, W[i]);
						if (newLast) {
							percl_set_nextLayer(res->last, newLast);
							res->last = newLast;
						} else
							res = percnn_destroy(res);
					}
				else
					res = percnn_destroy(res);
				neuronsByLayer[0] = neurInBackup;
			} else {
				neuronsByLayer[0] = neurInBackup;
				res = percnn_destroy(res);
				res = percnn_create(numLayers, neuronsByLayer, types,
						muRetenContex, strFile, sem);
			}
		} else
			res = percnn_create(numLayers, neuronsByLayer, types, muRetenContex,
					strFile, sem);
		if (neuronsByLayer)
			neuronsByLayer[0] = neurInBackup;
	}
	return res;
}

percnn_t percnn_destroy(percnn_t net) {
	if (net) {
		percl_destroy(net->first);
		free(net->strFile); /** /
		 free(net->lastErr);/ **/
		free(net->muRetenContex);
		free(net);
	}
	return NULL ;
}

int percnn_save_plaintext(percnn_t net, char* strFile) {
	int res = 2, i = 0;
	if (net) {
		FILE *file = NULL;
		while (file == NULL && 0 < res) {
			if (strFile != NULL && (file = fopen(strFile, "w")) != NULL ) {
				i = strlen(strFile);
				free(net->strFile);
				net->strFile = (char*) calloc(i + 5, sizeof(char));
				net->strFile = strncpy(net->strFile, strFile, i + 1);
			} else {
				if (net->strFile) {
					i = strlen(net->strFile);
					strFile = (char*) calloc(i + 5, sizeof(char));
					strFile = strncpy(strFile, net->strFile, i + 1);
				}
				res--;
			}
		}
		if (strFile != NULL && 0 < res) {
			char *toFile = NULL;
			int lacum = 0, l = 0;
			toFile = percnn_to_str(net);
			lacum = strlen(toFile) + 1;
			l = fwrite(toFile, sizeof(char), lacum, file);
			free(toFile);
			if (lacum != l) {
				printf("ERROR guardando la net... Archivo incompleto!\n");
				res = 0;
			}
			fclose(file);
		} else
			printf(
					"FAIL!!! No puedo abrir el archivo o el archivo no existe!!\n");
	} else {
		res = 0;
		printf("La net es Nula!\n");
	}
	return res;
}

/**
 * Dado un strNeuralNet crea una net perc exactamente como la describe strNeuralNet
 */
percnn_t percnn_from_str(char *strNeuralNet) {
	int offset = 0, maxLen = 0;
	percnn_t res = NULL;
	if (strNeuralNet) {
		int i = 0;
		double maxMuRetCont = .0;
		maxLen = strlen(strNeuralNet);
		if (0 < maxLen) {
			res = (percnn_t) calloc(1, sizeof(struct sPercNeuralNet));
			res->strFile = (char*) calloc(500, sizeof(char));
			sscanf(strNeuralNet,
					" File: %s\nNumCapas: %i\tNumRealIn: %i\tnu: %lg\tmaxNu: %lg\tminNu: %lg\tupNu: %lg\tdownNu: %lg\talfaMomento: %lg\tmaxErr: %lg\tminErr: %lg\ttotalAttemps: %li\ttypeErr: %i",
					res->strFile, &(res->numLayers), &(res->numRealIn),
					&(res->nu), &(res->maxNu), &(res->minNu), &(res->upNu),
					&(res->downNu), &(res->alphaMoment), &(res->maxErr),
					&(res->minErr), &(res->totalAttemps), &(res->typeErr));
			res->oldError = .0;
			res->attempsRemaining = res->totalAttemps;
			res->muRetenContex = NULL;
			res->numVIContext = 0;
			offset = strFindEndIn(strNeuralNet, "mu Retencion", maxLen, offset);
			sscanf(&(strNeuralNet[offset]), " Contexto: %lg", &(maxMuRetCont));
			res->first = percl_from_str(strNeuralNet, NULL, NULL, maxLen,
					&offset);
			res->last = res->first;
			if (res->numRealIn != percl_get_num_Out(res->first)) {
				res->muRetenContex = (double *) calloc(res->numLayers + 1,
						sizeof(double));
				res->muRetenContex[0] = maxMuRetCont;
			}
			for (i = 0; res && i < res->numLayers && offset < maxLen; i++) {
				/**
				 * Deprecado!!!! Implementar en capaNeuronalElmanJordan o al del estilo
				 * /
				 if (res->muRetenContex) {
				 offset = strFindEndIn(strNeuralNet, "mu Retencion", maxLen,
				 offset);
				 sscanf(&(strNeuralNet[offset]), " Contexto: %lg",
				 &(res->muRetenContex[i + 1]));
				 if (.0 <= res->muRetenContex[i + 1]
				 && res->muRetenContex[i + 1] <= 1.0) {
				 if (maxMuRetCont < res->muRetenContex[i + 1])
				 maxMuRetCont = res->muRetenContex[i + 1];
				 }
				 }*/
				percl_t newLast = percl_from_str(strNeuralNet, NULL, res->last,
						maxLen, &offset);
				if (newLast) {
					percl_set_nextLayer(res->last, newLast);
					res->last = newLast;
				} else
					res = percnn_destroy(res);
			}
			if (res->muRetenContex)
				/* Error del 5% */
				res->numVIContext = 1
						+ log(.05 * (1.0 - maxMuRetCont)) / log(maxMuRetCont);
		}
	}
	return res;
}

/**
 * Pasa la net perc a str, guardando todos los detalles de la net
 * Separa el str en lineas, cada linea es una capa.
 */
char *percnn_to_str(percnn_t net) {
	char *res = NULL;
	int l = 575;
	if (net) {
		if (net->strFile)
			l += strlen(net->strFile);
		res = (char *) calloc(l, sizeof(char));
		if (res) {
			percl_t layer = net->first;
			int i = 0;
			l =
					sprintf(res,
							"File: %s\nNumCapas: %i\tNumRealIn: %i\tnu: %g\tmaxNu: %g\tminNu: %g\tupNu: %g\tdownNu: %g\talfaMomento: %g\tmaxErr: %g\tminErr: %g\ttotalAttemps: %li\ttypeErr: %i\nRed:\n",
							net->strFile, net->numLayers, net->numRealIn,
							net->nu, net->maxNu, net->minNu, net->upNu,
							net->downNu, net->alphaMoment, net->maxErr,
							net->minErr, net->totalAttemps, net->typeErr);
			while (layer && res) {
				char *tmp = NULL;
				tmp = percl_to_str(layer);
				if (net->muRetenContex)
					res = myVarStrCat(res,
							"Cap Num: %i\nmu Retencion Contexto: %g\n", i,
							net->muRetenContex[i]);
				else
					res = myVarStrCat(res,
							"Cap Num: %i\nmu Retencion Contexto: -1.0\n", i);
				res = myVarStrCat(res, tmp);
				layer = percl_get_nextLayer(layer);
				free(tmp);
				i++;
			}
			res[strlen(res)] = '\0';
		}
	} else
		printf("ups\n");
	return res;
}

int percnn_get_num_Context(percnn_t net) {
	if (net && net->first)
		return percl_get_num_Out(net->first) - net->numRealIn;
	return 0;
}

int percnn_get_num_In(percnn_t net) {
	if (net)
		return net->numRealIn;
	return 0;
}

int percnn_get_num_Out(percnn_t net) {
	if (net && net->last)
		return percl_get_num_Out(net->last);
	return 0;
}

int percnn_get_numLayers(percnn_t net) {
	if (net)
		return net->numLayers;
	return 0;
}

double percnn_get_nu(percnn_t net) {
	if (net)
		return net->nu;
	return .0476;
}

double percnn_get_maxNu(percnn_t net) {
	if (net)
		return net->maxNu;
	return .05;
}

double percnn_get_minNu(percnn_t net) {
	if (net)
		return net->minNu;
	return .027;
}

double percnn_get_upNu(percnn_t net) {
	if (net)
		return net->upNu;
	return .0001;
}

double percnn_get_downNu(percnn_t net) {
	if (net)
		return net->downNu;
	return .985;
}

double percnn_get_alphaMoment(percnn_t net) {
	if (net)
		return net->alphaMoment;
	return .5;
}

int percnn_get_numVIContext(percnn_t net) {
	if (net)
		return net->numVIContext;
	return 0;
}

int percnn_set_alphaMoment(percnn_t net, double newAlphaMoment) {
	if (net && .0 <= newAlphaMoment && newAlphaMoment < 1.0) {
		net->alphaMoment = newAlphaMoment;
		return 1;
	}
	return 0;
}

int percnn_set_allNu(percnn_t net, double newNu, double newMaxNu,
		double newMinNu, double newUpNu, double newDownNu) {
	if (net && .0 < newNu && newNu < 1.0 && newMinNu < newMaxNu && .0 < newMinNu
			&& newMaxNu < 1.0 && .0 < newUpNu && newUpNu < 1.0 && .0 < newDownNu
			&& newDownNu < 1.0) {
		net->nu = newNu;
		net->maxNu = newMaxNu;
		net->minNu = newMinNu;
		net->upNu = newUpNu;
		net->downNu = newDownNu;
		return 1;
	}
	return 0;
}

int percnn_set_percErrContext(percnn_t net, double porc) {
	if (net && .0 < porc && porc < 1.0) {
		double maxRetCont = .0;
		int i = 0;
		for (i = 0; i < net->numLayers + 1; i++)
			if (.0 <= net->muRetenContex[i] && net->muRetenContex[i] < 1.0
					&& maxRetCont < net->muRetenContex[i])
				maxRetCont = net->muRetenContex[i];
		net->numVIContext = 1
				+ log(porc * (1.0 - maxRetCont)) / log(maxRetCont);
		return 1;
	}
	return 0;
}

int percnn_set_lineError(percnn_t net, long totalAttemps, double minErr,
		double maxErr, int typeErr) {
	if (net) {
		minErr = dabs(minErr);
		maxErr = dabs(maxErr);
		if (maxErr < minErr) {
			double tmp = minErr;
			minErr = maxErr;
			maxErr = tmp;
		}
		if (minErr != .0)
			net->minErr = minErr;
		if (maxErr != .0)
			net->maxErr = maxErr;
		if (0 < totalAttemps)
			net->totalAttemps = totalAttemps;
		else
			net->totalAttemps = -totalAttemps + 1;
		net->attempsRemaining = net->totalAttemps;
		if (0 < typeErr)
			net->typeErr = typeErr;
		else
			net->typeErr = -typeErr;
		return 1;
	}
	return 0;
}

/**
 * El resultado es un array de double,
 * len(res)==percnn_get_num_Out(net)
 * (Si la ultima capa es de neuronas binarias el resultado es {1,-1})
 * in es la entrada a evaluar por la net,
 * len(in)==percnn_get_num_In(net).
 */
double *percnn_eval(percnn_t net, double *in) {
	return percnn_evalX(net, in, 0);
}

double *percnn_evalToLearn(percnn_t net, double **in, int *selec, int point) {
	/**
	 * Deprecado!!!! Implementar en capaNeuronalElmanJordan o al del estilo
	 * /
	 if (net && percnn_get_num_Context(net) != 0) {
	 int i = 0;
	 if (selec[point] == 0) {
	 percl_t tempC = net->first;
	 while (tempC) {
	 for (i = 0; i < tempC->neuralOut; i++)
	 tempC->out[i] = .0;
	 tempC = tempC->nextLayer;
	 }
	 } else if (point == 0
	 || net->numVIContext < selec[point] - selec[point - 1]
	 || selec[point] < selec[point - 1]) {
	 for (i = net->numVIContext; 0 < i; i--) {
	 if (i <= selec[point])
	 free(percnn_eval(net, in[selec[point] - i]));
	 }
	 } else if (selec[point - 1] < selec[point]) {
	 for (i = selec[point - 1] + 1; i < selec[point]; i++)
	 free(percnn_eval(net, in[i]));
	 }
	 }*/
	return percnn_evalX(net, in[selec[point]], 1);
}

double *percnn_evalX(percnn_t net, double *in, int learnOrNot) {
	double *res = NULL;
	if (net && net->first && in) {
		/*int i = 0, j = 0, k = 0;*/
		const int numIn = percnn_get_num_In(net);
		/*percl_t capaActual = net->first;*/
		percl_set_In(net->first, numIn, in);
		/**
		 * Deprecado!!!! Implementar en capaNeuronalElmanJordan o al del estilo
		 * /
		 if (net->muRetenContex && percnn_get_num_Context(net))
		 for (j = 0; j < net->numLayers + 1 && capaActual; j++) {
		 if (.0 <= net->muRetenContex[j] && net->muRetenContex[j] < 1.0)
		 for (k = 0;
		 i < percl_get_num_Out(net->first)
		 && ((capaActual == net->first && k < numIn)
		 || (capaActual != net->first
		 && k < percl_get_num_Out(capaActual)));
		 k++) {
		 net->first->out[i] = net->first->out[i]
		 * net->muRetenContex[j] + capaActual->out[k];
		 i++;
		 }
		 capaActual = capaActual->nextLayer;
		 }*/
		res = percl_eval(net->first, learnOrNot);
	} else
		printf(
				"No se puede evaluaaar!!!\nPor:\nred: %i\tfirstCap: %i\tin: %i\n",
				net == NULL, net->first == NULL, in == NULL );
	return res;
}

void percnn_adaptNu(percnn_t net, double newError) {
	if (newError < net->oldError) {
		if (net->nu < net->maxNu) {
			net->nu = net->nu + net->upNu;
		}
	} else if (net->oldError < newError) {
		if (net->minNu < net->nu) {
			net->nu = net->nu * net->downNu;
		}
	}
	net->oldError = newError;
}

int percnn_lineError(percnn_t net, int len, double* res, double* desired,
		double *retErr) {
	int fail = 0, j;
	double limitErr = net->maxErr
			- ((net->maxErr - net->minErr) * ((double) net->attempsRemaining)
					/ ((double) net->totalAttemps));
	*retErr = .0;
	switch (net->typeErr) {
	case 1:
		for (j = 0; j < len; j++)
			*retErr = *retErr + dabs(res[j] - desired[j]);
		*retErr = *retErr / (double) j;
		fail = (limitErr < *retErr);
		break;
	default:
		for (j = 0; j < len; j++) {
			double localErr = dabs(res[j] - desired[j]);
			if (*retErr < localErr)
				*retErr = localErr;
			fail |= (limitErr < *retErr);
		}
		break;
	}
	return fail;
}

void percnn_learn_online(percnn_t net, int numData, double **in,
		double **desired, int *selec) {
	if (net && desired && in && selec && desired[numData - 1]
			&& in[numData - 1]) {
		double newError = .0, *res = NULL;
		int i = 0;
		for (i = 0; i < numData; i++) {
			int fail = 1;
			long localAttempsRemaining = net->attempsRemaining;
			free(percnn_evalToLearn(net, in, selec, i));
			while (fail && 0 < localAttempsRemaining) {
				localAttempsRemaining--;
				percl_learn(net->last, desired[selec[i]], net->alphaMoment,
						net->nu);
				res = percnn_evalToLearn(net, in, selec, i);
				fail = percnn_lineError(net, percnn_get_num_Out(net), res,
						desired[selec[i]], &newError);
				free(res);
				percnn_adaptNu(net, newError);
			}
		}
	}
}

void percnn_learn_batch(percnn_t net, int numData, double **in,
		double **desired, int *selec) {
	if (net && desired && in && selec && desired[numData - 1]
			&& in[numData - 1]) {
		int i = 0;
		for (i = 0; i < numData; i++) {
			free(percnn_evalToLearn(net, in, selec, i));
			percl_learn(net->last, desired[selec[i]], net->alphaMoment,
					net->nu);
		}
	}
}

/**
 * La net aprende el ultimo valor evaluado
 * desired es un array de doubles que refleja el resultado que deberia
 * haber teniedo neurona a neurona de sallida para el ultimo in evaluado
 * len(desired)==percnn_get_num_Out(net)
 * maxErr es el maximo error permtido... (ya vamos a ver en que y donde)
 */
int percnn_learn(percnn_t net, int numData, double** in, double** desired,
		method learnBy) {
	int restConcepts = numData, i = 0;
	net->attempsRemaining = net->totalAttemps;
	if (net && desired && in && desired[numData - 1] && in[numData - 1]) {
		double *res = NULL;
		int *selec = NULL;
		selec = (int *) calloc(numData, sizeof(int));
		if (selec) {
			net->oldError = 1000000.0 * (double) numData;
			for (i = 0; i < numData; i++) {
				selec[i] = i;
			}
			/* Learn all the concepts. */
			while (0 < restConcepts && 0 < net->attempsRemaining) {
				double actualError, pActError = .0, pActFailError = .0;
				/** /printf("AttempsRemaining: %ld\tRestConcepts: %i\tNu: %g\n",net->attempsRemaining,restConcepts,net->nu);/ **/
				net->attempsRemaining--;
				/* Merging the concepts */
				for (i = restConcepts; 0 < i; i--) {
					int r = i * ran2(net->sem), tmp = 0;
					tmp = selec[i - 1];
					selec[i - 1] = selec[r];
					selec[r] = tmp;
				}
				/* Learning */
				switch (learnBy) {
				case online:
					percnn_learn_online(net, restConcepts, in, desired, selec);
					break;
				default:
					percnn_learn_batch(net, restConcepts, in, desired, selec);
					break;
				}
				/* Checking forgotten concepts. */
				restConcepts = 0;
				for (i = 0; i < numData; i++) {
					int fail = 0;
					res = percnn_eval(net, in[i]);
					fail = percnn_lineError(net, percnn_get_num_Out(net), res,
							desired[i], &actualError);
					pActError = pActError + actualError;
					free(res);
					if (fail) {
					pActFailError = pActFailError + actualError;
						selec[restConcepts] = i;
						restConcepts++;
					}
				}
				/** /printf("IntenRest=%ld\tConcFalta=%i\tError=%f\tErrorFailer=%f\tNu=%g\n",
				 net->attempsRemaining, restConcepts,
				 pActError / (double) numData,
				 pActFailError / (double) restConcepts,
				 net->nu);/ **/
				percnn_adaptNu(net, (double) restConcepts);
			}
			free(selec);
		}
	}
	return net->attempsRemaining;
}

int percnn_test(percnn_t net, int numData, double** in, double** desired,
		tMode testBy) {
	int result = 0, i = 0, j = 0, len = percnn_get_num_Out(net);
	double *res = NULL;
	for (i = 0; i < numData; i++) {
		int fail = 0;
		res = percnn_eval(net, in[i]);
		for (j = 0; !fail && j < len; j++) {
			switch (testBy) {
			case delta:
				fail = 1;
				break;
			default:
				fail = (res[j] * desired[i][j] < .0);
				break;
			}
		}
		free(res);
		if (!fail)
			result++;
	}
	return result;
}
