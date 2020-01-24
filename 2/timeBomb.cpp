#include <iostream>
//#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "/usr/local/include/mpi.h"
#define MCW MPI_COMM_WORLD

using namespace std;

int main(int argc, char **argv) {

    int rank, size;
    int timer;
    int dest;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);  // unique number 0 to size-1
    MPI_Comm_size(MCW, &size);  // number of processes
    srand(time(nullptr));       // seed random so it's different every time
    
    if (rank==3) {
        timer = rand()%20;      // randomly set timer length
        cout << "The bomb has been planted with a " << timer << " second timer." << endl;

        for(int i=0; i < size; ++i)cout<<" "<<i<<" ";
        cout<<endl;

        MPI_Send(&timer,1,MPI_INT,3,0,MCW);
    }

    while(1) {
        // Wait to receive timer
        MPI_Recv(&timer,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);

        if(timer<0)break;
        sleep(1);
        // print location of the bomb with time value
        for (int i = 0; i < rank; ++i) cout << "   ";
        cout << " " << timer << " " <<endl;
        if (timer == 0) {
            cout << "Rank " << rank << " tried to defuse the bomb and did not succeed. F." << endl << endl;
        }

        // End and send the poison pill to everyone
        if(timer == 0){
            timer=-1;
            for(int i=0;i<size;++i) MPI_Send(&timer,1,MPI_INT,i,0,MCW);
        }

        timer--;                // decrement timer
        dest = rand() % size;   // randomly select next destination

        MPI_Send(&timer,1,MPI_INT,dest,0,MCW);
    }

    cout<<"rank "<<rank<<" left the game.\n";

    MPI_Finalize();

    return 0;
}
