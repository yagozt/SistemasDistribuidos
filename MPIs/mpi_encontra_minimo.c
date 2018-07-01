/****************************************************************************/
/*                                                                          */
/* Nome: Yago Henrique                                                      */
/* Exercicio: Encontra minimo em vetor com MPI                              */
/* Descricao:                                                               */
/*                                                                          */
/* Compilar: mpicc mpi_encontra_minimo.c -o mpi_encontra_minimo -Wall       */
/* Executar: mpirun -np 2 mpi_encontra_minimo                               */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define TAM_VET 1000
#define EPSILON 0.00001

int main(int argc, char *argv[])
{
	int size,rank;
    srand(time(NULL));
    float vetor_rcv[TAM_VET];
    float local_min = 10000;
    float global_min = 10000;
    int totalInteracoes = 0;
    int i, quantidadeInteracoes = 0;
    MPI_Init(&argc,&argv);

    // Preenchendo Vetor
    for(i = 0;i<TAM_VET;i++)
    {
        float r = (rand() % 100)+1;
        vetor_rcv[i] = r;
    }

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);


    int part = TAM_VET/size;
	
    int init = part * rank;
  
    do {
        /* Distribuindo o vetor em varias partes para cada processo */
        /* Cada um multiplicando o vetor local por 0.9 */
        for (i = init; i < (init+part); i++) {
            quantidadeInteracoes += 1;
            vetor_rcv[i] *= 0.9;
            if(vetor_rcv[i] <= local_min)
            {
                local_min = vetor_rcv[i];
                if(local_min <= EPSILON)
                {
                    break;
                }
            }
            
        }

        /* Compartilhando o menor valor local com todos os processos */
        MPI_Allreduce(&local_min, &global_min, 1, MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD); 

    }
    while(global_min >= EPSILON); //CONDIÇÃO DE PARADA (global_min <= epsilon).    

    MPI_Reduce(&quantidadeInteracoes, &totalInteracoes, 1, MPI_INT, MPI_SUM,0, MPI_COMM_WORLD);


    if (rank == 0) {
        printf("Minimo global = %f, \nTotal de Interações = %d \n", global_min, totalInteracoes);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return(0);
}	


