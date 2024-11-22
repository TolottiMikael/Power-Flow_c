#include <stdio.h>
#include "./circuit/circuit.h"
#include "./mikaelMath/MKTMath.h"

int main()
{
    printf("Este é um script em C que realiza a criação de uma tabela de Admitâncias\n");
    printf("Digite as impedâncias e reatâncias das linhas\n");
    printf("Separe as ligações de linhas com enter\n");
    printf("Digite FIM para encerrar\n");

    float Vbase = 230.0000000000000000;
    float Sbase = 100.0000000000000000;
    printf("Vbase: %f\n", Vbase);
    printf("Sbase: %f\n", Sbase);

    char linha[100];
    circuitBar *circuitos = NULL;
    int count = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;

    conexoesBar *conexoes = malloc(1 * sizeof(conexoesBar));
    conexoes->numCircuitos = 0;
    conexoes->partida = NULL;
    conexoes->destino = NULL;
    int numConex = 0;

    // string para coletar entrada do usuário
    char input[100];

    do
    {
        printf("Entrada: ");
        scanf("%99s", linha);

        circuitBar novoCircuito;
        // tentando verificar a exitência de um circuito
        if (sscanf(linha, "%d-%d:%lf;%lf;%lf",
                   &novoCircuito.partida, &novoCircuito.destino,
                   &novoCircuito.impedancia, &novoCircuito.reatancia,
                   &novoCircuito.power) != 5)
        {
            printf("Entrada não é um circuito\n");
        }
        else
        {

            novoCircuito.partida--;
            novoCircuito.destino--;

            // partida é sempre menor que destino
            if (novoCircuito.partida > novoCircuito.destino)
            {
                int aux = novoCircuito.partida;
                novoCircuito.partida = novoCircuito.destino;
                novoCircuito.destino = aux;
            }

            // verificando as conexões
            int conexIndex = getExistingConnection(conexoes, novoCircuito.partida, novoCircuito.destino);

            if (conexIndex == -1)
            {
                // adicionando uma conexão nova
                conexoes->numCircuitos++;
                conexoes->partida = realloc(conexoes->partida, conexoes->numCircuitos * sizeof(int));
                conexoes->destino = realloc(conexoes->destino, conexoes->numCircuitos * sizeof(int));
                conexoes->partida[conexoes->numCircuitos - 1] = novoCircuito.partida;
                conexoes->destino[conexoes->numCircuitos - 1] = novoCircuito.destino;

                circuitos = realloc(circuitos, (count + 1) * sizeof(circuitBar));
                circuitos[count++] = novoCircuito;
            }
            else
            {
                printf("este circuito já Existe!\n");
            }
        }
    } while (strcmp(linha, "FIM") != 0);

    printConnAndCircuit(circuitos, conexoes, count);

    printf("count : %d\n", count);
    int matrixRange = getMaiorBar(circuitos, count) + 1;
    printf("matrixRange : %d\n", matrixRange);
    // calculando admitâncias
    double complex y[matrixRange][matrixRange];
    double complex Y[matrixRange][matrixRange];

    for (i = 0; i < matrixRange; i++)
    {
        for (j = 0; j < matrixRange; j++)
        {
            if (i == j)
            {
                // diagonal principal
                y[i][j] = 0;
                conexoesBar conexoes_da_barra = getConexoesBetween(i, conexoes);
                int k = 0;
                for (k = 0; k < conexoes_da_barra.numCircuitos; k++)
                {
                    circuitBar circuito_ = getCircuitoBetween(conexoes_da_barra.partida[k], conexoes_da_barra.destino[k], circuitos, count);
                    y[i][j] += (circuito_.power / Sbase) / 2.00000;
                    // if (i == 0)
                    // {
                    //     printf("y0 = %lf \n ", y[i][j]);
                    //     printf("%lf\n", circuito_.power);
                    // }

                    y[i][j] += 1 / (circuito_.impedancia + (I * circuito_.reatancia));

                    // if (i == 0)
                    // {
                    //     printf("y0 = %lf \n ", y[i][j]);
                    //     printf("%lf\n",  );
                    // }
                }
                Y[i][j] = y[i][j];
            }
            else
            {
                circuitBar circuit = getCircuitoBetween(i, j, circuitos, count);
                if (isNullCircuit(circuit))
                {
                    y[i][j] = 0;
                }
                else
                {
                    y[i][j] = 1 / (circuit.impedancia + (I * circuit.reatancia));
                }
                Y[i][j] = -y[i][j];
            }
        }
    }

    printf("Matriz de Admitâncias y \n");
    printAdmittanceMatrix(matrixRange, y);

    printf("\n\nMatriz de Admitâncias y \n");
    printAdmittanceMatrix(matrixRange, Y);
    printf("\n");
    printf("\n");
    // calculando as tensoes e correntes
    nodesValuesBar nodes[2][matrixRange];
    unsigned int interaction = 0;
    double complex erro[matrixRange];

    // não utilizado mais dessa forma!
    for (i = 0; i < matrixRange; i++)
    {
        for (j = 0; j < matrixRange; j++)
        {
            printf("%lf <%lf\t\t", cabs(Y[i][j]), carg(Y[i][j]) * RAD_TO_DEG);

            // utilizado mais a frente
            nodes[0][i].voltage = 1;
            nodes[0][i].real_Power = 1;
            nodes[0][i].reactive_Power = 1;
            nodes[0][i].apparent_Power = 1;
            nodes[0][i].delta = 0;

            nodes[1][i].voltage = 1;
            nodes[1][i].real_Power = 1;
            nodes[1][i].reactive_Power = 1;
            nodes[1][i].apparent_Power = 1;
            nodes[1][i].delta = 0;

            nodes[0][i].isSlackbar = 0;
            nodes[0][i].doesHaveGenerator = 0;

            nodes[1][i].isSlackbar = 0;
            nodes[1][i].doesHaveGenerator = 0;

            erro[i] = 0;
        }
        printf("\n");
    }

    printf("para Prosseguir você deve indicar informações importantes\n");
    printf("\n\n <==> \n\n");
    printf("indique qual barra é slack\n");
    int inputs = 1;
    int slackBar = -1;
    int generator = 0;
    scanf("%d", &inputs);
    for (i = 0; i < 2; i++)
    {
        nodes[i][inputs - 1].isSlackbar = 1;
        nodes[i][inputs - 1].doesHaveGenerator = 1;
        slackBar = inputs - 1;
    }
    printf("barra %d  \n", nodes[1][0].isSlackbar);
    printf("barra %d  \n", nodes[1][1].isSlackbar);

    double value;
    for (i = 0; i < matrixRange; i++)
    {
        printf("qual a carga ativa na barra %d\n", i + 1);
        scanf("%lf", &value);
        nodes[0][i].carga.ativa = value;
        nodes[1][i].carga.ativa = value;

        printf("qual a carga reativa na barra %d\n", i + 1);
        scanf("%lf", &value);
        nodes[0][i].carga.reativa = value;
        nodes[1][i].carga.reativa = value;

        if (nodes[0][i].isSlackbar != 1)
        {
            printf("qual a geração em MW na barra (digite 0 se não ter gerador) %d\n", i + 1);
            scanf("%lf", &value);
            nodes[0][i].generatorRealPower = value;
            nodes[1][i].generatorRealPower = value;

            printf("qual a geração em MVar na barra (digite 0 se não ter gerador) %d\n", i + 1);
            scanf("%lf", &value);
            nodes[0][i].generatorReactivePower = value;
            nodes[1][i].generatorReactivePower = value;
            if (value == 0)
            {
                nodes[0][i].doesHaveGenerator = 0;
                nodes[1][i].doesHaveGenerator = 0;
            }
            else
            {
                nodes[0][i].doesHaveGenerator = 1;
                nodes[1][i].doesHaveGenerator = 1;

                generator = i;
            }
        }
    }

    char string[1000] = "";
    // printNodesArray(nodes[0], nodes[1], matrixRange, string);
    // printf("%s\n", string);
    double deltaP[matrixRange];
    double deltaQ[matrixRange];

    int MAXInteration = 4;

    do
    {

        system("cls");
        interaction++;
        attNodeValues(nodes[1], nodes[0], matrixRange, Y, conexoes, circuitos);
        printf(" <==================> \n nós após a interação\n");

        // strcpy(string, "");
        // printNodesArray(nodes[0], nodes[1], matrixRange, string);
        // printf("%s\n", string);

        // calculando os resíduos

        for (i = 0; i < matrixRange; i++)
        {
            deltaP[i] = ((nodes[1][i].generatorRealPower - nodes[1][i].carga.ativa) / Sbase) - nodes[1][i].real_Power;
            deltaQ[i] = ((nodes[1][i].generatorReactivePower - nodes[1][i].carga.reativa) / Sbase) - nodes[1][i].reactive_Power;
        }

        double complex H_[matrixRange][matrixRange];
        double complex J_[matrixRange][matrixRange];
        double complex N_[matrixRange][matrixRange];
        double complex L_[matrixRange][matrixRange];

        for (i = 0; i < matrixRange; i++)
        {
            for (j = 0; j < matrixRange; j++)
            {
                if (i == j)
                {
                    // diagonal principal
                    H_[i][j] = -(nodes[1][i].reactive_Power) -
                               (pow(cabs(nodes[1][i].voltage), 2) * cimag(Y[i][j]));
                    // restrições para N
                    N_[i][j] = nodes[1][i].real_Power +
                               (pow(cabs(nodes[1][i].voltage), 2)) * creal(Y[i][j]);

                    J_[i][j] = nodes[1][i].real_Power -
                               (pow(cabs(nodes[1][i].voltage), 2)) * creal(Y[i][j]);
                    L_[i][j] = nodes[1][i].reactive_Power -
                               (pow(cabs(nodes[1][i].voltage), 2) * cimag(Y[i][j]));
                }
                else
                {
                    // diagonal não principal
                    H_[i][j] = -((cabs(nodes[1][i].voltage) * cabs(nodes[1][j].voltage) * cabs(Y[i][j])) * sin(((carg(Y[i][j]) * RAD_TO_DEG) + nodes[1][j].delta - nodes[1][i].delta) * DEG_TO_RAD));
                    N_[i][j] = cabs(cabs(nodes[1][i].voltage) * cabs(nodes[1][j].voltage) * cabs(Y[i][j])) * cos(((carg(Y[i][j]) * RAD_TO_DEG) - nodes[1][j].delta + nodes[1][i].delta) * DEG_TO_RAD);
                    J_[i][j] = -cabs(cabs(nodes[1][i].voltage) * cabs(nodes[1][j].voltage) * cabs(Y[i][j])) * cos(((carg(Y[i][j]) * RAD_TO_DEG) - nodes[1][j].delta + nodes[1][i].delta) * DEG_TO_RAD);
                    L_[i][j] = -cabs(cabs(nodes[1][i].voltage) * cabs(nodes[1][j].voltage) * cabs(Y[i][j])) *
                               sin(((carg(Y[i][j]) * RAD_TO_DEG) + nodes[1][j].delta - nodes[1][i].delta) * DEG_TO_RAD);
                }

                // printf("H[%d][%d] = %lf \t", i - countSlackBarROW, j - countSlackBarCOL, H[i - countSlackBarROW][j - countSlackBarCOL])
            }
        }
        /*
            até aqui está tudo calculado corretamente, porém a Jacobiana não inclui a barra slack
            Logo preciso cortar todas linhas e colunas que possuem a barra slack
        */
        double complex H[matrixRange - 1][matrixRange - 1];
        double complex J[matrixRange - 1][matrixRange - 1];
        double complex N[matrixRange - 1][matrixRange - 1];
        double complex L[matrixRange - 1][matrixRange - 1];

        double complex H__[matrixRange - 2][matrixRange - 2];
        double complex J__[matrixRange - 2][matrixRange - 2];
        double complex N__[matrixRange - 2][matrixRange - 2];
        double complex L__[matrixRange - 2][matrixRange - 2];

        cutMatrix(matrixRange, matrixRange, H_, slackBar, slackBar, H);
        cutMatrix(matrixRange, matrixRange, J_, slackBar, slackBar, J__);
        cutMatrix(matrixRange, matrixRange, N_, slackBar, slackBar, N__);
        cutMatrix(matrixRange, matrixRange, L_, slackBar, slackBar, L__);

        cutRow(matrixRange - 1, matrixRange - 1, J__, generator, J);
        cutColumn(matrixRange - 1, matrixRange - 1, N__, generator, N);
        cutMatrix(matrixRange - 1, matrixRange - 1, L__, generator, generator, L);
        // rever esta parte abaixo para ser genérico

        system("cls");

        int rangeH = matrixRange - 1;
        int m = 0;

        double complex Jacobiana[matrixRange + 1][matrixRange + 1];

        // Inicialização manual dos elementos da matriz Jacobiana
        // for (i = 0; i < rangeH; i++)
        // {
        //     for (j = 0; j < rangeH; j++)
        //     {
        //         Jacobiana[i][j] = H[i][j];
        //     }
        // }

        // for (i = 0; i < rangeH; i++)
        // {
        //     for (j = 0; j < (rangeH - 1); j++)
        //     {
        //         Jacobiana[i][j + rangeH] = N[i][j];
        //     }
        // }

        Jacobiana[0][0] = H[0][0];
        Jacobiana[0][1] = H[0][1];
        Jacobiana[0][2] = H[0][2];
        Jacobiana[0][3] = N__[0][0]; // Correto
        Jacobiana[0][4] = N__[0][2]; // Corrigido: era N[0][1], deveria ser N[0][2]

        Jacobiana[1][0] = H[1][0];
        Jacobiana[1][1] = H[1][1];
        Jacobiana[1][2] = H[1][2];
        Jacobiana[1][3] = N__[1][0]; // Correto
        Jacobiana[1][4] = N__[1][2]; // Corrigido: era N[1][1], deveria ser N[1][2]

        Jacobiana[2][0] = H[2][0];
        Jacobiana[2][1] = H[2][1];
        Jacobiana[2][2] = H[2][2];
        Jacobiana[2][3] = N__[2][0]; // Correto
        Jacobiana[2][4] = N__[2][2]; // Corrigido: era N[2][1], deveria ser N[2][2]

        Jacobiana[3][0] = J__[0][0]; // Correto
        Jacobiana[3][1] = J__[0][1]; // Correto
        Jacobiana[3][2] = J__[0][2]; // Correto
        Jacobiana[3][3] = L__[0][0]; // Corrigido: para corresponder ao L(1,1) em MATLAB
        Jacobiana[3][4] = L__[0][2]; // Corrigido: para corresponder ao L(1,3) em MATLAB

        Jacobiana[4][0] = J__[2][0]; // Corrigido: era J[1][0], deveria ser J[2][0] para corresponder ao MATLAB
        Jacobiana[4][1] = J__[2][1]; // Corrigido: era J[1][1], deveria ser J[2][1] para corresponder ao MATLAB
        Jacobiana[4][2] = J__[2][2]; // Corrigido: era J[1][2], deveria ser J[2][2] para corresponder ao MATLAB
        Jacobiana[4][3] = L__[2][0]; // Corrigido: para corresponder ao L(3,1) em MATLAB
        Jacobiana[4][4] = L__[2][2]; // Corrigido: para corresponder ao L(3,3) em MATLAB

        // printf("L[0][0]: %f + %fi\n", creal(N[0][0]), cimag(N[0][0])); // Exemplo se for um número complexo
        // printf("L[0][1]: %f + %fi\n", creal(N[0][1]), cimag(N[0][1]));
        // printf("Jacobiana[3][0]: %f + %fi\n", creal(Jacobiana[3][0]), cimag(Jacobiana[3][0]));
        // printf("Jacobiana[3][1]: %f + %fi\n", creal(Jacobiana[3][1]), cimag(Jacobiana[3][1]));

        // for (i = 0; i < rangeH; i++)
        // {
        //     for (j = 0; j < rangeH; j++)
        //     {
        //         // printf("(%d,%d)\t", i, j);
        //         // printf("(%d,%d) salvando H[%d][%d]\t", i, j, i, j );
        //         printf("(%d:%d) %lf + %lfi\t",i,j, creal(H_[i][j]), cimag(H_[i][j]));
        //         Jacobiana[i][j] = H_[i][j];
        //     }
        //     // printf("agora N\n");
        //     for (m = rangeH; m < matrixRange + 1; m++)
        //     {
        //         // printf("(%d,%d)\t", i, m);
        //         // printf("(%d,%d) salvando N[%d][%d]\t", i, m, i, m- rangeH );
        //         printf("(%d:%d) %lf + %lfi\t",i, m- rangeH, creal(N_[i][m - rangeH]), cimag(N_[i][m - rangeH]));
        //         Jacobiana[i][m] = N_[i][m - rangeH];
        //     }
        //     printf("\n");
        // }
        // for (i = rangeH; i <= matrixRange; i++)
        // {
        //     for (j = 0; j < rangeH; j++)
        //     {
        //         // printf("(%d,%d)\t", i, j);
        //         // printf("(%d,%d) salvando J[%d][%d]\t", i, j, i-rangeH, j );
        //         printf("(%d:%d) %lf + %lfi\t",i- rangeH, j, creal(J_[i - rangeH][j]), cimag(J_[i - rangeH][j]));
        //         Jacobiana[i][j] = J_[i - rangeH][j];
        //     }
        //     // printf("agroa l\n");
        //     for (m = rangeH; m <= matrixRange; m++)
        //     {
        //         // printf("(%d,%d) salvando L[%d][%d]\t", i, m, i- rangeH, m - rangeH);
        //         printf("(%d:%d) %lf + %lfi\t",i- rangeH, m - rangeH, creal(L_[i - rangeH][m - rangeH]), cimag(L_[i - rangeH][m - rangeH]));
        //         Jacobiana[i][m] = L_[i - rangeH][m - rangeH];
        //     }
        //     printf("\n");
        // }

        // system("cls");
        // printf("matriz H \n");
        // showMatrix(matrixRange -1 , matrixRange- 1, H);
        // printf("matriz N \n");
        // showMatrix(matrixRange- 1, matrixRange- 1, N);
        // printf("matriz J \n");
        // showMatrix(matrixRange- 1, matrixRange- 1, J);
        // printf("matriz L \n");
        // showMatrix(matrixRange- 1, matrixRange- 1, L);
        printf("Jacobiana\n");
        showMatrix(matrixRange + 1, matrixRange + 1, Jacobiana);

        double complex inv_Jacobiana[matrixRange + 1][matrixRange + 1];
        getInverse_matrix(matrixRange + 1, Jacobiana, inv_Jacobiana);
        // printf("\nJacobiana Inversa\n");
        // showMatrix(matrixRange + 1, matrixRange + 1, inv_Jacobiana);

        double complex residuos[matrixRange + 1][1];
        double complex final[matrixRange + 1][1];

        residuos[0][0] = deltaP[1];
        residuos[1][0] = deltaP[2];
        residuos[2][0] = deltaP[3];
        residuos[3][0] = deltaQ[1];
        residuos[4][0] = deltaQ[2];

        // printf("\nResiduos\n");
        // showMatrix(matrixRange + 1, 1, residuos);

        multiplyMatrices(matrixRange + 1, matrixRange + 1, inv_Jacobiana, matrixRange + 1, 1, residuos, final);

        printf("\nFinal\n");
        showMatrix(matrixRange + 1, 1, final);

        system("pause");
        // d2 = final(1);
        // d3 = final(2);
        // d4 = final(3);

        // deltaV2 = final(4);
        // deltaV3 = final(5);

        // V(2) = abs(V(2))* ( 1 + (deltaV2/abs(V(2))) );
        // V(3) = abs(V(3))* ( 1 + (deltaV3/abs(V(3))) );

        // delta(2) = delta(2) + d2;
        // delta(3) = delta(3) + d3;
        // delta(4) = delta(4) + d4;

        nodes[0][1].voltage = cabs(nodes[1][1].voltage) * (1 + (final[1][3] / cabs(nodes[1][1].voltage)));
        nodes[0][2].voltage = cabs(nodes[1][2].voltage) * (1 + (final[1][4] / cabs(nodes[1][2].voltage)));

        nodes[0][1].delta = nodes[1][1].delta + final[1][0];
        nodes[0][2].delta = nodes[1][2].delta + final[1][1];
        nodes[0][3].delta = nodes[1][3].delta + final[1][2];
    } while (interaction < MAXInteration);

    strcpy(string, "");
    printNodesArray(nodes[0], nodes[1], matrixRange, string);
    printf("%s\n", string);

    // desalocando memória
    free(conexoes->partida);
    free(conexoes->destino);
    free(conexoes);
    free(circuitos);
}