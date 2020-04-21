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
#include <signal.h>


sem_t * returnVal;
static int fullCycles = 0;
static int Finished = 0;

void validParams(int argc, char** argv);
void eat(int p);
void think(int p);
void randTime();
void dining(int argc, char** argv);
void sigHandler(int sig);
void initSem(int seats, sem_t *S);
void initGpid();
void finishEating();

int main(int argc, char** argv) {
    validParams(argc, argv);
    fprintf(stdout,"Pid: %d\n", getpid());
    dining(argc, argv);
    printf("pid: %d \npgid: %d\n", getppid(),getpgid(0));
    return (EXIT_SUCCESS);
}

/*Validate that parameters entered fit the requirements
  Only 3 total parameters and position must be within size of seats*/
void validParams(int argc, char** argv){
    if(argc < 3){
        fprintf(stderr, "Error: Not enough arguments,");
        fprintf(stderr, "number of seats and current position needed.\n");
        exit(EXIT_FAILURE);
    } else if(argc > 3){
        fprintf(stderr, "Error: Too many arguments,");
        fprintf(stderr, "only number of seats and current position needed\n");
        exit(EXIT_FAILURE);        
    } else{   
        if(atoi(argv[2]) > atoi(argv[1])){
           printf("Philosopher position greater than number of total seats\n");
           exit(EXIT_FAILURE);
        }
    }
}

/*Begin Dining*/
void dining(int argc, char** argv){
    int seats = atoi(argv[1]);
    int pos = atoi(argv[2]);
    int i;
    int fd = shm_open(argv[2], O_RDWR | O_CREAT, 0666);
    sem_t * chopsticks[seats]; 
    sem_t * temp;
    
      
    if (ftruncate(fd, sizeof(*temp)*seats) != 0) {
        fprintf(stderr, "Failed to resize the memory object\n");
        exit(EXIT_FAILURE);
    }
    
    mmap(NULL, sizeof(*temp), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    
    if(temp == MAP_FAILED) {
        fprintf(stderr, "Error: Failed to mmap() the memory object\n");
	exit(EXIT_FAILURE);
    }
    /*After finishing mapping you can unlink*/
    if (shm_unlink(argv[2]) < 0) {
        fprintf(stderr, "Error: failed to unlink the memory object\n");
    }

    initGpid();
    signal(SIGTERM, sigHandler);
    for(i = 0; i < seats; i++){	
            if(sem_init(chopsticks[i], 1, 1) != 0){
                    fprintf(stderr,"Error: Failed to initialize semaphore\n");
            }
    }
    do{
        /*sem_wait(chopsticks[pos]);
        sem_wait(chopsticks[(pos+1) % seats]);*/
        eat(pos);
        /*sem_post(chopsticks[pos]);
        sem_post(chopsticks[(pos+1) % seats]);*/
        think(pos);
        fullCycles++;
    }while(!Finished);
}

/*Philosopher "eats" for random time*/
void eat(int p){
    randTime();
    fprintf(stdout, "Philosopher #%d is eating\n", p);
}

/*Philosopher "thinks" for random time*/
void think(int p){
    randTime();
    fprintf(stdout, "Philosopher #%d is thinking\n", p);
}

void initSem(int seats, sem_t *S){
    int semCount = 1;
    char semNames[12];
    
    // 0666 = rw-rw-rw
    for(semCount = 1; semCount < seats+1; semCount++){
    sprintf(semNames, "%d", semCount);
    S = sem_open(semNames, O_CREAT|O_EXCL, 0666, 1);
    }
}

void initGpid(){
    setpgid(getpgrp(),0);
}

void sigHandler(int sig){
    Finished = 1;
}

//remove philospher from eat/think cycle and deallocate system resources
void finishEating(int pos, sem_t chop[], char* sem){
	sem_close(&chop[pos]);
	/*shm_unlink(sem);*/
	sem_destroy(&chop[pos]);
}

/*Waits for a random amount of time, no greater than 1,000,000*/
void randTime(){
    useconds_t usec = rand() % 1000000;
    if(usleep(usec) == -1){
        fprintf(stderr, "usleep function failed to run\n");
        exit(EXIT_FAILURE);
    }
}