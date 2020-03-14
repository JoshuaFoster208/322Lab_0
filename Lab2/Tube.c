#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>
/*
 * 
 */

void tube(int argc, char** argv);
int argument(int argc, char** argv);
void fillArrays(char** argv, int argc ,int size, char* newArg1[], char* newArg2[]);


int main2(int argc, char** argv) {    
    tube(argc, argv);
    return (0);
}

void tube(int argc, char** argv){
    int pipeEnds[2];
    int P, status1, status2, size;
    int prc1, prc2 = -5;
    size = argument(argc, argv);
    char* arg1[size+1];
    char* arg2[argc -(size)];
    fillArrays(argv, argc, size, arg1, arg2);
    arg2[argc -(size+1)] =NULL;
    arg1[size] = NULL;
    P = pipe(pipeEnds);
    if(P == 0){
        prc1 = fork();
        if(prc1 > 0){
            if((prc2 = fork()) > 0){
            fprintf(stderr, "%s: $$ = %d\n", argv[1], prc1);
            fprintf(stderr, "%s: $$ = %d\n", argv[size+2], prc2);
            close(pipeEnds[0]);
            close(pipeEnds[1]);
            }
        }
        else if(prc1 == -1){
        printf("Fork creation failed, error %d\n", errno);
        exit(EXIT_FAILURE);
        }
     
        if(prc1 == 0){ 
            dup2(pipeEnds[0], 0);//write end
            close(pipeEnds[1]); 
            execve(arg1[0], arg1, NULL);
            close(pipeEnds[0]);
        }
        else if(prc2 == 0){//read end
            dup2(pipeEnds[0], 0);
            close(pipeEnds[1]);
            execve(arg2[0], arg2, NULL); 
            close(pipeEnds[0]);
    }
        if(prc1 > 0 && prc2 > 0){
            waitpid(prc1, &status1, 0); 
            waitpid(prc2, &status2, 0);
            fprintf(stderr, "\n%s: $? = %d \n", argv[1], status1);
            fprintf(stderr, "%s: $? = %d \n", argv[size+2], status2);
        }
    }   
    else {
    

        printf("Pipe creation failed, error %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

int argument(int argc, char** argv){
    int i = 1;
    while((i < argc) && (argv[i][0] != ',')){
        i++;
    }
    return i - 1;
}

void fillArrays(char** argv, int argc ,int size, char* newArg1[], char* newArg2[]){
    int i, j=0;
    for(i = 1; i <size+1; i++)
    {
    newArg1[i-1] = argv[i];    
    }
    for(i = size+2; i <argc; i++)
    {
    newArg2[j++] = argv[i];    
    }
}