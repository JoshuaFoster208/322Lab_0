#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>
/*
 * 
 */  
//Declarations 
void tube(int argc, char** argv);
int argument(int argc, char** argv);
void fillArrays(char** argv, int argc ,int size, char* newArg1[], char* newArg2[]);

//main function  
int main(int argc, char** argv) {         
    tube(argc, argv);
    return (0);
}

//Sets up a tube between two child processes and executes 2 commands passed in
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
    if(P == 0){//pipe implementation was successful
        prc1 = fork();
        if(prc1 > 0){//parent process 
            if((prc2 = fork()) > 0){ //if not new child prc
            fprintf(stderr, "%s: $$ = %d\n", argv[1], prc1);
            fprintf(stderr, "%s: $$ = %d\n", argv[size+2], prc2);
            close(pipeEnds[0]);
            close(pipeEnds[1]);
            }
        }
        else if(prc1 == -1){//if first child failed
        printf("Fork creation failed, error %d\n", errno);
        exit(EXIT_FAILURE);
        } 
         
        if(prc1 == 0){ //if first child
            dup2(pipeEnds[0], 0);
            close(pipeEnds[1]); 
            execve(arg1[0], arg1, NULL);
            close(pipeEnds[0]);
        }
        else if(prc2 == 0){//if second child process
            dup2(pipeEnds[0], 0);
            close(pipeEnds[1]);
            execve(arg2[0], arg2, NULL); 
            close(pipeEnds[0]);
        }
        
        if(prc1 > 0 && prc2 > 0){ //if parent process wait for children
            waitpid(prc1, &status1, 0); 
            waitpid(prc2, &status2, 0);
            fprintf(stderr, "\n%s: $? = %d \n", argv[1], status1);
            fprintf(stderr, "%s: $? = %d \n", argv[size+2], status2);
}
    }   
    else {//if pipe implementation failed
        printf("Pipe creation failed, error %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

//returns size array to hold the first command and its parameters
int argument(int argc, char** argv){
    int i = 1;
    while((i < argc) && (argv[i][0] != ',')){
        i++;
    }
    return i - 1;
}

//Loads the 2 commands and their parameters into seperate arrays
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