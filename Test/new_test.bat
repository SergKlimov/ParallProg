@echo off

set arg1=%1
set arg2=%2

echo Script is running...

for /l %%x in (1, 1, %arg2%) do (
	MatrixMult.exe 1000 >> Tres_seq.txt
)

for /l %%x in (1, 1, %arg2%) do (
	MatrixMultThreads.exe 1000 %arg1% >> Tres_thr.txt
)

for /l %%x in (1, 1, %arg2%) do (
	mpiexec.exe -np %arg1% MatrixMultMpi.exe 1000 >> Tres_mpi.txt
)