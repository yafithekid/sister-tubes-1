#include <stdlib.h>
#include <stdio.h>

#define MAXSIZE 2048

//generate random number from -9 to 9
int getRandomNumber(){
    return (rand() % 19) - 9;
}

//generate random number
void generateRandomMatrix(int matrix[][MAXSIZE],int N)
{
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            matrix[i][j] = getRandomNumber();
        }
    }
}

//print the matrix
void printMatrix(int matrix[][MAXSIZE], int N){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++)
            printf("%d ",matrix[i][j]);
        printf("\n");
    }
}

void writeMatrixToFile(int matrix[][MAXSIZE],int N,char filename[]){
    FILE *fout = fopen(filename,"a+");
    if (fout == NULL){
        printf("Error: cannot create file\n");
    } else {
        fprintf(fout,"%d\n",N);
        for(int i = 0; i < N; ++i){
            for(int j = 0; j < N; ++j){
                fprintf(fout,"%d ",matrix[i][j]);
            }
            fprintf(fout,"\n");
        }
        printf("Successfully write matrix\n");
        fclose(fout);
    }
}
//usage ./run <matrix_size> <output_file>

int matrix[MAXSIZE][MAXSIZE];

int main(int argc,char* argv[]){
    if (argc != 3){
        printf("Usage ./run <matrix_size> <output_file>");
        return 0;
    }
    int matrix_size;

    sscanf(argv[1],"%d",&matrix_size);
    if (matrix_size > MAXSIZE){
        printf("Size limit %d exceeded\n",MAXSIZE); return 0;
    }

    //first matrix
    generateRandomMatrix(matrix,matrix_size);
    writeMatrixToFile(matrix,matrix_size,argv[2]);
    //second matrix
    generateRandomMatrix(matrix,matrix_size);
    writeMatrixToFile(matrix,matrix_size,argv[2]);
}