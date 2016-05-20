@echo off

set arg1=%1

echo Script is running...

for /f %%i in ('MatrixMult.exe 1000') do set seq=%%i
for /f %%i in ('MatrixMultThreads.exe 1000 %arg1%') do set thr=%%i
for /f %%i in ('mpiexec.exe -np %arg1% MatrixMultMpi.exe 1000') do set mpi=%%i

@echo Sequental: %seq%
@echo Threads:   %thr%
@echo MPI:       %mpi%

fc /w seq_res.txt res_mat.txt
fc /w thr_res.txt res_mat.txt
fc /w mpi_res.txt res_mat.txt