mpicc matrix_multiplication.c -o matrix_multiplication
scp matrix_multiplication sister_13512014@167.205.32.47:matrix_multiplication
scp matrix_multiplication sister_13512014@167.205.32.48:matrix_multiplication
mpirun -np 2 --hostfile mpi_hostfile --bind-to core:overload-allowed matrix_multiplication matrix4.txt