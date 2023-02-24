#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define MAXNUM 46341 // sqrt(2^31)
#define MAXNAME 40
#define MAXFILES 10001
#define MAXMAPPERS 32 
#define MAXREDUCERS 32
#define MAXPRIMENUM 6500 // max nr of prime numbers up to sqrt(2^31)
#define MAXFACTORS 10 // max amount of prime factors for an int

// macro to check if id i corresponds to a mapper or a reducer, 1 - reducer, 0 - mapper
#define ISREDUCER(i, M, R) ((i) >= (M) && (i) < ((M) + (R)) ? 1 : 0)

typedef struct FileList {
    int num; // number of files
    char list[MAXFILES][MAXNAME]; // file names
} fileList;

typedef struct factDesc {
    int base[MAXFACTORS]; // bases
    int exponent[MAXFACTORS]; // exponents
    int nBases; // number of bases and exponents
    int smallestExponent;
} factDesc;

typedef struct files {
    int num; // no. of numbers in file named name
    char *name; // file name
} filesStruct;

typedef struct argument {
    int id[MAXMAPPERS + MAXREDUCERS]; // thread id
    fileList fList[MAXMAPPERS]; // every mapper's list of files to compute
    int mapList[MAXMAPPERS][MAXREDUCERS + 2][MAXNUM]; // mappers partial list
    int redList[MAXREDUCERS + 2][MAXNUM]; // reducers partial lists
    int primeNumbers[MAXPRIMENUM]; // list of prime numbers
    int nPrimes;

} arg;

int compareFiles(const void *a, const void *b);
int sieve(int n, int *v);
factDesc getFactDesc(int n, int nrPrimes, int *v);
factDesc getCommonDivisors(factDesc desc);
int getNumberForFile(FILE *f, filesStruct *fileNames, int fileNum);
void processNumbers(arg *a, FILE *f, int nrReducers, int id);

