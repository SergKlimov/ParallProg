@echo off
setlocal

set t0=%time: =0%
MatrixMult.exe 1000 > res_seq.txt
set t=%time: =0%

set /a h=1%t0:~0,2%-100
set /a m=1%t0:~3,2%-100
set /a s=1%t0:~6,2%-100
set /a c=1%t0:~9,2%-100
set /a starttime = %h% * 360000 + %m% * 6000 + 100 * %s% + %c%
 
set /a h=1%t:~0,2%-100
set /a m=1%t:~3,2%-100
set /a s=1%t:~6,2%-100
set /a c=1%t:~9,2%-100
set /a endtime = %h% * 360000 + %m% * 6000 + 100 * %s% + %c%

set /a runtime = %endtime% - %starttime%
set runtime = %s%.%c%
 
echo Sequence:
echo Ran for %runtime%0 ms

set t0=%time: =0%
MatrixMultThreads.exe 1000 > res_thr.txt
set t=%time: =0%

set /a h=1%t0:~0,2%-100
set /a m=1%t0:~3,2%-100
set /a s=1%t0:~6,2%-100
set /a c=1%t0:~9,2%-100
set /a starttime = %h% * 360000 + %m% * 6000 + 100 * %s% + %c%

set /a h=1%t:~0,2%-100
set /a m=1%t:~3,2%-100
set /a s=1%t:~6,2%-100
set /a c=1%t:~9,2%-100
set /a endtime = %h% * 360000 + %m% * 6000 + 100 * %s% + %c%

set /a runtime = %endtime% - %starttime%
set runtime = %s%.%c%
 
echo Threads:
echo Ran for %runtime%0 ms

set t0=%time: =0%
mpiexec.exe -np 2 MatrixMultMpi.exe 1000 > res_mpi.txt
set t=%time: =0%

set /a h=1%t0:~0,2%-100
set /a m=1%t0:~3,2%-100
set /a s=1%t0:~6,2%-100
set /a c=1%t0:~9,2%-100
set /a starttime = %h% * 360000 + %m% * 6000 + 100 * %s% + %c%

set /a h=1%t:~0,2%-100
set /a m=1%t:~3,2%-100
set /a s=1%t:~6,2%-100
set /a c=1%t:~9,2%-100
set /a endtime = %h% * 360000 + %m% * 6000 + 100 * %s% + %c%

set /a runtime = %endtime% - %starttime%
set runtime = %s%.%c%
 
echo MPI:
echo Ran for %runtime%0 ms

fc /w res_seq.txt res_mat.txt
fc /w res_thr.txt res_mat.txt
fc /w res_mpi.txt res_mat.txt