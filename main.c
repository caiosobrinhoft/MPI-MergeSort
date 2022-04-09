#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <sys/time.h>

//Caio Sobrinho - Bacharelado Ciencia da Computação 021
//RA - 211044156 

#define N 2000000

void merge(int vetor[], int start, int mid, int end) {
    int n1 = start;
    int n2 = mid+1;
    int aux = 0, tam = end-start+1;
    int *vetAux;
    vetAux = (int*)malloc(tam * sizeof(int));

    while(n1 <= mid && n2 <= end){
        if(vetor[n1] < vetor[n2]) {
            vetAux[aux] = vetor[n1];
            n1++;
        } else {
            vetAux[aux] = vetor[n2];
            n2++;
        }
        aux++;
    }

    while(n1 <= mid){  //while the start of merge is less than mid
        vetAux[aux] = vetor[n1]; //the auxiliar array receives the value of "vetor" with start
        aux++; //increment aux and n1 until n<=mid value
        n1++;
    }

    while(n2 <= end) {   //while the mid value is less than the end value
        vetAux[aux] = vetor[n2]; //auxiliar array receives the array from the middle 
        aux++; //increment aux and n2 for the check of itens
        n2++;
    }

    for(aux = start; aux <= end; aux++){    //after the validation move the auxiliar array to the original array
        vetor[aux] = vetAux[aux-start];
    }
    
    free(vetAux);
}
void mergeSort(int vet[], int start, int end){ //mergeSort with the ordenation of function "merge"
    if (start < end) {
        int mid = (end+start)/2;

        mergeSort(vet, start, mid);
        mergeSort(vet, mid+1, end);

        merge(vet, start, mid, end); //
    }
}
void insertionSort(int vet[],int k){ //insertionSort that will be executed after the MPI Ordenation
    int valor, j;
    for(int i=1;i<k;i++){
        valor=vet[i];
        
        j=i;
        while(j>0 && vet[j-1]>valor){
            vet[j]=vet[j-1];
            j--;
        }
    vet[j]=valor;
    } 
}
int main(int argc, char **argv){ //use of MPI in function main
    struct timeval start, end;
    FILE *arq;
    FILE *arq2;
    int vet[N];

    arq = fopen("bigint.dat","r");

    for(int i=0; i<N;i++){
        fscanf(arq, "%d ", &vet[i]);
    }
    
    fclose(arq);

    MPI_Init(NULL, NULL); //Start MPI
    int world_size;
    int world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); //receive numbers of process 
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); //rank of process

    int indiceVet;
    indiceVet = N/world_size; // calculate value of n/k using MPI

    int vetInsert[indiceVet]; //sliced array
    int vetMerge[N]; //array that will be used for mergeSort

    gettimeofday(&start, NULL);

    MPI_Scatter(vet, indiceVet, MPI_INT, vetInsert, indiceVet, MPI_INT, 0, MPI_COMM_WORLD); //almost the same of MPI_Bcast
    //sends a segment of vet to all the process in COMM
    //vetdiv receives the segment
    //after the array split in sub-arrays, the program can execute insertionSort

    insertionSort(vetInsert, indiceVet); //sort the array
    MPI_Gather(vetInsert, indiceVet, MPI_INT, vetMerge, indiceVet, MPI_INT,0 , MPI_COMM_WORLD);
    
    if(world_rank==0){ //rank of process 0
    //mergeSort(0, N,vetMerge);
    mergeSort(vetMerge,0,N-1); //mergesort
    gettimeofday(&end, NULL); //contains the final time executed

    printf("\nTime Elapsed program: %lf\n", (double) ((end.tv_sec - start.tv_sec) + 1E-6 * (end.tv_usec - start.tv_usec))); //calculate the time elapsed of program

    arq2 = fopen("saida.dat","wt"); //open the exit archive

    for(int i=0; i<N;i++){
        fprintf(arq2, "%d \n", vetMerge[i]); //file print
    } 
    fclose(arq2);} //close the second archive
    
    MPI_Finalize(); //finalize the MPI
}