#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>
/*
 * 
 */

void tube(char** argv);

int main(int argc, char** argv) {
    tube(argv);
    return (0);
}

void tube(char** argv){
    int pipeEnds[2];
    int P, status1, status2;
    int prc1, prc2 = -5;
    P = pipe(pipeEnds);
    if(P == 0){
        prc1 = fork();
        if(prc1 > 0){
            prc2 = fork();
            fprintf(stderr, "%s: $$ = %d\n", argv[1], prc1);
            fprintf(stderr, "%s: $$ = %d\n", argv[2], prc2);
            close(pipeEnds[0]);
            close(pipeEnds[1]);
        }
        else if(prc1 == -1){
        printf("Fork creation failed, error %d\n", errno);
        exit(EXIT_FAILURE);
        }
     
        if(prc1 == 0){
            //dup
        }
        else if(prc2 == 0){
            //dup
    }
        if(prc1 > 0 && prc2 > 0){
            waitpid(prc1, &status1, 0);
            waitpid(prc2, &status2, 0);
            fprintf(stderr, "%s: $? = %d \n", argv[1], status1);
            fprintf(stderr, "%s: $? = %d \n", argv[2], status2);
        }
    }   
    else {
    

        printf("Pipe creation failed, error %d\n", errno);
        exit(EXIT_FAILURE);
    }
}