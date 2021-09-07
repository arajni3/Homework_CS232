/* File: primes.c */
/* Author: Britton Wolfe */
/* Date: July 16, 2009 */
/* This program outputs all the primes in the range given */
/* by the command line arguments */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, const char** argv){

    int lowerBound, upperBound;

    if(argc != 3){
        fprintf(stderr, "USAGE: %s lowerBound upperBound\n", argv[0]);
        return -1;
    }
  
    lowerBound = atoi(argv[1]);
    upperBound = atoi(argv[2]);
  
    if(lowerBound < 1 || upperBound < 1){
        fprintf(stderr, "ERROR: both the lowerBound (%d) and the upperBound (%d)"
	          " must be positive.\n", lowerBound, upperBound);
        return -2;
    }
  
    {
        /* TODO: fill in the code that outputs the prime numbers */
        /*   in the range [lowerBound,upperBound] in ascending order */
        for (int num = lowerBound; num < upperBound; ++num) {
            // initially assume num is prime
            int isPrime = 1;

            // 1 is not prime
            if (num == 1) {
                isPrime = 0;
            }

            // if num != 1, then check all possible divisors of num in brute force fashion
            for (int possible_factor = 2; (possible_factor < num) && isPrime; ++possible_factor) {
                if (num % possible_factor) {
                    isPrime = 0;
                }
            }

            if (isPrime) {
                printf("%d", num);
            }
        }

    }

    return 0;
}
