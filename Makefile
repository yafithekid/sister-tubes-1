CC=mpicc
CF=mat_mul

compile:
	$(CC) -Wall -o $(CF) $(CF).c -std=c99

move:
	cp $(CF) ~/
	rsync --dirs $(CF) 167.205.32.47:~/$(CF)
	rsync --dirs $(CF) 167.205.32.48:~/$(CF)
work:
	@read -p "Enter number of processes: " NP; \
	read -p "Enter input file: " FILE; \
	mpirun -np $$NP --hostfile mpi_hostfile --bind-to core:overload-allowed ~/$(CF) $$FILE

clean:
	rm -f $(CF)
	rm -f ~$$*.docx
