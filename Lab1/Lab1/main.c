//Header Files

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>

//prints the first lines and the start
time_t printStartTime(){
    time_t T = time(NULL);
    printf("START: %ld\n", T);
    return T;
}

//prints processes for both the child and parent fucntions
void printProcess(int P, int S){
    struct tms buf;
    printf("PPID: %d, ", getppid());
    if(P == 0){
    printf("PID: %d\n", getpid());
    }
    else{
    printf("PID: %d, CPID: %d, RETVAL: %d\n", getpid(), P, S);
    times(&buf);
    printf("USER: %lu, SYS: %lu\n", buf.tms_utime, buf.tms_stime);
    printf("CUSER: %lu, CSYS: %lu\n", buf.tms_cutime, buf.tms_cstime);
    printf("STOP: %lu\n", time(NULL));
    }
}

void splitProcess(){
    int pid;
    int status = 0; 
    pid_t wait;
    pid = fork();
    wait = waitpid(pid, &status, 0);
    printProcess(pid, status);
}

int main(int argc, char** argv) {
    time_t T;
    T = printStartTime();
    splitProcess(T);
    return (EXIT_SUCCESS);
}

