#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h> 
#include <time.h>
#include <unistd.h>

/*
 * 
 */
static int Count = 0;
static char* sigArray = {"SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGEMT"
"SIGEMT","SIGFPE", "SIGKILL", "SIGNUS", "SIGSEGV", "SIGSYS", "SIGPIPE", "SIGALRM", "SIGTERM",
"SIGURG", "SIGSTOP", "SIGTSTP", "SIGCONT", "SIGCHLD", "SIGTTIN", "SIGTTOU", "SIGIO", "SIGXCPU",
"SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGINFO", "SIGUSR1", "SIGUSR2"}; 


void printStatusLine();
void sigHandler(int sig); 
void initSignal(int argc, char* argv);
void pauseLoop();

int main(int argc, char* argv) {
    printStatusLine();
    initSignal(argc, argv);
    return (0);
}

/*Prints initial status and process ID*/
void printStatusLine(){
    pid_t p = getpid();
    fprintf(stderr, "catcher: $$ = %d\n", p);
}

void sigHandler(int sig){
    fprintf(stdout, "%s caught at %d\n", sigArray[sig-1], time(NULL));
    Count++;
}

void initSignal(int argc, char* argv){
    int i;
    for(i = 1; i < argc; i++){
    if(signal("SIG"+argv[1], sigHandler) == SIG_ERR)
    printf("can't catch sig\n");
    }
}

void pauseLoop(){
    while(1){
        pause();
    }
        
}