/*
 * $ Author: Eric Nahuel Jurio $
 * $ Date: 22/1/2011 19:22 $
 * $ License: GPL v3 $
 *
 */

#include "neural-networks/lib/generadores.h"
#include "neural-networks/lib/otraLibMas.h"
#include "neural-networks/perceptron/perceptronNeuralNet.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

void printHelp(char* invalidOption);
int ejer2(int hypORln, int printRN);
int ejer3(void);
int ejer1F(int selecFun, int p, int printRN);
int ejer2F(int selecFun, int p, int printRN);
int ejer2Fpruebita(int selecFun, int p, int printRN);
int ejer3F(int selecFun, int p, int printRN);


int ejer2(int hypORln, int printRN) {
    int i = 0, p = 20, conMomento = 1;
    double **res = NULL, **in = NULL, *test = NULL;
    long sem = -131;
    res = (double**) calloc(p, sizeof (double*));
    in = (double**) calloc(p, sizeof (double*));
    if (res && in) {
        percnn_t red = NULL;
        int numNeuronas[3];
		neuralType tipoNeuronas[3];
        for (i = 0; i < p; i++) {
            res[i] = (double*) calloc(1, sizeof (double));
            in[i] = (double*) calloc(1, sizeof (double));
        }
        numNeuronas[0] = 1;
        numNeuronas[2] = 1;
		tipoNeuronas[0] = tanhyp;
		tipoNeuronas[1] = tanhyp;
		tipoNeuronas[2] = lineal;
        for (p = 5; p < 24; p *= 2) {
            if (hypORln) {
                numNeuronas[1] = 4;
                for (i = 0; i < p; i++) {
                    in[i][0] = 1.0 + ran2(&sem)*4.0;
                    res[i][0] = 1.0 / in[i][0];
                }
            } else {
                numNeuronas[1] = 6;
                for (i = 0; i < p; i++) {
                    in[i][0] = 1.0 + ran2(&sem)*4.0;
                    res[i][0] = log(in[i][0]);
                }
            }
			red = percnn_create(2, numNeuronas, tipoNeuronas, NULL, NULL, &sem);
            if (red) {
                double err = ((double) (numNeuronas[1] - 2)) / 200.0;
                int numIntentos = 0;
				if (!conMomento)
					percnn_set_alphaMoment(red, .0);
				percnn_set_lineError(red, 100000, err, err, 0);
				numIntentos = percnn_learn(red, p, in, res, online);
                if (numIntentos) {
                    if (p < 10) {
                        printf("f(x):\n\n");
                        for (i = 0; i < 100; i++) {
                            double x = 1.0 + ((double) i)*.04;
							if (hypORln)
								printf("%f\t%f\n", x, 1.0 / x);
							else
								printf("%f\t%f\n", x, log(x));
                        }
                    }
					printf(
							"\nIntentos: %i\nred(x): x in [1,5]: (solo %i puntos aleatorios)\n\n",
							numIntentos, p);
                    for (i = 0; i < p; i++) {
						test = percnn_eval(red, in[i]);
                        printf("%f\t%f\n", in[i][0], test[0]);
                        free(test);
                    }
                    printf("\nred(x): x in [1,5]:\n\n");
                    for (i = 0; i < 100; i++) {
                        double x = 1.0 + ((double) i)*.04;
						test = percnn_eval(red, &x);
                        printf("%f\t%f\n", x, test[0]);
                        free(test);
                    }
                    if (printRN) {
						char *str = percnn_to_str(red);
                        printf("%s\n\n\n", str);
                        free(str);
                    }
				} else
					printf("No aprendí :-S :'(  TT__TT\n");
				red = percnn_destroy(red);
			} else
				printf("OO OOO\n");
        }
        p = 20;
        for (i = 0; i < p; i++) {
            free(res[i]);
            free(in[i]);
        }
        free(res);
        free(in);
    }
    return 0;
}

/**
 * Prueba comparativa entre actF tanhyp, hyp2, log2 sobre hyp y log
 */
int ejer1F(int selecFun, int p, int printRN) {
	int i = 0, conMomento = 1;
	double **res = NULL, **in = NULL/*, *test = NULL*/;
	long sem = -131;
	res = (double**) calloc(p, sizeof(double*));
	in = (double**) calloc(p, sizeof(double*));
	if (res && in) {
		percnn_t red = NULL;
		int numLayers = 2, numNeuronas[3], adaptGrowMode = 0;
		neuralType tipoNeuronas[3];
		for (i = 0; i < p; i++) {
			res[i] = (double*) calloc(1, sizeof(double));
			in[i] = (double*) calloc(1, sizeof(double));
		}
		numNeuronas[0] = 1;
		numNeuronas[2] = 1;
		tipoNeuronas[0] = tanhyp;
		tipoNeuronas[1] = tanhyp;
		tipoNeuronas[2] = lineal;
		switch (selecFun) {
		case 1:
			numNeuronas[1] = 4;
			for (i = 0; i < p; i++) {
				in[i][0] = 1.0 + ran2(&sem) * 4.0;
				res[i][0] = 1.0 / in[i][0];
			}
			break;
		case 2:
			numNeuronas[1] = 6;
			for (i = 0; i < p; i++) {
				in[i][0] = 1.0 + ran2(&sem) * 4.0;
				res[i][0] = log(in[i][0]);
			}
			break;
		case 3:
			numNeuronas[1] = 4;
			tipoNeuronas[1] = hyp2;
			for (i = 0; i < p; i++) {
				in[i][0] = 1.0 + ran2(&sem) * 4.0;
				res[i][0] = 1.0 / in[i][0];
			}
			break;
		case 4:
			numNeuronas[1] = 6;
			tipoNeuronas[1] = hyp2;
			for (i = 0; i < p; i++) {
				in[i][0] = 1.0 + ran2(&sem) * 4.0;
				res[i][0] = log(in[i][0]);
			}
			break;
		case 5:
			numNeuronas[1] = 4;
			tipoNeuronas[1] = ln2p;
			for (i = 0; i < p; i++) {
				in[i][0] = 1.0 + ran2(&sem) * 4.0;
				res[i][0] = 1.0 / in[i][0];
			}
			break;
		case 6:
			numNeuronas[1] = 6;
			tipoNeuronas[1] = ln2p;
			for (i = 0; i < p; i++) {
				in[i][0] = 1.0 + ran2(&sem) * 4.0;
				res[i][0] = log(in[i][0]);
			}
			break;
		case 7:
			numLayers=1;
			numNeuronas[1] = 1;
			tipoNeuronas[1] = lineal;
			adaptGrowMode = 1;
			printRN=1;
			for (i = 0; i < p; i++) {
				in[i][0] = 1.0 + ran2(&sem) * 4.0;
				res[i][0] = 1.0 / in[i][0];
			}
			break;
		case 8:
			numLayers=1;
			numNeuronas[1] = 1;
			tipoNeuronas[1] = lineal;
			adaptGrowMode = 1;
			printRN=1;
			for (i = 0; i < p; i++) {
				in[i][0] = 1.0 + ran2(&sem) * 4.0;
				res[i][0] = log(in[i][0]);
			}
			break;
		default:
			if (p < 5) {
				printf("f(x):\n\n");
				for (i = 0; i < 200; i++) {
					double x = .0001 + ((double) i) * .05;
					if (p) {
						/*x = x + .1;*/
						printf("%f\t%f\n", x, 1.0 / x);
					} else {
						printf("%f\t%f\n", x, log(x));
					}
				}
			}
			break;
		}
		if (selecFun) {
			red = percnn_create(numLayers, numNeuronas, tipoNeuronas, NULL, NULL, &sem);
			if (red) {
				double err = ((double) (numNeuronas[1] - 2)) / 200.0;
				int numIntentos = 0;
				if (!conMomento)
					percnn_set_alphaMoment(red, .0);
				percnn_setAdaptativelyGrowMode(red, adaptGrowMode);
				percnn_set_lineError(red, 100000, err, err, 0);
				numIntentos = percnn_learn(red, p, in, res, online);
				if (numIntentos) {printRN++;/*
					printf(
							"\nIntentos: %i\nred(x): x in [1,5]: (solo %i puntos aleatorios)\n\n",
							numIntentos, p);
					for (i = 0; i < p; i++) {
						test = percnn_eval(red, in[i]);
						printf("%f\t%f\n", in[i][0], test[0]);
						free(test);
					}
					printf("\nred(x): x in [0,10]:\n\n");
					for (i = 0; i < 200; i++) {
						double x = ((double) i) * .05;
						test = percnn_eval(red, &x);
						printf("%f\t%f\n", x, test[0]);
						free(test);
					}
					if (printRN) {
						char *str = percnn_to_str(red);
						printf("%s\n\n\n", str);
						free(str);
					}
				} else {
					char *str = percnn_to_str(red);
					printf("%s\n\n\n", str);
					free(str);
					printf("No aprendí :-S :'(  TT__TT %i\n",percnn_getAdaptativelyGrowMode(red));
				*/}
				red = percnn_destroy(red);
			} else
				printf("OO OOO\n");
		}
		for (i = 0; i < p; i++) {
			free(res[i]);
			free(in[i]);
		}
		free(res);
		free(in);
	}
	return 0;
}

/**
 * Entrenando X*Y comparacion con actF tanhyp y con pow2
 */
int ejer2F(int selecFun, int p, int printRN) {
	int i = 0, conMomento = 1;
	double **res = NULL, **in = NULL, *test = NULL;
	long sem = -131;
	res = (double**) calloc(p, sizeof(double*));
	in = (double**) calloc(p, sizeof(double*));
	if (res && in) {
		percnn_t red = NULL;
		int numNeuronas[3];
		neuralType tipoNeuronas[3];
		for (i = 0; i < p; i++) {
			res[i] = (double*) calloc(1, sizeof(double));
			in[i] = (double*) calloc(2, sizeof(double));
			in[i][0] = ran2(&sem) * 20.0 - 10.0;
			in[i][1] = ran2(&sem) * 20.0 - 10.0;
			res[i][0] = in[i][0] * in[i][1];
		}
		numNeuronas[0] = 2;
		numNeuronas[2] = 1;
		tipoNeuronas[0] = tanhyp;
		tipoNeuronas[1] = tanhyp;
		tipoNeuronas[2] = lineal;
		switch (selecFun) {
		case 1:
			numNeuronas[1] = 6;
			break;
		case 2:
			/* primero intentamos con 2 y no aprendio */
			numNeuronas[1] = 3;
			tipoNeuronas[1] = pow2;
			break;
		default:
			if (p < 5) {
				printf("f(x):\n\n");
				for (i = 0; i < 200; i++) {
					double x = ((double) i) - 100.0;
					int j = 0;
					for (j = 0; j < 200; j++) {
						double y = ((double) j) - 100.0;
						printf("%f\t%f\t%f\n", x, y, x * y);
					}
				}
			}
			break;
		}
		if (selecFun) {
			red = percnn_create(2, numNeuronas, tipoNeuronas, NULL, NULL, &sem);
			if (red) {
				double err = .02;
				int numIntentos = 0;
				if (!conMomento)
					percnn_set_alphaMoment(red, .0);
				/*
				res->nu = .0476;
				res->alphaMoment = .5;
				res->maxNu = .057;
				res->minNu = .027;
				res->upNu = .0001;
				res->downNu = .985;*/
		                switch (selecFun) {
                		case 1:
                                        percnn_set_allNu(red, .9, .99, 1e-69, .005, .5);
		                        break;
		                case 2:
				percnn_set_allNu(red, .00000008, .99, 1e-69, .00000000005, .1);
                		        break;
		                default:
					break;
				}
				percnn_set_lineError(red, 100000, err, err, 0);
				numIntentos = percnn_learn(red, p, in, res, online);
				if (numIntentos) {
					printf(
							"\nIntentos: %i\nred(x): x in [-10,10][-10,10]: (solo %i puntos aleatorios)\n\n",
							numIntentos, p);
					for (i = 0; i < p; i++) {
						test = percnn_eval(red, in[i]);
						printf("%f\t%f\t%f\n", in[i][0], in[i][1], /*res[i][0],*/
						test[0]);
						free(test);
					}
					printf("\nred(x): x in [-100,100][-100,100]:\n\n");
					for (i = 0; i < 200; i++) {
						double x[2];
						int j = 0;
						x[0] = ((double) i) - 100.0;
						for (j = 0; j < 200; j++) {
							x[1] = ((double) j) - 100.0;
							test = percnn_eval(red, x);
							printf("%f\t%f\t%f\n", x[0], x[1], test[0]);
							free(test);
						}
					}
					if (printRN) {
						char *str = percnn_to_str(red);
						printf("%s\n\n\n", str);
						free(str);
					}
				} else
					printf("No aprendí :-S :'(  TT__TT\n");
				red = percnn_destroy(red);
			} else
				printf("OO OOO\n");
		}
		for (i = 0; i < p; i++) {
			free(res[i]);
			free(in[i]);
		}
		free(res);
		free(in);
	}
	return 0;
}/**/

/**
 * It's only an other TC
 * /
int ejer2F(int selecFun, int p, int printRN) {
	int i = 0, conMomento = 1;
	double **res = NULL, **in = NULL, *test = NULL;
	long sem = -131;
	res = (double**) calloc(p, sizeof(double*));
	in = (double**) calloc(p, sizeof(double*));
	if (res && in) {
		percnn_t red = NULL;
		int numNeuronas[2];
		neuralType tipoNeuronas[2];
		for (i = 0; i < p; i++) {
			res[i] = (double*) calloc(1, sizeof(double));
			in[i] = (double*) calloc(1, sizeof(double));
			in[i][0] = ran2(&sem) * 20.0 - 10.0;
			res[i][0] = in[i][0] * in[i][0];
		}
		numNeuronas[0] = 1;
		numNeuronas[1] = 1;
		tipoNeuronas[0] = tanhyp;
		tipoNeuronas[1] = lineal;
		switch (selecFun) {
		case 1:
			numNeuronas[1] = 1;
			break;
		case 2:
			numNeuronas[1] = 1;
			tipoNeuronas[1] = pow2;
			break;
		default:
			if (p < 5) {
				printf("f(x):\n\n");
				for (i = 0; i < 200; i++) {
					double x = ((double) i) - 100.0;
					printf("%f\t%f\n", x, x * x);
				}
			}
			break;
		}
		if (selecFun) {
			red = percnn_create(1, numNeuronas, tipoNeuronas, NULL, NULL, &sem);
			if (red) {
				double err = .02;
				int numIntentos = 0;
				if (!conMomento)
					percnn_set_alphaMoment(red, .0);
				/ *
				res->nu = .0476;
				res->alphaMoment = .5;
				res->maxNu = .057;
				res->minNu = .027;
				res->upNu = .0001;
				res->downNu = .985;* /
				percnn_set_allNu(red, .00008, .99, 1e-69, .00000005, .01);
				percnn_set_lineError(red, 10/ *0000* /, err, err, 0);
				numIntentos = percnn_learn(red, p, in, res, online);
				if (numIntentos) {
					printf(
							"\nIntentos: %i\nred(x): x in [-10,10]: (solo %i puntos aleatorios)\n\n",
							numIntentos, p);
					for (i = 0; i < p; i++) {
						test = percnn_eval(red, in[i]);
						printf("%f\t%f\n", in[i][0], / *res[i][0],* /
						test[0]);
						free(test);
					}
					printf("\nred(x): x in [-100,100]:\n\n");
					for (i = 0; i < 200; i++) {
						double x[1];
						x[0] = ((double) i) - 100.0;
						test = percnn_eval(red, x);
						printf("%f\t%f\n", x[0], test[0]);
						free(test);
					}
					if (printRN) {
						char *str = percnn_to_str(red);
						printf("%s\n\n\n", str);
						free(str);
					}
				} else
					printf("No aprendí :-S :'(  TT__TT\n");
				red = percnn_destroy(red);
			} else
				printf("OO OOO\n");
		}
		for (i = 0; i < p; i++) {
			free(res[i]);
			free(in[i]);
		}
		free(res);
		free(in);
	}
	return 0;
}
/ **/
/**
 * Otra pruebita Entrenando X^2 comparacion con actF tanhyp y con pow2
 * Notar que esta en otro rango los datos de prueba
 */
int ejer2Fpruebita(int selecFun, int p, int printRN) {
	int i = 0/*, conMomento = 1*/;
	double **res = NULL, **in = NULL, *test = NULL;
	long sem = -131;
	res = (double**) calloc(p, sizeof(double*));
	in = (double**) calloc(p, sizeof(double*));
	if (res && in) {
		percnn_t red = NULL;
		int numNeuronas[3];
		neuralType tipoNeuronas[3];
		for (i = 0; i < p; i++) {
			res[i] = (double*) calloc(1, sizeof(double));
			in[i] = (double*) calloc(2, sizeof(double));
			in[i][0] = ran2(&sem) * 2.0 - 1.0;
			in[i][1] = ran2(&sem) * 2.0 - 1.0;
			res[i][0] = in[i][0] * in[i][1];
		}
		numNeuronas[0] = 2;
		numNeuronas[2] = 1;
		tipoNeuronas[0] = tanhyp;
		tipoNeuronas[1] = tanhyp;
		tipoNeuronas[2] = lineal;
		switch (selecFun) {
		case 1:
			numNeuronas[1] = 12;
			break;
		case 2:
			numNeuronas[1] = 2;
			tipoNeuronas[1] = pow2;
			break;
		default:
			if (p < 5) {
				printf("f(x):\n\n");
				for (i = 0; i < 200; i++) {
					double x = (((double) i)/10.0) - 10.0;
					int j = 0;
					for (j = 0; j < 200; j++) {
						double y = (((double) j)/10.0) - 10.0;
						printf("%f\t%f\t%f\n", x, y, x * y);
					}
				}
			}
			break;
		}
		if (selecFun) {
			red = percnn_create(2, numNeuronas, tipoNeuronas, NULL, NULL, &sem);
			if (red) {
				double err = .002;
				int numIntentos = 0;
				percnn_set_lineError(red, 10000000, err, err, 0);
				/*
				res->nu = .0476;
				res->alphaMoment = .5;
				res->maxNu = .057;
				res->minNu = .027;
				res->upNu = .0001;
				res->downNu = .985;*/
				/*percnn_set_allNu(red, .00008, .99, 1e-69, .00000005, .01);
				percnn_set_allNu(red, .0001, .002, .00001, .0001, .5);*/
				numIntentos = percnn_learn(red, p, in, res, online/*batch*/);
				if (numIntentos) {
					printf(
							"\nIntentos: %i\nred(x): x in [-10,10][-10,10]: (solo %i puntos aleatorios)\n\n",
							numIntentos, p);
					for (i = 0; i < p; i++) {
						test = percnn_eval(red, in[i]);
						printf("%f\t%f\t%f\n", 10.0*in[i][0], 10.0*in[i][1], /*res[i][0],*/
						100.0*test[0]);
						free(test);
					}
					printf("\nred(x): x in [-100,100][-100,100]:\n\n");
					for (i = 0; i < 200; i++) {
						double x[2];
						int j = 0;
						x[0] = (((double) i)/10.0) - 10.0;
						for (j = 0; j < 200; j++) {
							x[1] = (((double) j)/10.0) - 10.0;
							test = percnn_eval(red, x);
							printf("%f\t%f\t%f\n", 10.0*x[0], 10.0*x[1], 100.0*test[0]);
							free(test);
						}
					}
					if (printRN) {
						char *str = percnn_to_str(red);
						printf("%s\n\n\n", str);
						free(str);
					}
				} else
					printf("No aprendí :-S :'(  TT__TT\n");
				red = percnn_destroy(red);
			} else
				printf("OO OOO\n");
		}
		for (i = 0; i < p; i++) {
			free(res[i]);
			free(in[i]);
		}
		free(res);
		free(in);
	}
	return 0;
}

/**
 * Entrenando (sin(X)*sin(Y))/(X*Y)
 * comparacion con actF tanhyp y capa oculta mixta con sin
 */
int ejer3F(int selecFun, int p, int printRN) {
	int i = 0, conMomento = 1;
	double **res = NULL, **in = NULL, *test = NULL;
	long sem = -131;
	res = (double**) calloc(p, sizeof(double*));
	in = (double**) calloc(p, sizeof(double*));
	if (res && in) {
		percnn_t red = NULL;
		int numNeuronas[4], nTypesByLayers[] = { 1, 2, 1, 1 }, **nnTypes = NULL;
		neuralType **tipos = NULL;
		for (i = 0; i < p; i++) {
			res[i] = (double*) calloc(1, sizeof(double));
			in[i] = (double*) calloc(2, sizeof(double));
			in[i][0] = ran2(&sem) * 20.0 - 10.0;
			in[i][1] = ran2(&sem) * 20.0 - 10.0;
			res[i][0] = sin(in[i][0]) * sin(in[i][1]) / (in[i][0] * in[i][1]);
		}
		printf("\nComenzando prueba ejer3F\n");
		numNeuronas[0] = 2;
		numNeuronas[1] = 6;
		numNeuronas[2] = 4;
		numNeuronas[3] = 1;
		nnTypes = calloc(4, sizeof(int*));
		for (i = 0; i < 4; i++) {
			nnTypes[i] = calloc(2, sizeof(int));
			nnTypes[i][1] = 0;
		}
		tipos = calloc(4, sizeof(neuralType*));
		for (i = 0; i < 4; i++) {
			tipos[i] = calloc(2, sizeof(neuralType));
			tipos[i][0] = tanhyp;
			tipos[i][1] = tanhyp;
		}
		nnTypes[0][0] = 2;
		nnTypes[1][0] = 6;
		nnTypes[2][0] = 4;
		nnTypes[3][0] = 1;
		/**/tipos[3][0] = lineal;/**/
		tipos[1][1] = seno;
		/**/tipos[3][1] = lineal;/**/
		switch (selecFun) {
		case 1:
			break;
		case 2:
			nnTypes[1][0] = 4;
			nnTypes[1][1] = 2;
			break;
		default:
			if (p < 5) {
				printf("f(x):\n\n");
				for (i = 0; i < 200; i++) {
					double x = ((double) i) - 100.0;
					int j = 0;
					for (j = 0; j < 200; j++) {
						double y = ((double) j) - 100.0;
						printf("%f\t%f\t%f\n", x, y, x * y);
					}
				}
			}
			break;
		}
		if (selecFun) {
			red = percnn_create_with_neuronLayerMultiTypes(3, numNeuronas,
					nTypesByLayers, nnTypes, tipos, NULL, NULL, &sem);
			if (red) {
				double err = .02;
				int numIntentos = 0;
				/**
				 res->nu = .0476;
				 res->alphaMoment = .5;
				 res->maxNu = .057;
				 res->minNu = .027;
				 res->upNu = .0001;
				 res->downNu = .985;
				 */
				percnn_set_allNu(red, .0476, .05, .046, .0001, .985);/**/
				if (!conMomento)
					percnn_set_alphaMoment(red, .0);
				percnn_set_lineError(red, 10000000, err, err, 0);
				numIntentos = percnn_learn(red, p, in, res, online);
				if (numIntentos) {
					printf(
							"\nIntentos: %i\nred(x): x in [-10,10][-10,10]: (solo %i puntos aleatorios)\n\n",
							numIntentos, p);
					for (i = 0; i < p; i++) {
						test = percnn_eval(red, in[i]);
						printf("%f\t%f\t%f\n", in[i][0], in[i][1], /*res[i][0],*/
						test[0]);
						free(test);
					}
					printf("\nred(x): x in [-100,100][-100,100]:\n\n");
					for (i = 0; i < 200; i++) {
						double x[2];
						int j = 0;
						x[0] = ((double) i) - 100.0;
						for (j = 0; j < 200; j++) {
							x[1] = ((double) j) - 100.0;
							test = percnn_eval(red, x);
							printf("%f\t%f\t%f\n", x[0], x[1], test[0]);
							free(test);
						}
					}
					if (printRN) {
						char *str = percnn_to_str(red);
						printf("%s\n\n\n", str);
						free(str);
					}
				} else
					printf("No aprendí :-S :'(  TT__TT\n");
				red = percnn_destroy(red);
			} else
				printf("OO OOO\n");
		}
		for (i = 0; i < p; i++) {
			free(res[i]);
			free(in[i]);
		}
		free(res);
		free(in);
		free(nnTypes);
		free(tipos);
	}
	return 0;
}

/**
 * Ejer de red codificadora 8:3:8 y 8:5:8
 */
int ejer3(void) {
    int i = 0, j = 0, p = 8, conMomento = 1;
    double **res = NULL, **in = NULL;
    long sem = -131;
    res = (double**) calloc(p, sizeof (double*));
    in = (double**) calloc(p, sizeof (double*));
    if (res && in) {
        percnn_t red = NULL;
		char strFiles[2][60] = { "../dat/redCodificadoraPract3Ejer3-8a3a8.rnp",
				"../dat/redCodificadoraPract3Ejer3-8a5a8.rnp" };
        int numNeuronas[3];
		neuralType tipoNeuronas[3];
        for (i = 0; i < p; i++) {
            res[i] = (double*) calloc(p, sizeof (double));
            in[i] = (double*) calloc(p, sizeof (double));
        }
        for (i = 0; i < p; i++)
            for (j = 0; j < p; j++) {
                if (i == j) {
                    in[i][j] = 1.0;
                    res[i][j] = 1.0;
                } else {
                    in[i][j] = -1.0;
                    res[i][j] = -1.0;
                }
            }
        numNeuronas[0] = 8;
        numNeuronas[2] = 8;
		tipoNeuronas[0] = tanhyp;
		tipoNeuronas[1] = tanhyp;
		tipoNeuronas[2] = tanhyp;
        for (p = 0; p < 2; p++) {
            numNeuronas[1] = 3 + p * 2;
			red = percnn_create(2, numNeuronas, tipoNeuronas, NULL, strFiles[p],
					&sem);
            if (red) {
                int numIntentos = 0;
				if (!conMomento)
					percnn_set_alphaMoment(red, .0);
				percnn_set_lineError(red, 100000, .015, .0199, 0);
				numIntentos = percnn_learn(red, 8, in, res, online);
                if (numIntentos) {
                    char *str = NULL;
					str = percnn_to_str(red);
					percnn_save_plaintext(red, NULL );
                    printf("Num de Intentos: %i\n%s\n\n", numIntentos, str);
                    free(str);
				} else
					printf("No aprendí :-S :'(  TT__TT\n");
				red = percnn_destroy(red);
			} else
				printf("OO OOO\n");
        }
        p = 8;
        for (i = 0; i < p; i++) {
            free(res[i]);
            free(in[i]);
        }
        free(res);
        free(in);
    }
    return 0;
}

void printHelp(char* invalidOption) {
	if (invalidOption != NULL )
		printf("No me gusto el \"%s\"\n", invalidOption);
	printf("Debe ingresar:\n\t-E[e] <num_de_ejer>\n\t-N[n] <num_de_Neuronas>\n\t-M[m] <momentum>\n\t-V[v] <Verbose>\n\t-P[p] <practico>\n\n");
}

int main(int argc, char** argv) {
	int i = 1, ejer = 0, momento = 0, tam = 0, printRN = 0, practicSelected = 0;
	if (argc < 3) {
		printHelp(NULL );
		return EXIT_FAILURE;
	}
	while (i + 1 < argc) {
        if (!strcmp(argv[i], "-E") || !strcmp(argv[i], "-e")) {
            i++;
            ejer = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-N") || !strcmp(argv[i], "-n")) {
            i++;
            tam = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-M") || !strcmp(argv[i], "-m")) {
            i++;
            momento = atoi(argv[i]);
		} else if (!strcmp(argv[i], "-V") || !strcmp(argv[i], "-v")) {
            i++;
            printRN = 1;
		} else if (!strcmp(argv[i], "-P") || !strcmp(argv[i], "-p")) {
			i++;
			practicSelected = 3;
        } else {
			printHelp(argv[i]);
			return EXIT_FAILURE;
        }
        i++;
    }
	if (practicSelected)
        switch (ejer) {
            case 2:
                ejer2(tam, printRN);
                break;
            case 3:
                ejer3();
                break;
            default:
                ejer2(tam, printRN);
                break;
		}
	else {
		switch (ejer) {
		case 1:
			ejer1F(tam, momento, printRN);
			break;
		case 2:
			ejer2F(tam, momento, printRN);
			break;
		case 3:
			ejer2Fpruebita(tam, momento, 0);
			break;
		default:
			ejer3F(tam, momento, 0);
			break;
        }
    }
    return EXIT_SUCCESS;
}
