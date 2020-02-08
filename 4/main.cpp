#include <iostream>
#include <cstdlib>
#include <ctime>
//#include <mpi.h>
#include "/usr/local/include/mpi.h"

#define MCW MPI_COMM_WORLD
#define ROOT 0

using namespace std;

// NOTE: bitonic algorithm from geeksforgeeks.org

/*The parameter dir indicates the sorting direction, ASCENDING
   or DESCENDING; if (a[i] > a[j]) agrees with the direction,
   then a[i] and a[j] are interchanged.*/
void compAndSwap(int a[], int i, int j, int dir)
{
    if (dir==(a[i]>a[j]))
        swap(a[i],a[j]);
}

/*It recursively sorts a bitonic sequence in ascending order,
  if dir = 1, and in descending order otherwise (means dir=0).
  The sequence to be sorted starts at index position low,
  the parameter cnt is the number of elements to be sorted.*/
void bitonicMerge(int a[], int low, int cnt, int dir)
{
    if (cnt>1)
    {
        int k = cnt/2;
        for (int i=low; i<low+k; i++)
            compAndSwap(a, i, i+k, dir);
        bitonicMerge(a, low, k, dir);
        bitonicMerge(a, low+k, k, dir);
    }
}

/* This function first produces a bitonic sequence by recursively
    sorting its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order */
void bitonicSort(int a[],int low, int cnt, int dir)
{
    if (cnt>1)
    {
        int k = cnt/2;

        // sort in ascending order since dir here is 1
        bitonicSort(a, low, k, 1);

        // sort in descending order since dir here is 0
        bitonicSort(a, low+k, k, 0);

        if (cnt == 16) {
            cout << "Bitonic: ";
            for (int i = 0; i < cnt; i++) {
                cout << a[i] << " ";
            }
            cout << endl;
        }

        // Will merge wole sequence in ascending order
        // since dir=1.
        bitonicMerge(a,low, cnt, dir);
    }
}

int main(int argc, char** argv) {
    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    // Create unsorted array
    int n = 16;
    int *unsortedArray = new int[n];

    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        unsortedArray[i] = rand() % 100;
    }

    if (rank == ROOT) {
        cout << "Unsorted: ";
        for (int i = 0; i < n; i++) {
            cout << unsortedArray[i] << " ";
        }
        cout << endl;
    }

    // Calculate subarray size
    int subSize = n / size;
    int *subArray = new int[subSize];

    // Send sub-arrays to processes
    MPI_Scatter(unsortedArray, subSize, MPI_INT, subArray, subSize, MPI_INT, ROOT, MCW);

    if (rank < size/2) {
        bitonicSort(subArray, 0, subSize, 1);
        cout << "Rank " << rank << " sorted ascending: ";

        for (int i = 0; i < subSize; i++) {
            cout << subArray[i] << " ";
        }
        cout << endl;
    } else {
        bitonicSort(subArray, 0, subSize, 0);
        cout << "Rank " << rank << " sorted descending: ";

        for (int i = 0; i < subSize; i++) {
            cout << subArray[i] << " ";
        }
        cout << endl;
    }

    // Gather sorted subarrays
    int *sorted = NULL;

    if(rank == 0) {
        sorted = new int[n];
    }

    MPI_Gather(subArray, subSize, MPI_INT, sorted, subSize, MPI_INT, ROOT, MCW);

    // Merge sorted sub-arrays at ROOT
    if (rank == ROOT) {

        int *final_array = new int[n];
        bitonicSort(sorted,0,n,1);

        cout << "Sorted: ";
        for (int i = 0; i < n; i++) {
            cout << sorted[i] << " ";
        }
        cout << endl;

        // Clean memory
        delete[] sorted;
        delete[] final_array;
    }

    // Clean up memory
    delete[] unsortedArray;
    delete[] subArray;

    MPI_Barrier(MCW);
    MPI_Finalize();

}
