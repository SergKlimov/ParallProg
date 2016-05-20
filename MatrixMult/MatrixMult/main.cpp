#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "fstream"
#include "iostream"
using namespace std;


int **initM(int s){
	int** arr = new int*[s];
	ifstream file("matrix.txt");
	for (int i = 0; i < s; i++){
		arr[i] = new int[s];
		for (int j = 0; j < s; j++){
			file >> arr[i][j];
		}
	}
	file.close();
	return arr;
}

int main(int argc, char** argv)
{
	if (argc < 2){
		puts("No args");
		return 1;
	}
	int s = 0;
	s = atoi(argv[1]);

	FILE *file;
	fopen_s(&file, "matrix.txt", "r");

	int** tmp1 = (int**)malloc(s*sizeof(int*));
	for (int i = 0; i < s; i++)
		tmp1[i] = (int*)malloc(s * sizeof(int));

	int** tmp2 = (int**)malloc(s*sizeof(int*));
	for (int i = 0; i < s; i++)
		tmp2[i] = (int*)malloc(s * sizeof(int));

	int** res = (int**)malloc(s*sizeof(int*));
	for (int i = 0; i < s; i++)
		res[i] = (int*)malloc(s * sizeof(int));

	for (int i = 0; i < s; i++)
		for (int j = 0; j < s; j++){
			if (!fscanf_s(file, "%d", &tmp1[i][j]))
				break;
		}

	for (int i = 0; i < s; i++)
		for (int j = 0; j < s; j++){
			if (!fscanf_s(file, "%d", &tmp2[i][j]))
				break;
		}

	int sum = 0;

	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	double elapsedTime;

	QueryPerformanceFrequency(&frequency);

	QueryPerformanceCounter(&t1);

	for (int i = 0; i < s; i++) {
		for (int j = 0; j < s; j++) {
			for (int k = 0; k < s; k++) {
				sum = sum + tmp1[i][k] * tmp2[k][j];
			}
			res[i][j] = sum;
			sum = 0;
		}
	}

	QueryPerformanceCounter(&t2);

	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	cout << elapsedTime << " ms."<< endl;


	ofstream ost;
	ost.open("seq_res.txt");

	for (int i = 0; i<s; i++) {
		for (int j = 0; j < s; j++)
			ost << res[i][j] << "\t";
		ost << endl;
	}
	ost << endl;
	ost.close();

	return 0;
}