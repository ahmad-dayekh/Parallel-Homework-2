#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define TERMINATOR -1
/* Note: Number of processes should be sqrt(MAX_NUMBER) or greater */
#define MAX_NUMBER 10000 // Find primes up to this number

void generate_numbers(int rank, int size) {
    int dest = 1;
    for (int num = 2; num <= MAX_NUMBER; num++) {
        MPI_Send(&num, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }
    int terminator = TERMINATOR;
    MPI_Send(&terminator, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
}

void filter_numbers(int rank, int size) {
    MPI_Status status;
    int number;
    bool first_number = true;
    int my_prime = 0;

    while (1) {
        MPI_Recv(&number, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);

        if (number == TERMINATOR) {
            if (rank < size - 1) {
                MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
            }
            break;
        }

        if (first_number && number != TERMINATOR && my_prime == 0) {
            my_prime = number;
            printf("Process %d assigned prime: %d\n", rank, my_prime);
            printf("%d ", my_prime);
            first_number = false;
            continue;
        }

        // Only passing numbers that arent multiples of our prime
        if (!first_number && number % my_prime != 0) {
            if (rank < size - 1) {
                MPI_Send(&number, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
            } else {
                printf("%d ", number);
            }
        }
    }
}

int main(int argc, char** argv) {
    double start_time = MPI_Wtime();

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("This program requires at least 2 processes\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        fflush(stdout);
        generate_numbers(rank, size);
    } else {
        filter_numbers(rank, size);
    }


    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    double total_time;
    MPI_Reduce(&elapsed_time, &total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total elapsed time: %f seconds\n", total_time);
    }

    MPI_Finalize();
    return 0;
}

/* Explanation:
suppose p = 3:
    Process 0 is the generator (doesn't get assigned a prime)
    Process 1 gets prime 2 and filters out even numbers
    Process 2 gets prime 3 and filters out multiples of 3
The remaining numbers that make it through all filters are prime numbers
*/