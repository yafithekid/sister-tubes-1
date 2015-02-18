mat_gen.c
pasangan matrix generator ukuran N x N dengan bilangan random antara -9 ... 9.

Cara pakai: mat_gen <ukuran_matriks> <file_output>

Contoh: mat_gen 2 matriks2.txt
file matriks2.txt:
2
1 2
3 4
2
5 6
7 8

mat_mul.c
program geje

mpi_hostfile
target hostfile

Makefile

Untuk mengcompile:
make compile

Untuk memindahkan ke server lain:
make move

Untuk menjalankan
make NP=<banyak_proses> FILE=<file_input> work
contoh: make NP=2 FILE=matriks2.txt work
