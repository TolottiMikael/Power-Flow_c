#include "circuit.h"

#define RAD_TO_DEG 57.295779513082320876798154814105170332405472466564321549160243861 // valor mais preciso para conversão radiano-graus
#define DEG_TO_RAD 0.0174532925199432957692369076848861271344287188854172545609719144



// retorna um circuito NULO
circuitBar getNullCircuit()
{
    circuitBar nullCircuit = {-1, -1, 0, 0, 0};
    return nullCircuit;
}
// função para saber se o circuito é nulo
char isNullCircuit(circuitBar circuit)
{
    circuitBar nullCircuit = getNullCircuit();
    if (
        circuit.partida == nullCircuit.partida &&
        circuit.destino == nullCircuit.destino &&
        circuit.impedancia == nullCircuit.impedancia &&
        circuit.reatancia == nullCircuit.reatancia)
    {
        return 1;
    }
    return 0;
}

// função que retorna o circuito entre os dois
circuitBar getCircuitoBetween(int circuito1, int circuito2, circuitBar *circuitos, int count)
{
    int i = 0;
    int j = 0;
    int circuitoMaior = 0;
    int circuitoMenor = 0;

    if (circuito1 > circuito2)
    {
        circuitoMaior = circuito1;
        circuitoMenor = circuito2;
    }
    else
    {
        circuitoMaior = circuito2;
        circuitoMenor = circuito1;
    }

    for (i = 0; i < count; i++)
    {
        for (j = 0; j < count; j++)
        {
            // fazer os testes
            if (circuitos[i].partida == circuitoMenor && circuitos[i].destino == circuitoMaior)
            {
                return circuitos[i];
            }
        }
    }

    return getNullCircuit();
}

//  função que retorna a maior barra
int getMaiorBar(circuitBar *circuitos, int count)
{
    int maior = 0;
    int i = 0;
    int j = 0;

    for (i = 0; i < count; i++)
    {
        for (j = 0; j < count; j++)
        {
            // fazer os testes
            if (circuitos[i].partida > maior)
            {
                maior = circuitos[i].partida;
            }
            else if (circuitos[i].destino > maior)
            {
                maior = circuitos[i].destino;
            }
        }
    }
    return maior;
}

// função que retorna quais conexões existem entre dois circuitos
conexoesBar getConexoesBetween(int circuito1, conexoesBar *conexoes)
{
    int i = 0;
    conexoesBar conexoesDaBarra;

    conexoesDaBarra.numCircuitos = 0;
    conexoesDaBarra.partida = NULL;
    conexoesDaBarra.destino = NULL;

    for (i = 0; i < conexoes->numCircuitos; i++)
    {
        if (
            (conexoes->partida[i] == circuito1 || conexoes->destino[i] == circuito1))
        {
            conexoesDaBarra.numCircuitos++;
            conexoesDaBarra.partida = realloc(conexoesDaBarra.partida, conexoesDaBarra.numCircuitos * sizeof(int));
            conexoesDaBarra.destino = realloc(conexoesDaBarra.destino, conexoesDaBarra.numCircuitos * sizeof(int));

            conexoesDaBarra.partida[conexoesDaBarra.numCircuitos - 1] = conexoes->partida[i];
            conexoesDaBarra.destino[conexoesDaBarra.numCircuitos - 1] = conexoes->destino[i];
        }
    }
    return conexoesDaBarra;
}

// função que imprime o circuito
void printCircuit(circuitBar circuit, char *string)
{
    sprintf(string, "(%d-%d) %f %f %f", circuit.partida, circuit.destino, circuit.impedancia, circuit.reatancia, circuit.power);
}

void printNodes(nodesValuesBar nodes, char *string)
{
    sprintf(string, "V: %lf +i%lf \n P:%lf \n Q:%lf \n S:%lf +i%lf \n Delta:%lf \n",
            creal(nodes.voltage), cimag(nodes.voltage),
            (nodes.real_Power),
            (nodes.reactive_Power),
            creal(nodes.apparent_Power),
            nodes.delta);
}

void printNodesArray(nodesValuesBar *nodes1, nodesValuesBar *nodes2, int count, char *string)
{
    printf("realizando um print de %d nós\n", count);
    int i = 0;
    char addString[100];
    for (i = 0; i < count; i++)
    {
        sprintf(addString, "<--- NÓ %d --->\n", i);
        strcat(string, addString);
        strcat(string, "NODES[1]\n");
        printNodes(nodes1[i], addString);
        strcat(string, addString);
        strcat(string, "NODES[2]\n");
        printNodes(nodes2[i], addString);
        strcat(string, addString);
        strcat(string, "<------------------------>\n");
    }
}
void printConnection(conexoesBar conn, char *string)
{
    char addString[100];
    int i = 0;

    sprintf(string, "%d Conexoes\n", conn.numCircuitos);

    for (i = 0; i < conn.numCircuitos; i++)
    {
        sprintf(addString, "%d: (%d <-> %d)\n", i, conn.partida[i], conn.destino[i]);
        strcat(string, addString);
    }
}

int getExistingConnection(
    conexoesBar *conexoes,
    int partida,
    int destino)
{
    int i = 0;
    for (i = 0; i < conexoes->numCircuitos; i++)
    {
        if (conexoes->partida[i] == partida && conexoes->destino[i] == destino)
        {
            return i;
        }
    }
    return -1;
}

void printConnAndCircuit(circuitBar *circuitos, conexoesBar *conexoes, int count)
{
    char string[100];
    printf("Circuitos \n");
    for (int i = 0; i < count; i++)
    {
        printCircuit(circuitos[i], string);
        printf("%s\n", string);
    }

    printConnection(*conexoes, string);
    printf("%s\n", string);
}

// funções de cálculo do newton-raphson
void attNodeValues(
    nodesValuesBar *nodesDepois,
    nodesValuesBar *nodesAntes,
    int matrixRange,
    double complex Y[matrixRange][matrixRange],
    conexoesBar *conexoes,
    circuitBar *circuitos)
{
    double complex deltaP[matrixRange];
    double complex deltaQ[matrixRange];

    int i = 0;
    for (i = 0; i < matrixRange; i++)
    {
        // printf(" calculando para a barra %d\n", i);
        nodesDepois[i]
            .real_Power =
            (pow((nodesAntes[i].voltage), 2) * creal(Y[i][i]));

        nodesDepois[i]
            .reactive_Power =
            ((pow((nodesAntes[i].voltage), 2) * cimag(Y[i][i])) * -1);

        // printf(" por enquanto : realP: %lf\n", nodesDepois[i].real_Power);
        // printf(" por enquanto : imagQ: %lf\n", nodesDepois[i].reactive_Power);

        for (int j = 0; j < matrixRange; j++)
        {
            if (i != j)
            {
                int menorIndex = 0;
                int maiorIndex = 0;
                if (j > i)
                {
                    maiorIndex = j;
                    menorIndex = i;
                }
                else
                {
                    maiorIndex = i;
                    menorIndex = j;
                }
                // index do array de conexoes que este circuito se encontra
                int index = getExistingConnection(conexoes, menorIndex, maiorIndex);
                if (index != -1)
                {
                    circuitBar bar = circuitos[index];

                    // char text[100];
                    // printCircuit(bar, text);
                    // printf("%s\n", text);
                    // printf("achei o circuito %d (%d <-> %d)\n", index, bar.partida, bar.destino);

                    nodesDepois[i]
                        .real_Power += (cabs(nodesAntes[i].voltage * nodesAntes[j].voltage) * cabs(Y[i][j])) *
                                       cos(((carg(Y[i][j]) * RAD_TO_DEG) + nodesAntes[j].delta - nodesAntes[i].delta) * DEG_TO_RAD);

                    nodesDepois[i]
                        .reactive_Power -= (cabs(nodesAntes[i].voltage * nodesAntes[j].voltage) * cabs(Y[i][j])) *
                                           sin(((carg(Y[i][j]) * RAD_TO_DEG) + nodesAntes[j].delta - nodesAntes[i].delta) * DEG_TO_RAD);
                    // printf(
                    //     "P-= |%lfx%lfx%lf|xsin( %lf + %lf - %lf ) = %lf\n",
                    //     cabs(nodesAntes[i].voltage), cabs(nodesAntes[j].voltage), cabs(Y[i][j]),
                    //     (carg(Y[i][j]) * RAD_TO_DEG), nodesAntes[j].delta, nodesAntes[i].delta,
                    //     ((cabs(nodesAntes[i].voltage * nodesAntes[j].voltage) * cabs(Y[i][j])) *
                    //      //  cos(((carg(Y[i][j]) * RAD_TO_DEG) + nodesAntes[j].delta - nodesAntes[i].delta) * DEG_TO_RAD)));
                    //      cos(((carg(Y[i][j]) * 1) + nodesAntes[j].delta - nodesAntes[i].delta) * 1)));

                    // printf("\n \n");
                }
            }
        }
        // printf("P[%d]= %lf\n \n\n", i, nodesDepois[i].real_Power);
        // printf("Q[%d]= %lf\n \n\n", i, nodesDepois[i].reactive_Power);
    }
}

