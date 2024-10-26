#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

void sieveOfEratosthenes(int n) { 
    bool isPrime[n + 1];
    memset(isPrime, true, sizeof(isPrime));
    
    for (int p = 2; p * p <= n; p++) {
        if (isPrime[p] == true) {
            for (int i = p * p; i <= n; i += p)
                isPrime[i] = false;
        }
    }
    
    for (int p = 2; p <= n; p++)
        if (isPrime[p])
            printf("%d ", p);
    printf("\n");
}

int main() {
    int n = 10000; // Find primes up to this number
    printf("Prime numbers up to %d are:\n", n);

    clock_t start = clock();
    
    sieveOfEratosthenes(n);

    clock_t end = clock();

    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time_taken);
    return 0;
}