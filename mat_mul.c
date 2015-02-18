#include "mpi.h"
#include <stdio.h>

#define MAXSIZE 1024
#define MAX(A,B) (((A)>(B))?(A):(B))

const int MASTER = 0;
const int MASTER_TO_SLAVE_TAG = 0;
const int SLAVE_TO_MASTER_TAG = 10;
//Diberikan matriks a dan b, hasilkan c = ab
int mat_size;
int mat_a[MAXSIZE][MAXSIZE],mat_b[MAXSIZE][MAXSIZE],mat_c[MAXSIZE][MAXSIZE];
int expected_result[MAXSIZE][MAXSIZE];
int part; //seberapa banyak baris yang dihitung tiap proses
int lower_row,upper_row;
MPI_Status status;
MPI_Request request;
double start_time;
double end_time;

//check if valid within range
int valid_range(int pos,int mat_size){
    return (0 <= pos && pos < mat_size);
}

//run: ./run filename
int main(int argc,char* argv[]){
    if (argc != 2){
        printf("Run command: ./matrix_multiplication <input_file_name>\n");
        return 0;
    }
    


    //benda aneh
    int num_tasks, my_rank;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Get_processor_name(processor_name, &name_len);
    //printf("Hello from processor %s, task %d of %d, \n", processor_name, my_rank, num_tasks);
    //printf("Hello from processor %s, task %d of %d, \n", processor_name, my_rank, num_tasks);

    if (my_rank == MASTER){
        //read matriks
        FILE *fin = fopen(argv[1],"r");
        if (fin == NULL){
            printf("File not found\n");
            return 0;
        } else {
            //read first matrix
            int i,j;
            fscanf(fin,"%d",&mat_size);
            for(i = 0; i < mat_size; ++i)
                for(j = 0; j < mat_size; ++j)
                    fscanf(fin,"%d",&mat_a[i][j]);

            //read second matrix
            fscanf(fin,"%d",&mat_size);
            for(i = 0; i < mat_size; ++i)
                for(j = 0; j < mat_size; ++j)
                    fscanf(fin,"%d",&mat_b[i][j]);

            fclose(fin);
        }

        //calculate expected result
        for(int i = 0; i < mat_size; ++i)
            for(int j = 0; j < mat_size; ++j)
                for(int k = 0; k < mat_size; ++k)
                    expected_result[i][j] += (mat_a[i][k] * mat_b[k][j]);
    }

    //send B and all matrix size to all task
    if (my_rank == MASTER){

        for(int i = 1; i < num_tasks; ++i){
            MPI_Send(&mat_size,1,MPI_INT,i,MASTER_TO_SLAVE_TAG + 10,MPI_COMM_WORLD);
            MPI_Send(mat_b,mat_size * MAXSIZE,MPI_INT,i,MASTER_TO_SLAVE_TAG + 11,MPI_COMM_WORLD);
        }
           
    } else {
        MPI_Recv(&mat_size,1,MPI_INT,MASTER,MASTER_TO_SLAVE_TAG + 10,MPI_COMM_WORLD,&status);
        MPI_Recv(mat_b,mat_size * MAXSIZE,MPI_INT,MASTER,MASTER_TO_SLAVE_TAG + 11,MPI_COMM_WORLD,&status);
        //printf("Task %d has receive %d\n",my_rank,mat_size);
    }

    //kirim matriks A
    if (my_rank == MASTER){
        start_time = MPI_Wtime();
        part = MAX(mat_size / num_tasks,1);
        upper_row = part; //master menghitung 0 ... part - 1

        //kirim baris atas sama baris bawah bagian matriks yang perlu dihitung oleh masing-masing kerja
        int i;
        for(i = 1; i < num_tasks; i++){
            lower_row = upper_row;
            upper_row = lower_row + part;

            if (i == num_tasks - 1) //last task, handle the rest
                upper_row = mat_size;
            
            MPI_Send(&lower_row,1,MPI_INT,i,MASTER_TO_SLAVE_TAG,MPI_COMM_WORLD);
            MPI_Send(&upper_row,1,MPI_INT,i,MASTER_TO_SLAVE_TAG + 1,MPI_COMM_WORLD);
            //kirim data matriks
            printf("Task 0 will send to task %d with row: %d to %d\n",i,lower_row,upper_row);
            //MPI_Send(mat_a + lower_row * (MAXSIZE) ,(upper_row - lower_row) * MAXSIZE,MPI_INT,i,MASTER_TO_SLAVE_TAG + 2,MPI_COMM_WORLD);
            MPI_Send(mat_a,mat_size * MAXSIZE,MPI_INT,i,MASTER_TO_SLAVE_TAG + 2,MPI_COMM_WORLD);
        }
        
        //set bagian si master sendiri
        lower_row = 0;
        if (num_tasks == 1) upper_row = mat_size; else upper_row = part;

    } else {
        //slave terima data
        MPI_Recv(&lower_row ,   1,  MPI_INT,MASTER, MASTER_TO_SLAVE_TAG,        MPI_COMM_WORLD,&status);
        MPI_Recv(&upper_row ,   1,  MPI_INT,MASTER, MASTER_TO_SLAVE_TAG + 1,    MPI_COMM_WORLD,&status);
        //MPI_Recv(mat_a + lower_row * (MAXSIZE),(upper_row - lower_row) * MAXSIZE,MPI_INT,MASTER,MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD,&status);   
        MPI_Recv(mat_a,mat_size * MAXSIZE,MPI_INT,MASTER,MASTER_TO_SLAVE_TAG+2,MPI_COMM_WORLD,&status);
    }

    if (valid_range(lower_row,mat_size) && valid_range(upper_row - 1,mat_size)){
        printf("Task %d will work from row %d to %d\n",my_rank,lower_row,upper_row);
        //valid input
        for(int i = lower_row; i < upper_row; ++i){
            for(int j = 0; j < mat_size; ++j){
                for(int k = 0; k < mat_size; ++k){
                    mat_c[i][j] += (mat_a[i][k] * mat_b[k][j]);
                }
            }
        }    
    } else {
        printf("Task %d will not work\n",my_rank);
    }
    

    //selain master kirim data ke master
    //kalo prosesnya kebanyakan dia bakal kirim range yang invalid jadi abaikan kirim matrix
    if (my_rank > MASTER){
        MPI_Send(&lower_row,1,MPI_INT,MASTER,SLAVE_TO_MASTER_TAG,MPI_COMM_WORLD);
        MPI_Send(&upper_row,1,MPI_INT,MASTER,SLAVE_TO_MASTER_TAG+1,MPI_COMM_WORLD);
        if (valid_range(lower_row,mat_size) && valid_range(upper_row - 1,mat_size)){
            MPI_Send(&mat_c[lower_row][0],(upper_row - lower_row) * MAXSIZE,MPI_INT,MASTER,SLAVE_TO_MASTER_TAG+2,MPI_COMM_WORLD);
        }
        
    } else {
        for(int i = 1; i < num_tasks; ++i){
            MPI_Recv(&lower_row,1,MPI_INT,i,SLAVE_TO_MASTER_TAG,MPI_COMM_WORLD,&status);
            MPI_Recv(&upper_row,1,MPI_INT,i,SLAVE_TO_MASTER_TAG+1,MPI_COMM_WORLD,&status);
            if (valid_range(lower_row,mat_size) && valid_range(upper_row - 1,mat_size)){
                MPI_Recv(&mat_c[lower_row][0],(upper_row - lower_row) * MAXSIZE,MPI_INT,i,SLAVE_TO_MASTER_TAG+2,MPI_COMM_WORLD,&status);
            }
        }
        end_time = MPI_Wtime();
    }

    if (my_rank == MASTER){
        //check assertion
        int valid = 1;
        for(int i = 0; i < mat_size; i++)
            for(int j = 0; j < mat_size; j++)
                valid = (valid && (mat_c[i][j] == expected_result[i][j]));
        if (valid){
            printf("Assertion OK, Running time = %.5f\n",end_time - start_time);
        } else {
            printf("Some calculation errors happened, running time = %.5f\n",end_time - start_time);
        }

        // printf("Result matrix:\n");
        // {
        //     int i,j;
        //     for(i = 0; i < mat_size; i++){
        //         for(j = 0; j < mat_size; j++){
        //             printf("%d ",mat_c[i][j]);
        //         }
        //         printf("\n");
        //     }
        // }
        
        // printf("Expected result:\n");
        // for(int i = 0; i< mat_size; ++i){
        //     for(int j = 0; j < mat_size; ++j)
        //         printf("%d ",expected_result[i][j]);
        //     printf("\n");
        // }
    }

    MPI_Finalize();
    return 0;
}