#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h> 
#include <time.h>
#include <unistd.h>
#include <string.h>

/*Global Variables*/
static int count = 0;
static int terms = 0;
static const char* sigArray[] = {"HUP", "INT", "QUIT", "ILL", "TRAP", "ABRT", "BUS", "FPE", "KILL",
                                  "USR1", "EGV", "USR2", "PIPE", "ALRM", "TERM", "CHLD", "CONT", "STOP",
                                  "TSTP", "TTIN", "TTOU", "URG", "XCPU", "XFSZ", "VTALRM", "PROF",
                                  "WINCH", "IO", "PWR", "SYS"};  
/*NonMain functions*/
void printStatusLine();
void sigHandler(int sig); 
void initSignal(int argc, char** argv);
void pauseLoop(int argc, char** argv);
void finish();
int retrvNumber(char str[]);

/*EntryPoint for the program*/
int main(int argc, char** argv) {
    printStatusLine();
    initSignal(argc, argv);
    pauseLoop(argc, argv);
    finish();
    return (0);
}

/*Prints initial status and process ID*/
void printStatusLine(){
    pid_t p = getpid();
    fprintf(stderr, "catcher: $$ = %d\n", p);
}

/*Signal handler tracks catches and tracks total signals 
  also tracks SIGTERMS in a row*/
void sigHandler(int sig){
    printf("SIG%s caught at %ld\n", sigArray[sig-1], time(NULL));
    if(sig == 15)
        terms++;
    else
        terms = 0;
    count++;
}

/*loops through and calls signal on all parameters */
void initSignal(int argc, char** argv){
    int i;
    for(i = 1; i < argc; i++){
    if((signal(retrvNumber(argv[i]), sigHandler), sigHandler) == SIG_ERR){
        printf("can't catch sig\n");

        }
    }
}

/*pauses the process until next signal called 
 also reinitializes the signals after the call*/
void pauseLoop(int argc,char** argv){
    while(terms != 3){
        pause();
        initSignal(argc, argv);
    }       
}

/*Prints signal total and gracefully exits program*/
void finish(){
    fprintf(stderr, "catcher: Total signals count = %d\n", count); 
    exit(EXIT_SUCCESS);
}

/*retrieves the number corresponding to the signal from sigArray and returns it*/
int retrvNumber(char str[]){
    int i = 0;
    while(i < 31 && (strcmp(sigArray[i], str) != 0)){
        i++;        
    }
    return ++i;
}