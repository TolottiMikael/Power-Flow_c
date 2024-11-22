#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#define RAD_TO_DEG 57.295779513082320876798154814105170332405472466564321549160243861
#define DEG_TO_RAD 0.0174532925199432957692369076848861271344287188854172545609719144

typedef struct {
    int partida;
    int destino;
    double impedancia;
    double reatancia;
    double power;
} circuitBar;

typedef struct {
    int numCircuitos;
    int *partida;
    int *destino;
} conexoesBar;

typedef struct {
    double ativa;
    double reativa;
} Carga;

typedef struct {
    double complex voltage;
    double real_Power;
    double reactive_Power;
    double complex apparent_Power;
    double delta;
    char isSlackbar;
    char doesHaveGenerator;
    Carga carga;
    double generatorRealPower;
    double generatorReactivePower;
} nodesValuesBar;

// Protótipos das funções
circuitBar getNullCircuit();
char isNullCircuit(circuitBar circuit);
circuitBar getCircuitoBetween(int circuito1, int circuito2, circuitBar *circuitos, int count);
int getMaiorBar(circuitBar *circuitos, int count);
conexoesBar getConexoesBetween(int circuito1, conexoesBar *conexoes);
void printCircuit(circuitBar circuit, char *string);
void printNodes(nodesValuesBar nodes, char *string);
void printNodesArray(nodesValuesBar *nodes1, nodesValuesBar *nodes2, int count, char *string);
void printConnection(conexoesBar conn, char *string);
int getExistingConnection(conexoesBar *conexoes, int partida, int destino);
void printConnAndCircuit(circuitBar *circuitos, conexoesBar *conexoes, int count);
void attNodeValues(nodesValuesBar *nodesDepois, nodesValuesBar *nodesAntes, int matrixRange, double complex Y[matrixRange][matrixRange], conexoesBar *conexoes, circuitBar *circuitos);


#include "circuit.c"

#endif
