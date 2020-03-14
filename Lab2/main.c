
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

int main(int argc, char** argv) {
    launch(argv);
    return (0);
}

void launch(int argc, char** argv){
    int prc, status = 0;
     char *env_args[] = {"PATH=/bin", (char*)0 }; 
    prc = fork();

    if(prc > 0){
        fprintf(stderr, "%s: $$ = %d\n", argv[1], prc);
        waitpid(prc, &status, 0);
        fprintf(stderr, "%s: $? = %d \n", argv[1], status);
    } 
    else if (prc == 0){
        execve(argv[1], argv, NULL);
//        printf("%s", argv[1]);
    } 
    else if (prc == -1) {
        printf("Fork creation failed, error %d\n", errno);
        exit(EXIT_FAILURE);
    }
}