#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define TAM_VET 12

int sum_pos(int* vetor_snd,int part)
{
    int sum = 0;
    for (int i = 0; i < part; i++)
    {
        sum += vetor_snd[i];
    }

    return sum;
}



int main(int argc, char *argv[])
{
    int size, rank;

    int vetor_snd[]={10,20,30,40,50,60,70,80,90,100,110,120};
    

    int sum_ind;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int vetor_rcv[TAM_VET/size];

    printf("Rank %d de um Total de %d\n", rank, size);


    if (rank==0)
    {
        printf("Ola sou o processo 0\n");

        int part = TAM_VET/size;
        int pos_vetor, rank_i;

        for (rank_i = 1; rank_i < size; rank_i++)
        {
            pos_vetor = rank_i * part;
            printf("SEND %d\t%d\t%d\n", rank_i,part,pos_vetor);
            MPI_Send(&vetor_snd[pos_vetor],part,MPI_INT,rank_i,123,MPI_COMM_WORLD);
        }


        part = TAM_VET/size;
        int sum = sum_pos(vetor_snd,part);

        
        for (int i = 1; i < size; ++i)
        {
            MPI_Recv(&sum_ind, 1, MPI_INT, i, 124, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += sum_ind;
        }

        printf("Resultado %d\n", sum);

    

    }else{
    
        int part = TAM_VET / size;
        MPI_Recv(vetor_rcv, part, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int sum = sum_pos(vetor_rcv,part);

        printf("Rank %d\t%d\n", rank, sum);

        MPI_Send(&sum, 1, MPI_INT, 0, 124, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}