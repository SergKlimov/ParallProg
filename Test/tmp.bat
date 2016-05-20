@echo off

set arg1=%1
set arg2=%2

echo %arg1%
echo %arg2%

echo Script is running...

for /l %%x in (1, 1, %arg2%) do (
	REM for /f %%i in ('MatrixMult.exe 1000') do set seq=%%i
	REM for /f %%i in ('MatrixMultThreads.exe 1000 %arg1%') do set thr=%%i
	REM for /f %%i in ('mpiexec.exe -np %arg1% MatrixMultMpi.exe 1000') do set mpi=%%i
	
	MatrixMult.exe 1000 >> res.txt
	MatrixMultThreads.exe 1000 %arg1% >> res.txt
	mpiexec.exe -np %arg1% MatrixMultMpi.exe 1000 >> res.txt
	echo.
	REM echo %%x, %seq%, %thr%, %mpi% >> res.txt
	
	REM set seq=
	REM set thr=
	REM set mpi=
)