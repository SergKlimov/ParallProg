#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "fstream"
#include "iostream"

#define TAG 111

using namespace std;

void printMatrix(int** matrix, int n);
int **alloc2d(int n);
int **init2d(int n);
int **init2db(int n);
int **initzero2d(int n);
void free2d(int **array);
void computeChunk(int s, int size, int rank);
void mainThread(int s, int size);

int main(int argc, char **argv) {

	int size, rank;
	int s = 0;

	if (argc < 2){
		puts("No args");
		return 1;
	}
	s = atoi(argv[1]);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2) {
		fprintf(stderr, "Requires at least two processes.\n");
		exit(-1);
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0){
		mainThread(s, size);
	}
	else
		computeChunk(s, size, rank);

	MPI_Finalize();

	return 0;
}

void mainThread(int s, int size){
	MPI_Status status;
	int** tmp1;
	int** tmp2;
	int** res;
	int** buf;
	int rank = 0;
	double start, end;

	start = MPI_Wtime();

	tmp1 = init2d(s);
	tmp2 = init2db(s);
	res = initzero2d(s);
	buf = initzero2d(s);

	MPI_Bcast(&(tmp1[0][0]), s*s, MPI_INT, rank, MPI_COMM_WORLD);
	MPI_Bcast(&(tmp2[0][0]), s*s, MPI_INT, rank, MPI_COMM_WORLD);

	int n_lines = (s + size - (s % size)) / size;
	int sum = 0;
	int pr1;

	int pr = (rank + 1)*n_lines;

	if (pr > s)
		pr = s;
	for (int l = rank*n_lines; l < pr; l++){
		for (int j = 0; j < s; j++)
		{
			for (int m = 0; m < s; m++){
				sum += tmp1[l][m] * tmp2[m][j];
			}
			res[l][j] = sum;
			pr1 = sum;
			sum = 0;
		}
	}

	for (int i = 1; i < size; i++){
		MPI_Recv(&(buf[0][0]), s*s, MPI_INT, i, TAG, MPI_COMM_WORLD, &status);
		int pr = (i + 1)*n_lines;
		if (pr > s)
			pr = s;
		for (int l = i*n_lines; l < pr; l++){
			for (int j = 0; j < s; j++)
				res[l][j] = buf[l][j];
		}
	}

	end = MPI_Wtime();


	//cout << "Result:" << endl;

	printMatrix(res, s);

	//cout << "Thread " << rank << " ended." << endl;

	//printf("\nRunning Time = %f\n\n", end - start);

	free2d(tmp1);
	free2d(tmp2);
	free2d(res);
	free2d(buf);
}

void computeChunk(int s, int size, int rank){
	const int src = 0;
	int** tmp1;
	int** tmp2;
	int** res;

	tmp1 = alloc2d(s);
	tmp2 = alloc2d(s);
	res = initzero2d(s);
	MPI_Bcast(&(tmp1[0][0]), s*s, MPI_INT, src, MPI_COMM_WORLD);
	MPI_Bcast(&(tmp2[0][0]), s*s, MPI_INT, src, MPI_COMM_WORLD);
	int n_lines = (s + size - (s % size)) / size;
	int sum = 0;
	int pr1;

	int pr = (rank + 1)*n_lines;

	if (pr > s)
		pr = s;
	for (int l = rank*n_lines; l < pr; l++){
		for (int j = 0; j < s; j++)
		{
			for (int m = 0; m < s; m++){
				sum += tmp1[l][m] * tmp2[m][j];
			}
			res[l][j] = sum;
			pr1 = sum;
			sum = 0;
		}
	}

	MPI_Send(&(res[0][0]), s*s, MPI_INT, 0, TAG, MPI_COMM_WORLD);
	//cout << "Thread " << rank << " ended." << endl;
	free2d(tmp1);
	free2d(tmp2);
	free2d(res);
}

void printMatrix(int** matrix, int n){
	for (int i = 0; i<n; i++) {
		for (int j = 0; j < n; j++)
			cout << matrix[i][j] << "\t";
		cout << endl;
	}
	cout << endl;
}

int **alloc2d(int n) {
	int *data = (int*)malloc(n*n*sizeof(int));
	int **array = (int**)malloc(n*sizeof(int *));
	for (int i = 0; i<n; i++) {
		array[i] = &(data[i*n]);
	}
	return array;
}

int **init2d(int n) {
	ifstream file("matrix.txt");
	int *data = (int*)malloc(n*n*sizeof(int));
	int **array = (int**)malloc(n*sizeof(int *));
	for (int i = 0; i<n; i++) {
		array[i] = &(data[i*n]);
	}
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			file >> array[i][j];
	return array;
}

int **init2db(int n) {
	ifstream file("matrix.txt");
	int *data = (int*)malloc(n*n*sizeof(int));
	int **array = (int**)malloc(n*sizeof(int *));
	for (int i = 0; i<n; i++) {
		array[i] = &(data[i*n]);
	}

	for (int i = 0; i < n; i++)
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			file >> array[i][j];
	return array;
}

int **initzero2d(int n) {
	int *data = (int*)malloc(n*n*sizeof(int));
	int **array = (int**)malloc(n*sizeof(int *));
	for (int i = 0; i<n; i++) {
		array[i] = &(data[i*n]);
	}
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			array[i][j] = 0;
	return array;
}

void free2d(int **array) {
	free(array[0]);
	free(array);
}