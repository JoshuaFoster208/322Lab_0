#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

volatile sig_atomic_t sigTerm = 0;
volatile sig_atomic_t sigUsr = -1;

struct Mole{
    pid_t id;
    int exists;//bool
};

void sigSet();
void sigHandler1(int sig);
void sigHandler2(int sig);
void run();
int setGpid();
void setDir();
void setLim(struct rlimit* lim);
void openDescriptor();
void closeFileDescriptors();


int main(int argc, char** argv) {
    argc = argc;
    argv[0] = argv[0];
    printf("here\n");
    run();
    return (EXIT_SUCCESS);
}

void sigSet(){
    if(signal(SIGTERM, sigHandler1) == SIG_ERR){
        fprintf(stderr, "Error: Could not register SIGTERM!\n");
        exit(EXIT_FAILURE);
    }
    if(signal(SIGUSR1, sigHandler2) == SIG_ERR){
        fprintf(stderr, "Error: Could not register SIGUSR1!\n");
        exit(EXIT_FAILURE);
    }
    if(signal(SIGUSR2, sigHandler2) == SIG_ERR){
        fprintf(stderr, "Error: Could not register SIGUSR1!\n");
        exit(EXIT_FAILURE);
    }
}

void sigHandler2(int sig){
   //if sigusr1 is caught update the sig_atomic_t sigusr to 1
    if(sig == SIGUSR1)
        sigUsr = 0;
   //if sigusr2 is caught update the sig_atomic_t sigusr to 2
    else if(sig == SIGUSR2)
        sigUsr = 1;
}

void sigHandler1(int sig){
   //if sigterm is caught update the sig_atomic_t sigTerm to 1
    if (sig == SIGTERM)
       sigTerm = 1;
}


void run(){
    pid_t childPid, gpid;
    struct Mole mole1;
    struct Mole mole2;
    int i = 0, randNum = 0;
    int errnum;    
    
//set the child creation mask to 0
    umask(0);//0000 or 0666

//make a child process    
    childPid = fork();
    if(childPid == -1){//if failed exit
       fprintf(stderr, "Error: Failed to to fork!\n");
        exit(EXIT_FAILURE); 
    }
    else if(childPid != 0){//if not the child close
        printf("childPid =%d \n", childPid);
     exit(EXIT_SUCCESS);   
    }   
    
/*this is only called by the child process*/

 char *params[2];
 params[0] = "0";
 params[1] = NULL;
 
     //call sigSet here to 
    sigSet();
    
//sets it so rand is a different seed each time
    srand(time(NULL));
    //set the group id/sid
    gpid = setGpid();
    //set directory to root
    setDir();
//create the limit struct and close all open file descriptors
    struct rlimit limit;
    if (limit.rlim_max == RLIM_INFINITY){
        limit.rlim_max = 1024;
    }
    for(i=0; i < (int)limit.rlim_max; i++)
        close(i);
//opens the three file descriptors    
    openDescriptor();
    mole1.id = 1;
    mole1.exists = 0;
    mole2.id = 1;
    mole2.exists = 0;    
//while there has not been a sigterm signal
    while(!sigTerm){
//pause and wait for a signal    
    pause(); 
//if sigUsr1 signal was received
    if(mole1.id != 0 || mole2.id != 0)
    {
    if(sigUsr == 0){
        if(mole1.exists == 1){//if mole is not a child
          kill(mole1.id, SIGKILL);
          mole1.exists = 0;
        }
        randNum = (rand() % 2) + 1;
        if(mole1.exists == 0 && randNum == 1){
                printf("im here mole1: %d\n",mole1.id);
            mole1.id = fork();
            mole1.exists = 1;
            if(mole1.id == 0){
                printf("umm mole1.id:%d\n", mole1.id);
            params[0] = "1";
                if(execv("./mole", params) == -1){
                    printf("error\n");
                    errnum = errno;
      fprintf(stderr, "Value of errno: %d\n", errno);
      perror("Error printed by perror");
      fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
                }
                }
            }
        else if (mole2.exists == 0 && randNum == 2){
            mole2.id = fork();
            mole2.exists = 1;
            if(mole2.id == 0){
                printf("umm mole2.id:%d\n", mole2.id);
            params[0] = "2";
            execv("./mole", params);
            if(execv("./mole", params) == -1){
                    printf("error\n");
                    errnum = errno;
      fprintf(stderr, "Value of errno: %d\n", errno);
      perror("Error printed by perror");
      fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
            }
                }
            }
        }
    else if(sigUsr == 1){
            if(mole2.exists == 1){//if mole is not a child
          kill(mole2.id, SIGKILL);
          mole2.exists = 0;
        }
        randNum = (rand() % 2) + 1;
        if(mole1.exists == 0 && randNum == 1){
                printf("im here mole1: %d\n",mole1.id);
            mole1.id = fork();
            mole1.exists = 1;
            if(mole1.id == 0){
                printf("umm mole1.id:%d\n", mole1.id);
            params[0] = "1";
                 if(execv("./mole", params) == -1){
                    printf("error\n");
                    errnum = errno;
      fprintf(stderr, "Value of errno: %d\n", errno);
      perror("Error printed by perror");
      fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
            }
                }
            }
        else if (mole2.exists == 0 && randNum == 2){
            mole2.id = fork();
            mole2.exists = 1;
            if(mole2.id == 0){
                printf("umm mole2.id:%d\n", mole1.id);
            params[0] = "2";
             if(execv("./mole", params) == -1){
                    printf("error\n");
                    errnum = errno;
      fprintf(stderr, "Value of errno: %d\n", errno);
      perror("Error printed by perror");
      fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
            }
                }
            }
        }
            sigUsr = -1;
    }
    }//end of while
    fprintf(stderr, "im here pid: %d\n", getpid());
    kill(mole1.id, SIGTERM);
    kill(mole2.id, SIGTERM);
    exit(EXIT_SUCCESS);
    //if parent do nothing and exit
}

int setGpid(){
    int gpid;
    if((gpid = setsid()) == -1){
       fprintf(stderr, "Error: Failed to create new session!\n");
        exit(EXIT_FAILURE); 
    }
    return gpid;
}

//sets the current working directory to be /
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

void openDescriptor(){//attaches file descriptors 0,1,2 to /dev/null
    int f0,f1,f2;
    f0 = open("/dev/null", O_RDWR);//0
    f1 = dup(f0);//1
    f2 = dup(f0);//2
    
    f0 = f0;
    f1 = f1; 
    f2 = f2;
}


