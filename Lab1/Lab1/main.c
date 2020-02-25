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

/*
 * 
 */

void printStartTime()
{
    time_t T = time(NULL);
    printf("START: %ld\n", T);
}

void splitProcess()
{
int pid, status = 0; 
pid_t wait;
pid = fork();
wait = waitpid(pid, status, 0);
printProcess(pid, status);
}

void printProcess(int P, int S)
{
printf("PPID: %d, ", getppid());
if(P == 0)
printf("PID: %d\n", getpid());
else
printf("PID: %d, CPID: %d, RETVAL: %d\n", getpid(), P, S);
}

int main(int argc, char** argv) {

printStartTime();
splitProcess();
   // printf("%d\n", startTime); 
    return (EXIT_SUCCESS);
}

