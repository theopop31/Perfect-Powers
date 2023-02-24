#include "utils.h"

// get no. of numbers for each file in f, returns 0 for success, -1 for fail
int getNumberForFile(FILE *f, filesStruct *fileNames, int fileNum) {
    
    char *line = NULL;
    size_t len = 0;
    FILE *g;

    for (int i = 0; i < fileNum; ++i) {
        // read file name
        if (getline(&line, &len, f) == -1) {
        printf("Error on reading line\n");
        return -1;
        }

        fileNames[i].name = malloc(MAXNAME * sizeof(char));
        strcpy(fileNames[i].name, strtok(line, "\n\r")); // add file name to structure
        g = fopen(fileNames[i].name, "rt"); // open current file

        if (g == NULL) {
            printf("Eroare la deschiderea fisierului %s\n", fileNames[i].name);
            return -1;
        }
        // read no. of numbers in file
        if (getline(&line, &len, g) == -1) {
            printf("Error on reading line\n");
            return -1;
        }

        fileNames[i].num = atoi(strtok(line, "\n\r")); // add number to structure
        fclose(g);
    }
    return 0;

}

// process each number in file f
void processNumbers(arg *a, FILE *f, int nrReducers, int id) {
    char *line = NULL;
    size_t len = 0;
    int n;
    factDesc desc; // factorial decomposition

    while (getline(&line, &len, f) != -1) {
        n = atoi(strtok(line, "\n\r")); // get number
        if (n == 1) { // 1 is perfect power for every exponent
            for (int i = 2; i <= nrReducers + 1; ++i) {
                a->mapList[id][i][1] = 1; // mark 1^i as perfect power pair
            }
        } else if (n > 1) {
            // get factorial decomposition
            desc = getFactDesc(n, a->nPrimes, a->primeNumbers);
            // improve factorial decomposition
            desc = getCommonDivisors(desc);
            // for every base, mark it and it's exponent in mapper partial list
            for (int i = 0; i < desc.nBases; ++i) {
                a->mapList[id][desc.exponent[i]][desc.base[i]] = 1;
            }
        }
    }

}

// comparison function for qsort
int compareFiles(const void *a, const void *b) {
    filesStruct *nr1 = (filesStruct *)a;
    filesStruct *nr2 = (filesStruct *)b;

    return nr1->num - nr2->num;
}

// sieve of eratosthenes up to n, returns number of prime numbers up to n
int sieve(int n, int *v) {
    bool prime[n + 1];
    memset(prime, true, sizeof(prime));
    int k = 0;

    for (int i = 2; i * i <= n; ++i) {
        if (prime[i]) {
            for (int j = i * i; j <= n; j += i) {
                prime[j] = false;
            }
        }
    }

    // add all prime numbers up to n to array
    for (int i = 2; i <= n; ++i) {
        if (prime[i]) {
            v[k] = i;
            ++k;
        }
    }
    return k;
}

// returns factorial decomposition for number n
factDesc getFactDesc(int n, int nrPrimes, int *v)
{
    factDesc result;
    result.nBases = 0;
    result.smallestExponent = 32;
    memset(result.base, 0, sizeof(result.base));
    memset(result.exponent, 0, sizeof(result.exponent));

    int aux = n;
    // iterate through prime numbers array
    for (int i = 0; v[i] * v[i] <= n && i < nrPrimes; ++i) {
        if (aux % v[i] == 0) { // found a divisor
            result.base[result.nBases] = v[i]; // mark it as base
            // calculate exponent for base
            while (aux % v[i] == 0) {
                aux /= v[i];
                ++result.exponent[result.nBases];
            }
            // if exponent is 1, n cannot be perfect power
            if (result.exponent[result.nBases] == 1) {
                result.smallestExponent = 1;
                ++result.nBases;
                return result;
                
            }
            // we want to keep the smallest exponent in n's decomposition
            if (result.exponent[result.nBases] < result.smallestExponent) {
                result.smallestExponent = result.exponent[result.nBases];
            }
            ++result.nBases;
        }
    }

    if (aux > 1) { // n is prime => cannot be perfect power
        result.base[result.nBases] = aux;
        result.exponent[result.nBases] = 1;
        result.smallestExponent = 1;
        ++result.nBases;
    }

    return result;
}

// for the factorial decomposition desc, search for common divisors of the exponents
factDesc getCommonDivisors(factDesc desc)
{
    factDesc result;
    result.nBases = 0;
    result.smallestExponent = -1;
    int ok;
    memset(result.base, 0, sizeof(result.base));
    memset(result.exponent, 0, sizeof(result.exponent));

    // we only search up to the smallest exponent
    for (int i = 2; i <= desc.smallestExponent; ++i) {
        ok = 1;
        for (int j = 0; j < desc.nBases; ++j) { // iterate through exponents
            if (desc.exponent[j] % i != 0) {
                ok = 0;
                break;
            }
        }
        if (ok) { // i is a common divisor for all exponents
            result.exponent[result.nBases] = i;
            result.base[result.nBases] = 1;
            // compute the new base for the number
            for (int j = 0; j < desc.nBases; ++j) {
                result.base[result.nBases] *= pow(desc.base[j], desc.exponent[j] / i); 
            }
            ++result.nBases;
        }
    }

    return result;
}
