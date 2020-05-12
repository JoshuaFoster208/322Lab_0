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
#include <limits.h>
#include <string.h>

//variables updated by the signal handler on signal calls
volatile sig_atomic_t sigTerm = 0;
volatile sig_atomic_t sigUsr = -1;

/*struct that stands for a mole
 * id is the pid of the mole
 * exists works as a bool flag that is used to know if the mole currently exists
 */ 
struct Mole{
    pid_t id;
    int exists;
};

void sigSet();
void sigHandler(int sig);
void run();
int  setGpid();
void setDir();
void setLim(struct rlimit* lim);
void openDescriptor();
void closeFileDescriptors();

/*the main function of the program*/
int main(int argc, char** argv) {
//no parameters so used to remove the warning message in make
    argc = argc;
    argv[0] = argv[0];
    run();
    return (EXIT_SUCCESS);
}

/*Sets up the signal handler by calling signal on the three signal types
 *USR1, USR2, TERM
 */
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

/*When a signal is caught update the associated sig_atomic_t variable accordingly*/
void sigHandler(int sig){
    if (sig == SIGTERM)
        sigTerm = 1;
    else if(sig == SIGUSR1)
        sigUsr = 0;
    else if(sig == SIGUSR2)
        sigUsr = 1;
}


void run(){
    pid_t childPid;
    struct Mole mole1;
    struct Mole mole2;
    int i = 0, randNum = 0;
    char curDirect[PATH_MAX+1] = {"."};
    char temp[PATH_MAX+1];

//My original plan was to store the path in a file and access that from mole
//then to delete the file at the end of this program. I couldn't think of a way to 
//get the mole to find that file so i did something else, however after implementing
//I just realized I could have just passed the path to the file in as a parameter 
//or just passed the homedirectory in as a parameter instead lol oh well
//char *homeDirect = getenv("HOME");
 
//store the current working directory and add a '.' to the beginning and "/mole"
//to the end for the call to execv later in the code
    getcwd(temp, sizeof(curDirect));
    strcat(curDirect, temp);
    strcat(curDirect, "/mole");
    printf("%s\n", curDirect);    
    
//set the child creation mask to 0
    umask(0);//0000 or 0666

//make a child process    
    childPid = fork();
//if failed to fork exit    
    if(childPid == -1){
       fprintf(stderr, "Error: Failed to to fork!\n");
       exit(EXIT_FAILURE); 
    }
//if not the child exit successfully
    else if(childPid != 0){
       exit(EXIT_SUCCESS);   
    }   
    
//This should only be called by the child process
    
//intialize params, params holds the parameters to be passed into execv
 char *params[2];
 params[0] = "0";
 params[1] = NULL;
 
//call sigSet here to setup the signals in the child process
    sigSet();
    
//sets it so rand is a different seed each time
    srand(time(NULL));
    
//set the group sid and exit if it fails
    setGpid();
    
//set directory to root and exit if it fails
    setDir();
    
//create the limit struct and close all open file descriptors
//tbh I need to look into this more I mostly got this part from what people did
//online and from the text book they didn't explain very well
    struct rlimit limit;
    if (limit.rlim_max == RLIM_INFINITY){
        limit.rlim_max = 1024;
    }
    
    for(i=0; i < (int)limit.rlim_max; i++)
        close(i);
    
//opens the three file descriptors
    openDescriptor();
    
//make the moles id 1 so that they can enter the loop and enter the code correctly
//make exists 0 as there are no moles as of now    
    mole1.id = 1;
    mole1.exists = 0;
    mole2.id = 1;
    mole2.exists = 0;    
    
//Loop while there has not been a sigTerm signal
    while(!sigTerm){
        
//pause and wait for a signal, not necessary but helps
    pause(); 

//prevents a child process that has been made to make a mole from entering and
//making another process
    if(mole1.id != 0 || mole2.id != 0)
    {
//if sigUsr1 signal was received
    if(sigUsr == 0){
        
//if mole1 currently exists kill it and set exists back to 0
        if(mole1.exists == 1){
          kill(mole1.id, SIGKILL);
          mole1.exists = 0;
        }
        
//randomly pick a numberfrom 1-2 for making a mole
        randNum = (rand() % 2) + 1;

//if there is no mole1 and the random value was 1 make a new mole1
        if(mole1.exists == 0 && randNum == 1){
            mole1.id = fork();
            mole1.exists = 1;
            
//if the child of the fork call execv            
            if(mole1.id == 0){
            params[0] = "1";
                if(execv(curDirect, params) == -1){
                    fprintf(stderr, "error\n");
                }
            }
        }
//if there is no mole1 and the random value was 2 make a new mole2
        else if (mole2.exists == 0 && randNum == 2){
            mole2.id = fork();
            mole2.exists = 1;
            
//if the child of the fork call execv            
            if(mole2.id == 0){
            params[0] = "2";
            if(execv(curDirect, params) == -1){
                    fprintf(stderr, "error\n");
            }
                }
            }
        }
//if sigUsr2 signal was received
    else if(sigUsr == 1){
//if mole2 currently exists kill it and set exists back to 0
            if(mole2.exists == 1){//if mole is not a child
               kill(mole2.id, SIGKILL);
               mole2.exists = 0;
        }
        //randomly pick a numberfrom 1-2 for making a mole
        randNum = (rand() % 2) + 1;

//if there is no mole1 and the random value was 1 make a new mole1
        if(mole1.exists == 0 && randNum == 1){
            mole1.id = fork();
            mole1.exists = 1;
            
//if the child of the fork call execv            
            if(mole1.id == 0){
            params[0] = "1";
                if(execv(curDirect, params) == -1){
                    fprintf(stderr, "error\n");
                }
            }
        }
//if there is no mole1 and the random value was 2 make a new mole2
        else if (mole2.exists == 0 && randNum == 2){
            mole2.id = fork();
            mole2.exists = 1;
            
//if the child of the fork call execv            
            if(mole2.id == 0){
            params[0] = "2";
            if(execv(curDirect, params) == -1){
                    fprintf(stderr, "error\n");
                }
            }
        }
    }
//reset the sigUsr variable
            sigUsr = -1;
    }
    }//end of while
    fprintf(stderr, "im here pid: %d\n", getpid());
    kill(mole1.id, SIGTERM);
    kill(mole2.id, SIGTERM);
    exit(EXIT_SUCCESS);
    //if parent do nothing and exit
}

/*setGpid sets the SID sends an error if fails*/
int setGpid(){
    int gpid;
    if((gpid = setsid()) == -1){
       fprintf(stderr, "Error: Failed to create new session!\n");
        exit(EXIT_FAILURE); 
    }
    return gpid;
}

/*sets the current working directory to be '/'c*/
void setDir(){
 if(chdir("/") == -1){
     fprintf(stderr, "Error: Failed to set directory to '/'!\n");
        exit(EXIT_FAILURE); 
 }   
}

/*Sets the limit to the value passed in by the lim struct parameter*/
void setLim(struct rlimit* lim){
   if(getrlimit(RLIMIT_NOFILE, lim) == -1){
     fprintf(stderr, "Error: Failed to set limits!\n");
        exit(EXIT_FAILURE); 
 }    
}

/*attaches file descriptors 0,1,2 to /dev/null*/
void openDescriptor(){
    int f0,f1,f2;
    f0 = open("/dev/null", O_RDWR);//0
    f1 = dup(f0);//1
    f2 = dup(f0);//2
//just to remove the warning when calling make    
    f0 = f0;
    f1 = f1; 
    f2 = f2;
}
