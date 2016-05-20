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

int **initzeros(int n, int m);
int **alloc(int n, int m);
void printMatrix2(int** matrix, int n, int m);
void getChunk(int** src, int** chunk, int s, int n_lines, int rank);

int main(int argc, char **argv) {

	int size, rank;
	int s = 0;

	double startTime, endTime, deltaT;

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

	//startTime = MPI_Wtime();
	if (rank == 0){
		startTime = MPI_Wtime();
		mainThread(s, size);
		endTime = MPI_Wtime();
//		printf("T: %8.8f %8.8f %2.8f\n", startTime, endTime, endTime - startTime);
	}
	else
		computeChunk(s, size, rank);

	//endTime = MPI_Wtime();

	MPI_Finalize();

	return 0;
}

void getChunk(int** src, int** chunk, int s, int n_lines, int rank){
	int pr = (rank + 1)*n_lines;
	if (pr >= s){
		pr = s;
	}
	chunk = initzeros(n_lines, s);
	int t_i;
	for (int i = rank*n_lines, t_i = 0; i < pr, t_i < n_lines; i++, t_i++){
		for (int j = 0; j < s; j++)
			chunk[t_i][j] = src[i][j];
	}
}

void mainThread(int s, int size){
	MPI_Status status;
	int** tmp1;
	int** tmp2;
	int** res;
	int** buf;
	int rank = 0;
	double start, end;

	

	tmp1 = init2d(s);
	tmp2 = init2db(s);
	res = initzero2d(s);
	buf = initzero2d(s);

	start = MPI_Wtime();

	int** tmp_send;

	//cout << "s: " << s << " size: " << size << endl;

	int n_lines = s / size;

	if (s%size!=0)
		n_lines = (s + size - (s % size)) / size;

	tmp_send = initzeros(n_lines, s);

	//cout << "nl: " << n_lines << endl;

	for (int k = 1; k < size; k++){
		for (int i = 0; i < n_lines; i++){
			for (int j = 0; j < s; j++){
				int pir = k*n_lines + i;
				if (pir < s)
					tmp_send[i][j] = tmp1[pir][j];
			}
		}

		/*for (int i = 0; i < n_lines; i++){
			for (int j = 0; j < s; j++){
				cout<<tmp_send[i][j]<<" ";
			}
			cout << endl;
		}*/

		MPI_Send(&(tmp_send[0][0]), n_lines*s, MPI_INT, k, TAG, MPI_COMM_WORLD);
	}


	//MPI_Bcast(&(tmp1[0][0]), s*s, MPI_INT, rank, MPI_COMM_WORLD);
	MPI_Bcast(&(tmp2[0][0]), s*s, MPI_INT, rank, MPI_COMM_WORLD);

	//int** chunk = alloc(n_lines, s);
	//getChunk(tmp1, chunk, s, n_lines, 1);
	//printMatrix2(chunk, n_lines, s);

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
	//printf("T recv results:\t %8.8f %8.8f %2.8f\n", start, end, end - start);
	printf("%4.2f ms.\n", (end - start)*1000);

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
	//int** tmp1;
	int** tmp2;
	int** res;

	//tmp1 = alloc2d(s);
	tmp2 = alloc2d(s);
	res = initzero2d(s);

	MPI_Status status;

	int n_lines = s / size;

	if (s%size != 0)
		n_lines = (s + size - (s % size)) / size;

	int** tmp_recv = initzeros(n_lines, s);

	MPI_Recv(&(tmp_recv[0][0]), n_lines*s, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);

	//MPI_Bcast(&(tmp1[0][0]), s*s, MPI_INT, src, MPI_COMM_WORLD);
	MPI_Bcast(&(tmp2[0][0]), s*s, MPI_INT, src, MPI_COMM_WORLD);
	
	int sum = 0;
	int pr1;

	int pr = (rank + 1)*n_lines;

	if (pr > s)
		pr = s;
	/*for (int l = rank*n_lines; l < pr; l++){
		for (int j = 0; j < s; j++)
		{
			for (int m = 0; m < s; m++){
				sum += tmp1[l][m] * tmp2[m][j];
			}
			res[l][j] = sum;
			pr1 = sum;
			sum = 0;
		}
	}*/

	int pira = n_lines * rank;

	if (pira > s){
		pira = s%n_lines;
	}
	else
	{
		pira = n_lines;
	}

	for (int l = rank*n_lines, p = 0; l < pr; l++, p++){
		for (int j = 0; j < s; j++)
		{
			for (int m = 0; m < s; m++){
				sum += tmp_recv[p][m] * tmp2[m][j];
			}
			res[l][j] = sum;
			pr1 = sum;
			sum = 0;
		}
	}

	/*for (int l = rank*n_lines; l < pr; l++){
		for (int j = 0; j < s; j++)
		{
			for (int m = 0; m < s; m++){
				sum += tmp1[l][m] * tmp2[m][j];
			}
			res[l][j] = sum;
			pr1 = sum;
			sum = 0;
		}
	}*/

	MPI_Send(&(res[0][0]), s*s, MPI_INT, 0, TAG, MPI_COMM_WORLD);
	//cout << "Thread " << rank << " ended." << endl;
	//free2d(tmp1);
	free2d(tmp2);
	free2d(res);
}

void printMatrix(int** matrix, int n){

	ofstream ost;
	ost.open("mpi_res.txt");

	for (int i = 0; i<n; i++) {
		for (int j = 0; j < n; j++)
			ost << matrix[i][j] << "\t";
		ost << endl;
	}
	ost << endl;
	ost.close();
}

void printMatrix2(int** matrix, int n, int m){
	/*for (int i = 0; i<n; i++) {
		for (int j = 0; j < m; j++)
			cout << matrix[i][j] << "\t";
		cout << endl;
	}
	cout << endl;*/
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
		file.ignore(numeric_limits<streamsize>::max(), '\n');
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

int **initzeros(int n, int m) {
	int *data = (int*)malloc(n*m*sizeof(int));
	int **array = (int**)malloc(n*sizeof(int *));
	for (int i = 0; i < n; i++) {
		array[i] = &(data[i*m]);
	}
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			array[i][j] = 0;
	return array;
}

int **alloc(int n, int m) {
	int *data = (int*)malloc(n*m*sizeof(int));
	int **array = (int**)malloc(n*sizeof(int *));
	for (int i = 0; i < n; i++) {
		array[i] = &(data[i*m]);
	}
	return array;
}

void free2d(int **array) {
	free(array[0]);
	free(array);
}