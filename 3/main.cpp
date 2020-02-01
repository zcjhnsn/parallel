#include <iostream>
#include <cstdlib>
#include <ctime>
//#include <mpi.h>
#include "/usr/local/include/mpi.h"

#define MCW MPI_COMM_WORLD
#define ROOT 0

using namespace std;

void merge(int *, int *, int, int, int);
void mergeSort(int *, int *, int, int);

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

    // Sort subArrays
    int *tempArray = new int[subSize];

    cout << "Rank " << rank << " sorting subarray: ";

    for (int i = 0; i < subSize; i++) {
        cout << subArray[i] << " ";
    }
    cout << endl;

    mergeSort(subArray, tempArray, 0, (subSize - 1));

    // Gather sorted subarrays
    int *sorted = NULL;

    if(rank == 0) {
        sorted = new int[n];
    }

    MPI_Gather(subArray, subSize, MPI_INT, sorted, subSize, MPI_INT, ROOT, MCW);

    // Merge sorted sub-arrays at ROOT
    if (rank == ROOT) {
        int *final_array = new int[n];
        mergeSort(sorted, final_array, 0, (n - 1));

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
    delete[] tempArray;

    MPI_Barrier(MCW);
    MPI_Finalize();

}

// Merge two arrays
void merge(int *a, int *b, int l, int m, int r) {
    int i, j, k, n;
    i = l;
    j = l;
    k = m + 1;

    while ((i <= m) && (k <= r)) {
        if(a[i] <= a[k]) {
            b[j] = a[i];
            i++;
        }

        else {
            b[j] = a[k];
            k++;
        }

        j++;
    }

    if (m < i) {
        for (n = k; n <= r; n++) {
            b[j] = a[n];
            j++;
        }
    }

    else {
        for (n = i; n <= m; n++) {
            b[j] = a[n];
            j++;
        }
    }

    for (n = l; n <= r; n++) {
        a[n] = b[n];
    }

}

// Merge sort arrays recursively
void mergeSort(int *a, int *b, int l, int r) {
    int m;

    if (l < r) {
        m = (l + r)/2;

        mergeSort(a, b, l, m);
        mergeSort(a, b, (m + 1), r);
        merge(a, b, l, m, r);
    }
}