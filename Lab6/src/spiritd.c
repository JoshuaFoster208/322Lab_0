#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>

void sigSet();
void sigHandler(int sig);
void run();
int setGpid();
void setDir();
void setLim(struct rlimit* lim);
void openDescriptor();

int main(int argc, char** argv) {
    run();
    return (EXIT_SUCCESS);
}

void sigSet(){
    if(signal(SIGTERM, sigHandler) == SIG_ERR){
        fprintf(stderr, "Error: Could not register SIGTERM!\n");
        exit(EXIT_FAILURE);
    }
    if(signal(SIGUSR1, sigHandler) == SIG_ERR){
        fprintf(stderr, "Error: Could not register SIGUSR1!\n");
        exit(EXIT_FAILURE);
    }
    if(signal(SIGUSR2, sigHandler) == SIG_ERR){
        fprintf(stderr, "Error: Could not register SIGUSR1!\n");
        exit(EXIT_FAILURE);
    }
}

void sigHandler(int sig){
   /* if (sig == SIGTERM || sig == SIGINT)
     */   
}

void run(){
    //int pid;
    pid_t childPid, gpid;
    umask(0);//0000 or 0666
    //pid = getpid();
    childPid = fork();
    if(childPid == -1){
       fprintf(stderr, "Error: Failed to to fork!\n");
        exit(EXIT_FAILURE); 
    }
    else if(childPid == 0){

    printf("my pid is %d\n", getpid());
    gpid = setGpid();
    setDir();
    struct rlimit limit;
    openDescriptor();
    }
}

int setGpid(){
    int gpid;
    if((gpid = setsid()) == -1){
       fprintf(stderr, "Error: Failed to create new session!\n");
        exit(EXIT_FAILURE); 
    }
    return gpid;
}

void setDir(){
 if(chdir("/") == -1){
     fprintf(stderr, "Error: Failed to set directory to '/'!\n");
        exit(EXIT_FAILURE); 
 }   
}

void setLim(struct rlimit* lim){
   if(getrlimit(RLIMIT_NOFILE, lim) == -1){
     fprintf(stderr, "Error: Failed to set limits!\n");
        exit(EXIT_FAILURE); 
 }    
}

void openDescriptor(){
    int fDes = open("/dev/null", O_RDWR);
    dup(fDes);
    dup(fDes);
}


