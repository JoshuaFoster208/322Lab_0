#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/times.h>
#include <time.h>

//names for files used in the program
#define File "PGROUP.txt"
#define File2 "UNLINKED.txt"

//keeps track of full number of eat-think cycles
static int fullCycles = 0;
/*Accessing or modifying shared objects in signal handlers can result in race 
conditions that can leave data in an inconsistent state. The two exceptions (C 
Standard, 5.1.2.3, paragraph 5) to this rule are the ability to read from and 
write to lock-free atomic objects and variables of type volatile sig_atomic_t. 
Accessing any other type of object from a signal handler is undefined behavior. 
(See undefined behavior 131.) info found at https://wiki.sei.cmu.edu/confluence/
display/c/SIG31-C.+Do+not+access+shared+objects+in+signal+handlers*/
volatile sig_atomic_t Finished = 0;

/*struct holds all info about the chopsticks created, including the current
philosopher seat and position. It also hold two sem_t pointers one for an array 
and the other for the critical section*/
struct chpInfo {
    int total, pos;
    sem_t **chopstick;
    //added for part 2 to create a critical section
    sem_t *HnWChopstick;
};

void validParams(int argc, char** argv);
void eat(struct chpInfo *ch);
void think(struct chpInfo *ch);
void randTime();
void dining(char** argv);
void sigHandler(int sig);
void initSem(char **argv, struct chpInfo *ch);
void setupString(char *chp, struct chpInfo *ch);
void initGpid();
void finishAndClose(struct chpInfo *ch);
void sigSet();
void chpUnlink(struct chpInfo *ch);
void checkFirst(struct chpInfo *ch);

int main(int argc, char** argv) {
    validParams(argc, argv);
    sigSet();
    fprintf(stdout,"Pid: %d\n", getpid());
    dining(argv);
    return (EXIT_SUCCESS);
}

/*Validate that parameters entered fit the requirements
  Only 3 total parameters and position must be within size of seats*/
void validParams(int argc, char** argv){
    if(argc < 3){
        fprintf(stderr, "Error: Not enough arguments, ");
        fprintf(stderr, "number of seats and current position needed.\n");
        exit(EXIT_FAILURE);
    } else if(argc > 3){
        fprintf(stderr, "Error: Too many arguments, ");
        fprintf(stderr, "only number of seats and current position needed\n");
        exit(EXIT_FAILURE);        
    } else{   
        if(atoi(argv[2]) > atoi(argv[1])){
           fprintf(stderr, "Philosopher position greater than number of total seats\n");
           exit(EXIT_FAILURE);
        }
    }
}

//Call signal for term and int
void sigSet(){
    if(signal(SIGTERM, sigHandler) == SIG_ERR){
        fprintf(stderr, "Error: Could not register SIGTERM!\n");
        exit(EXIT_FAILURE);
    }
    if(signal(SIGINT, sigHandler) == SIG_ERR){
        fprintf(stderr, "Error: Could not register SIGINT!\n");
        exit(EXIT_FAILURE);
    }
}

//When signal is caught set Finished flag to 0
void sigHandler(int sig){
   // if (sig == SIGTERM || sig == SIGINT)
    Finished = 1;
}

/*Begins Dining, do while loop to run the eat think cycle*/
void dining(char** argv){
    struct chpInfo ch; 
    //set group id 
    initGpid();
    srand(time(NULL));
    //initialize all the semaphores from 0 to argv[1]
    initSem(argv, &ch);
    do{
        //sem_wait on HnWChopstick to setup a critical section
        sem_wait(ch.HnWChopstick);
            if(sem_wait(ch.chopstick[ch.pos % ch.total]) != -1 &&
               sem_wait(ch.chopstick[(ch.pos+1) % ch.total]) != -1 ){
                    eat(&ch);
                    sem_post(ch.HnWChopstick);
                }
        
            if(sem_post(ch.chopstick[ch.pos % ch.total]) != -1 &&
               sem_post(ch.chopstick[(ch.pos+1) % ch.total]) != -1 && !Finished){
                think(&ch);
                fullCycles++;
                }
    }while(!Finished);
          //close and unlink semaphores
          finishAndClose(&ch);
          fprintf(stderr, "Philosopher #%d completed %d cycles\n", ch.pos, fullCycles);
}

/*Philosopher "eats" for random time
  if the flag has been set skip this
*/
void eat(struct chpInfo *ch){
    if(!Finished){
    fprintf(stdout, "Philosopher #%d is eating\n", ch->pos);
    randTime();
    }
}

/*Philosopher "thinks" for random time
  if the flag has been set skip this
*/
void think(struct chpInfo *ch){
    if(!Finished){
    fprintf(stdout, "Philosopher #%d is thinking\n", ch->pos);
    randTime();
    }
}

/*Initializes array of semaphores based on size of first argument passed in 
 Also sets up a second semaphore for implementing a critical section*/
void initSem(char **argv, struct chpInfo *ch){
    char chops[40];
        ch->total = atoi(argv[1]);
        ch->pos = atoi(argv[2]);
        ch->chopstick = malloc(sizeof(sem_t*) *ch->total+1);
        setupString(chops, ch);
        //added for part 2 to create a critical section
        ch->HnWChopstick = sem_open("allocate", O_CREAT, 0666, 1);
}

/*setup the string name and open a chopstick equal to the amount of seats 
  passed in*/
void setupString(char *chp, struct chpInfo *ch){
    int i;
    for (i = 0; i < ch->total; i++) {
    sprintf(chp, "Chopstick%d", i);
    ch->chopstick[i] = sem_open(chp, O_CREAT, 0666, 1);//maybe 0644
    }
}

/*Check for an existing file, if a file currently exists copy and set the gpid 
 * that is in the file, if no file exists set a gpid and store it in the file*/
void initGpid(){
   FILE *pnt ; 
   struct stat buffer;
    
    // Check if file PGROUP.txt currently exists
    if (stat(File, &buffer) == 0){
        //buffer for getline
        char *b = NULL;
        size_t size = 0;
        //PGROUP.txt already exists open it to be read with "r"
        pnt = fopen(File, "r");
        getline(&b, &size, pnt);
        setpgid(getpid(), strtol(b, NULL, 10));
        fclose(pnt);
    }
    else {
        int gpid = getpgid(getpid());
        //20 covers size of process IDS
        char pid[20];
        memset(pid, 0, 20);
        //PGROUP.txt doesnt exist create it and open to write with "w"
        pnt = fopen(File, "w");
        sprintf(pid, "%d", gpid);
        fwrite(pid, sizeof(char), sizeof(pid), pnt);
        fclose(pnt);
    }
}

//remove philosopher from eat/think cycle and dealocate system resources
void finishAndClose(struct chpInfo *ch){
    int i;
	for (i = 0; i < ch->total; i++) {
            //sem_close first to lower the count to 0 for unlink
            if(sem_close(ch->chopstick[i]) == -1)
                fprintf(stderr, "%d Error: Chopstick%d failed to close \n", getpid(), i);
        }
        if(sem_close(ch->HnWChopstick) == -1)
                fprintf(stderr, "%d Error: HnWChopstick failed to close \n", getpid());
        //Check for if semaphores have already been unlinked
        checkFirst(ch);
        //free the memory set with maloc on chopstick
        free(ch->chopstick);
}

/*Check for existence of a file, if it doesnt exist then create it and unlink,
 if it doesnt exist do nothing as the semaphores hace already been unlinked*/
void checkFirst(struct chpInfo *ch){
    FILE *pnt ; 
    struct stat buffer;
    if (stat(File2, &buffer) != 0){
        printf("%d\n",getpid());
        pnt = fopen(File2, "w");
        fclose(pnt);
        chpUnlink(ch);
    }
}

/*unlinks all the semaphores in the chopstick array as well as the semaphore 
 used for the critical section*/
void chpUnlink(struct chpInfo *ch){
    int i;
    char chops[40];
    for (i = 0; i < ch->total; i++) {
    sprintf(chops, "Chopstick%d", i);
    if(sem_unlink(chops) == -1 && errno !=2) 
       fprintf(stderr, "%d Error: Chopstick%d failed to unlink\n",getpid(), i);        
    }
    if(sem_unlink("allocate") == -1 && errno != 2) 
                fprintf(stderr, "%d Error: allocate failed to unlink\n", getpid());     
}

/*Waits for a random amount of time, no greater than 1,000,000
 set to be at least 99,999 to limit the speed in which the semaphores cycle*/
void randTime(){
    int usec = (rand() % 900000) + 99999;
    usleep(usec);
}

