#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

void allPrint(int data){
    int array[64];
    int rank, size;
    
    MPI_Comm_rank(MCW, &rank); 
    MPI_Comm_size(MCW, &size); 

    MPI_Gather(&data,1,MPI_INT,array,1,MPI_INT,0,MCW);
    if(!rank){
        for(int i=0;i<size;++i)cout<<i<<" ";
        cout<<endl;
        for(int i=0;i<size;++i)cout<<array[i]<<" ";
        cout<<endl;
    }
    return;
}

void cube(int f, int *data){
    int rank, size;
    int dest;
    int mask=1;
    mask <<= f;

    MPI_Comm_rank(MCW, &rank); 
    MPI_Comm_size(MCW, &size); 

    dest = rank ^ mask;

    MPI_Send(data,1,MPI_INT,dest,0,MCW);
    MPI_Recv(data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);

    return;
}



int main(int argc, char **argv){

    int rank, size;
    int data;
    int sum=0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank); 
    MPI_Comm_size(MCW, &size); 

    data = rank;
    sum = data;
    for(int i=0;i<3;++i){
        MPI_Barrier(MCW);
        cube(i,&data);
        sum+=data;
        data = sum;
    }

    cout << "rank "<<rank<<" has a sum of "<<sum<<endl;


    MPI_Finalize();

    return 0;
}

