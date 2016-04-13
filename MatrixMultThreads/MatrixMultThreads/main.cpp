#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "fstream"
using namespace std;

#define MAX_THREADS 3
#define BUF_SIZE 255

DWORD WINAPI MyThreadFunction(LPVOID lpParam);
void ErrorHandler(LPTSTR lpszFunction);
void debug_thread(LPVOID lpParam);

typedef struct MyData {
	int num;//thread n
	int** m1;
	int** m2;
	int** res;
	int size;
} MYDATA, *PMYDATA;

int main(int argc, char** argv)
{
	PMYDATA pDataArray[MAX_THREADS];
	DWORD   dwThreadIdArray[MAX_THREADS];
	HANDLE  hThreadArray[MAX_THREADS];

	if (argc < 2){
		puts("No args");
		return 1;
	}
	int s = 0;
	s = atoi(argv[1]);

	//printf("size: %d\n", s);

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

	for (int i = 0; i < MAX_THREADS; i++){

		// Allocate memory for thread data.

		pDataArray[i] = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
			sizeof(MYDATA));

		if (pDataArray[i] == NULL)
		{
			// If the array allocation fails, the system is out of memory
			// so there is no point in trying to print an error message.
			// Just terminate execution.
			ExitProcess(2);
		}

		// Generate unique data for each thread to work with.

		pDataArray[i]->num = i;
		pDataArray[i]->m1 = tmp1;
		pDataArray[i]->m2 = tmp2;
		pDataArray[i]->res = res;
		pDataArray[i]->size = s;

		// Create the thread to begin execution on its own.

		hThreadArray[i] = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			MyThreadFunction,       // thread function name
			pDataArray[i],          // argument to thread function 
			0,                      // use default creation flags 
			&dwThreadIdArray[i]);   // returns the thread identifier 


		// Check the return value for success.
		// If CreateThread fails, terminate execution. 
		// This will automatically clean up threads and memory. 

		if (hThreadArray[i] == NULL)
		{
			ErrorHandler(TEXT("CreateThread"));
			ExitProcess(3);
		}
	}

	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

	//printf_s("Thread result:\n");

	for (int i = 0; i < s; i++) {
		for (int j = 0; j < s; j++)
			printf("%d\t", res[i][j]);
		printf("\n");
	}

	// Close all thread handles and free memory allocations.

	for (int i = 0; i<MAX_THREADS; i++)
	{
		CloseHandle(hThreadArray[i]);
		if (pDataArray[i] != NULL)
		{
			HeapFree(GetProcessHeap(), 0, pDataArray[i]);
			pDataArray[i] = NULL;    // Ensure address is not reused.
		}
	}
	return 0;
}

void debug_thread(LPVOID lpParam){
	PMYDATA pDataArray;

	pDataArray = (PMYDATA)lpParam;

	//int n_lines = pDataArray->size / MAX_THREADS;
	int n_lines = (pDataArray->size + MAX_THREADS - (pDataArray->size % MAX_THREADS)) / MAX_THREADS;
	int sum = 0;

	int pr = (pDataArray->num + 1)*n_lines;
	if (pr > pDataArray->size)
		pr = pDataArray->size;
	printf_s("nym = %d nl = %d\n", pDataArray->num, n_lines);
	for (int l = pDataArray->num*n_lines; l < pr; l++){
		for (int j = 0; j < pDataArray->size; j++)
		{
			for (int m = 0; m < pDataArray->size; m++){
				sum += pDataArray->m1[l][m] * pDataArray->m2[m][j];
			}
			pDataArray->res[l][j] = sum;
			printf_s(" s = %d\n", sum);
			sum = 0;
		}
	}
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	HANDLE hStdout;
	PMYDATA pDataArray;

	TCHAR msgBuf[BUF_SIZE];
	size_t cchStringSize;
	DWORD dwChars;

	// Make sure there is a console to receive output results. 

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE)
		return 1;

	// Cast the parameter to the correct data type.
	// The pointer is known to be valid because 
	// it was checked for NULL before the thread was created.

	pDataArray = (PMYDATA)lpParam;

	//int n_lines = pDataArray->size / MAX_THREADS;
	int n_lines = (pDataArray->size + MAX_THREADS - (pDataArray->size % MAX_THREADS)) / MAX_THREADS;
	int sum = 0;
	int pr1;

	int pr = (pDataArray->num + 1)*n_lines;

	if (pr > pDataArray->size)
		pr = pDataArray->size;
	for (int l = pDataArray->num*n_lines; l < pr; l++){
		for (int j = 0; j < pDataArray->size; j++)
		{
			for (int m = 0; m < pDataArray->size; m++){
				sum += pDataArray->m1[l][m] * pDataArray->m2[m][j];
			}
			pDataArray->res[l][j] = sum;
			pr1 = sum;
			sum = 0;
		}
	}
	return 0;
}

void ErrorHandler(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code.

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message.

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	// Free error-handling buffer allocations.

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}