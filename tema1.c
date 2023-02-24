#include "utils.h"

int M, R;
int fileNum;
arg a;
pthread_barrier_t barrier;

void *func(void *arg) {
    int id = *(int*)arg;
    FILE *f;
    char *line = NULL;
    size_t len = 0;

    if (ISREDUCER(id, M, R) == 0) { // mapper thread
    
        for (int i = 0; i < a.fList[id].num; ++i) {
            f = fopen(a.fList[id].list[i], "r"); // open file
            if (f == NULL) {
                printf("Eroare la deschiderea fisierului %s\n", a.fList[id].list[i]);
                exit(-1);
            }
            getline(&line, &len, f); // we don't need the first number

            processNumbers(&a, f, R, id); // compute file content

            fclose(f); // close file
        }
    }
    pthread_barrier_wait(&barrier);
    if (ISREDUCER(id, M, R) == 1) { // reducer thread
        
        int exp = id - M + 2; // compute exponent based on reducer id
        int nr = 0;
        char *filename = malloc(20);
        FILE *g;

        // for every mapper and every possible base, search for exponent
        for (int i = 0; i < M; ++i) {
            for (int j = 1; j < MAXNUM; ++j) {
                if (a.mapList[i][exp][j] == 1) {
                    a.redList[exp][j] = 1; // mark exponent exp and base j
                }
            }
        }

        // count number of distinct pairs base-exponent
        for (int i = 1; i < MAXNUM; ++i) {
            if (a.redList[exp][i] == 1) {
                ++nr;
            }
        }

        sprintf(filename, "out%d.txt", exp); // create output file name
        g = fopen(filename, "w");
        fprintf(g, "%d", nr);
        fclose(g);
        free(filename);
    }
    pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
    char *filename = malloc(40 * sizeof(char));
    char *line = NULL;
    size_t len = 0;
    FILE *f;
    M = atoi(argv[1]); // no. of mappers
    R = atoi(argv[2]); // no. of reducers
    pthread_t threads[M + R];
    
    

    strcpy(filename, argv[3]);
    f = fopen(filename, "r"); // open file containing test file names
    if (f == NULL) {
        printf("Eroare la deschiderea fisieruluide input\n");
        exit(-1);
    }
    
    getline(&line, &len, f);
    fileNum = atoi(strtok(line, "\n\r")); // no. of files to process

    filesStruct *fileNames = (filesStruct *) calloc(fileNum, sizeof(filesStruct));

    if (getNumberForFile(f, fileNames, fileNum) == -1) {
        printf("Error reading number of numbers per file\n");
        exit(-1);
    }
    
    fclose(f); // close file
    
    // sort files after the amount of numbers in each one
    qsort(fileNames, fileNum, sizeof(filesStruct), compareFiles);

    // distribute files for each mapper
    for (int i = 0; i < fileNum; ++i) {
        ++a.fList[i % M].num;
        strcpy(a.fList[i % M].list[i / M], fileNames[i].name);
    }

    // calculate all prime numbers up to sqrt(2^31 - MAXINT)
    a.nPrimes = sieve(MAXNUM, a.primeNumbers);
    
    pthread_barrier_init(&barrier, NULL, M + R); // init barrier

    // create threads
    for (int i = 0; i < M + R; ++i) {
        a.id[i] = i;
        if (pthread_create(&threads[i], NULL, func, &a.id[i])) {
            printf("Eroare la crearea thread-ului %d\n", i);
            exit(-1);
        }
    }

    // join threads
    for (int i = 0; i < M + R; ++i) {
        if (pthread_join(threads[i], NULL)) {
            printf("Eroare la asteptarea thread-ului %d\n", i);
            exit(-1);
        }
    }

    
    pthread_barrier_destroy(&barrier); // destroy barrier

    // free memory
    free(filename);
    for(int i = 0; i < fileNum; ++i) {
        free(fileNames[i].name);
    }
    free(fileNames);
    return 0;
}