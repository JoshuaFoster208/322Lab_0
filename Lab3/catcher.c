#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h> 
#include <time.h>
#include <unistd.h>
#include <string.h>

/*
 * 
 */
int count = 0;
int terms = 0;
static char* sigArray[] = {"SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGEMT",
"SIGFPE", "SIGKILL", "SIGNUS", "SIGSEGV", "SIGSYS", "SIGPIPE", "SIGALRM", "SIGTERM",
"SIGURG", "SIGSTOP", "SIGTSTP", "SIGCONT", "SIGCHLD", "SIGTTIN", "SIGTTOU", "SIGIO", "SIGXCPU",
"SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGINFO", "SIGUSR1", "SIGUSR2"}; 
static const char sigArray1[31][16] = {"HUP", "INT", "QUIT", "ILL", "TRAP", "ABRT", "BUS", "FPE", "KILL",
                                  "USR1", "EGV", "USR2", "PIPE", "ALRM", "TERM", "CHLD", "CONT", "STOP",
                                  "TSTP", "TTIN", "TTOU", "URG", "XCPU", "XFSZ", "VTALRM", "PROF",
                                  "WINCH", "IO", "PWR", "SYS"};  

void printStatusLine();
void sigHandler(int sig); 
void initSignal(int argc, char** argv);
void pauseLoop(int argc, char** argv);
void finish();
int retrvNumber(char str[]);


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

void sigHandler(int sig){
    printf("%s caught at %ld\n", sigArray[sig-1], time(NULL));
    if(sig == 15)
        terms++;
    else
        terms = 0;
    count++;
}

void initSignal(int argc, char** argv){
    int i;
    for(i = 1; i < argc; i++){
    /*if(signal(retrvNumber(temp), sigHandler) == SIG_ERR)
    printf("can't catch sig\n");*/
        signal(retrvNumber(argv[i]), sigHandler);
    }

}

void pauseLoop(int argc,char** argv){
    while(terms != 3){
        pause();
        initSignal(argc, argv);
    }       
}

void finish(){
    fprintf(stderr, "catcher: Total signals count = %d\n", count); 
    exit(EXIT_SUCCESS);
}

int retrvNumber(char str[]){
    int i = 0;
    while(i < 31 && (strcmp(sigArray1[i], str) != 0)){
        i++;        
    }
//    printf("return %d\n", i+1);
    return ++i;
}