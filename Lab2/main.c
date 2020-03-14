
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
/*
 * 
 */

void launch(int argc, char** argv);
void fillArray(char** argv, int size, char* newArg[]);

int main(int argc, char** argv) {
    launch(argc, argv);
    return (0);
}

void launch(int argc, char** argv){
    int prc, status = 0;
    char* arg[argc-1];
    fillArray(argv, argc, arg);
    prc = fork();

    if(prc > 0){
        fprintf(stderr, "%s: $$ = %d\n", argv[1], prc);
        waitpid(prc, &status, 0);
        fprintf(stderr, "%s: $? = %d \n", argv[1], status);
    } 
    else if (prc == 0){
        execve(arg[0], arg, NULL);
//        printf("%s", argv[1]);
    } 
    else if (prc == -1) {
        printf("Fork creation failed, error %d\n", errno);
        exit(EXIT_FAILURE);
    }
}



void fillArray(char** argv, int argc, char* newArg[]) {
    int i;
    for(i = 1; i < argc; i++)
    {
    newArg[i-1] = argv[i];    
    }
}