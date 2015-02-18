CC=mpicc
CF=mat_mul

compile:
	$(CC) -Wall -o $(CF) $(CF).c -std=c99

move:
	scp $(CF) sister_13512014@167.205.32.47:$(CF)
	# scp $(CF) sister_13512014@167.205.32.48:$(CF)

work:
	mpirun -np ${NP} --hostfile mpi_hostfile --bind-to core:overload-allowed $(CF) ${FILE}
