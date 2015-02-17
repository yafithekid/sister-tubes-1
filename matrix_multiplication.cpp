#include "matrix.hpp"
#include <cstdio>

//Diberikan matriks a dan b, hasilkan c = ab
int mat_a[MAXSIZE][MAXSIZE],mat_b[MAXSIZE][MAXSIZE],mat_c[MAXSIZE][MAXSIZE];

//how to use
int main(int argc,char* argv[]){
    generate_random_matrix(matrix,10,10);
    int numtasks, rank;
    print_matrix(matrix,10,10);
}