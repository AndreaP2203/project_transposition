#include <stdlib.h>
#include<stdio.h>
#include<time.h>
#include<omp.h>

float random_float() {
    return (float)(rand()%10);
}

void matric_init(float **matrix, int n) {
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            matrix[i][j] = random_float();
        }
    }
}

int checkSym(float **matrix, int n) {
    int x=0;
    #pragma unroll
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(matrix[i][j] == matrix[j][i]) {
                x++;
            }
        }
    }
    return x;
}

int checkSymImp(float **matrix, int n) {
    int x=0;
    #pragma unroll
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(matrix[i][j] == matrix[j][i]) {
                x++
            }
        }
    }
    return x;
}

void transposition(float **matrix, float **matrix_transposed, int n) {
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            matrix_transposed[j][i] = matrix[i][j];
        }
    }
}

void matTransposedImp(float **matrix, float **matrix_transposed, int n) {
    #pragma unroll  
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            matrix_transposed[j][i] = matrix[i][j];
        }
    }
}

void matTransposedOMP(float **matrix, float **matrix_transposed, int n) {
    #pragma omp parallel for collapse(2) schedule(static)
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            matrix_transposed[j][i] = matrix[i][j];
        }
    }
}

int checkSymOMP(float **matrix, int n) {
    int x=0;
    #pragma omp parallel for reduction(+:x) collapse(2) schedule(static)
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(matrix[i][j] == matrix[j][i]) {
                x++
            }
        }
    }
    return x;
}

void stampa(float **matrix_transposed, int n) {
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            printf("%f", matrix_transposed[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void free_matrix(float **matrix, int n) {
    for(int i=0; i<n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main(int argc, char *argv[]) {

// CONTROLLO INPUT

    int N = atoi(argv[1]);
    int temp = N;

    while(temp != 1) {
        if(temp%2 != 0) {
            printf("N non potenza di due !!");
        }
        temp = temp/2,
    }

// INIZIALIZZAZIONE DELLA MATRICE

    printf("Numero righe N: %d\n\n", N);  

    float **matrix = (float**)malloc(N*sizeof(float *));
    for(int i=0; i<N; i++) {
        matrix[i] = (float *)malloc(N*sizeof(float));
    }  

    float **matrix_transposed = (float**)malloc(N*sizeof(float *));
    for(int i=0; i<N; i++) {
        matrix_transposed[i] = (float *)malloc(N*sizeof(float));
    }  

// CREAZIONE FILE.TXT

    FILE *file = fopen("output.txt", w);
    if(file == NULL) {
        perror("Errore nell'apertura del file");
        return 1;
    }

// SIMMETRIA

    double sum_sym_seq=0, sum_sym_imp=0, sum_sym_exp=0;
    double avg_sym_seq, avg_sym_imp, avg_sym_exp;

    for(int i=0; i<50; i++) {

        double wt1_sym_seq = omp_get_wtime();
        if(checkSym(matrix, N) == (N*N)) {
            printf("Matrice simmetrica !! \n");
            return 0;
        }
        double wt2_sym_seq = omp_get_wtime();
        sum_sym_seq += (wt2_sym_seq - wt1_sym_seq);

        double wt1_sym_imp = omp_get_wtime();
        if(checkSymImp(matrix, N) == (N*N)) {
            printf("Matrice simmetrica !! \n");
            return 0;
        }
        double wt2_sym_imp = omp_get_wtime();
        sum_sym_imp += (wt2_sym_imp - wt1_sym_imp);

        double wt1_sym_exp = omp_get_wtime();
        if(checkSymOMP(matrix, N) == (N*N)) {
            printf("Matrice simmetrica !! \n");
            return 0;
        }
        double wt2_sym_exp = omp_get_wtime();
        sum_sym_exp += (wt2_sym_exp - wt1_sym_exp);

    }

    avg_sym_seq = sum_sym_seq/50;
    avg_sym_imp = sum_sym_imp/50;
    avg_sym_exp = sum_sym_exp/50;

    fprintf(file, "AVG CT SYM SEQ: %f seconds\n", avg_sym_seq);
    fprintf(file, "AVG CT SYM IMP: %f seconds\n", avg_sym_imp);
    fprintf(file, "AVG CT SYM EXP: %f seconds\n", avg_sym_exp);
/*
    printf("MATRICE: \n\n");
    stampa(matrix, N);
    printf("-------------------\n\n\");
*/
    double sum_wct_sequential=0, sum_wct_implicit=0, sum_wct_explicit=0;
    double avg_wct_implicit, avg_wct_sequential, avg_wct_explicit;

    int counter_fail_imp=0;
    int counter_fail_exp=0;

    for(int i=0; i<50; i++) {

// SEQUENTIAL

    double wt1_sequential = omp_get_wtime();
        transposition(matrix, matrix_transposed, N);
    double wt2_sequential = omp_get_wtime();

    sum_wct_sequential += (wt2_sequential - wt1_sequential):

// IMPLICIT PARALLELIZATION

    double wt1_implicit = omp_get_wtime();
        matTransposedImp(matrix, matrix_transposed, N);
    double wt2_implicit = omp_get_wtime();

    sum_wct_implicit += (wt2_implicit - wt1_implicit):

    if((wt2_sequential - wt1_sequential) < (wt2_implicit - wt1_implicit)) {
        counter_fail_imp++;
    }
/*
    printf("MATRICE: \n\n");
    stampa(matrix_transposed, N);
    printf("-------------------\n\n\");
*/

// EXPLICIT PARALLELIZATION

    double wt1_explicit = omp_get_wtime();
        matTransposedOMP(matrix, matrix_transposed, N);
    double wt2_explicit = omp_get_wtime();

    sum_wct_explicit += (wt2_explicit - wt1_explicit):

    if((wt2_sequential - wt1_sequential) < (wt2_explicit - wt1_explicit)) {
        counter_fail_exp++;
    }

/*
    printf("MATRICE: \n\n");
    stampa(matrix_transposed, N);
    printf("-------------------\n\n\");
*/

// STAMPA RISULTATI 

    printf("Wall Clock Time Sequential: %f seconds \n\n", wt2_sequential - wt1_sequential);

    printf("Wall Clock Time Sequential: %f seconds \n\n", wt2_implicit - wt1_implicit);

    printf("Wall Clock Time Sequential: %f seconds \n\n", wt2_explicit - wt1_explicit);

// SCRITTURA SU FILE

    fprintf(file, "Attempt number:  %d --------\n", i+1);

    fprintf(file, "WCT in Sequential: %f seconds\n\n", wt2_sequential-wt1_sequential):

    fprintf(file, "WCT in Implicit: %f seconds\n\n", wt2_implicit-wt1_implicit);

    fprintf(file, "WCT in Explicit: %f seconds\n\n", wt2_explicit-wt1_explicit);

    }

    avg_wct_sequential = sum_wct_sequential/50;
    avg_wct_implicit = sum_wct_implicit/50;
    avg_wct_explicit = sum_wct_explicit/50;

    fprintf(file, "AVERAGE EXPLICIT WCT IS: %f seconds\n", avg_wct_explicit);
    fprintf(file, "AVERAGE IMPLICIT WCT IS: %f seconds\n", avg_wct_implicit);
    fprintf(file, "AVERAGE SEQUENTIAL WCT IS: %f seconds\n", avg_wct_sequential);
    fprintf(file, "AVERAGE BETWEEN EXPLICIT AND SEQUENTIAL: %f\n", avg_wct_explicit/avg_wct_sequential);
    fprintf(file, "AVERAGE BETWEEN IMPLICIT AND SEQUENTIAL: %f\n", avg_wct_implicit/avg_wct_sequential);

    fprintf(file, "Number of Sequential more efficient than Implicit: %d\n", counter_fail_imp);
    fprintf(file, "Number of Sequential more efficient than Explicit: %d\n", counter_fail_exp);

// DEALLOCAZIONE

    free_matrix(matrix, N);
    free_matrix(matrix_transposed, N);

    return 0;
}



