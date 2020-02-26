/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: josht
 *
 * Created on February 22, 2020, 1:41 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>


/*
 * 
 */

time_t printStartTime()
{
    time_t T = time(NULL);
    printf("START: %ld\n", T);
    return T;
}

void splitProcess(time_t T)
{
int pid, status = 0; 
pid_t wait;
pid = fork();
wait = waitpid(pid, status, 0);
printProcess(pid, status, T);
}

void printProcess(int P, int S, time_t sTime)
{
time_t cTime, pTime;
struct tms buf;
printf("PPID: %d, ", getppid());
if(P == 0)
{
printf("PID: %d\n", getpid());
cTime = time(NULL) - sTime;
}
else
{

printf("PID: %d, CPID: %d, RETVAL: %d\n", getpid(), P, S);
pTime = time(NULL) - sTime;
printf("USER: %jd, SYS: %jd\n", buf.tms_utime, buf.tms_stime);
printf("CUSER: %jd, CSYS: %jd\n", buf.tms_cutime, buf.tms_cstime);
printf("STOP: %jd\n", time(NULL));
}
}

int main(int argc, char** argv) {
    time_t T;
    clock_t times(struct tms *buf);
    T = printStartTime();
    splitProcess(T);
   // printf("%d\n", startTime); 
    return (EXIT_SUCCESS);
}

