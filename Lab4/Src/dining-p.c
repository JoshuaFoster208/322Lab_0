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

#define pGroupFile "PGROUP.txt"
sem_t * returnVal;
static int fullCycles = 0;
static int Finished = 0;

struct chpInfo {
    int seat;
    int pos;
    sem_t **chopstick;
    //added for part 2 to create a critical section
    sem_t *HnWChopstick;
};

void validParams(int argc, char** argv);
void eat(struct chpInfo *ch);
void think(struct chpInfo *ch);
void randTime();
void dining(int argc, char** argv, struct chpInfo *ch);
void sigHandler(int sig);
void initSem(char **argv, struct chpInfo *ch);
void initGpid();
void finishEating();
void sigSet();
int holdAndWait(struct chpInfo *ch, int j);

int main(int argc, char** argv) {
    struct chpInfo I; 
    validParams(argc, argv);
    sigSet();
    initGpid();
    fprintf(stdout,"Pid: %d\n", getpid());
    dining(argc, argv, &I);
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

void sigSet(){
    if(signal(SIGTERM, sigHandler) == SIG_ERR){
        fprintf(stderr, "Error: Could not register signal!\n");
        exit(EXIT_FAILURE);
    }
}

/*Begin Dining*/
void dining(int argc, char** argv, struct chpInfo *ch){
    initSem(argv, ch);
    do{
         if (holdAndWait(ch, 1) == 1) {
            eat(ch);
            think(ch);
            fullCycles++;
         }
    }while(!Finished);
            
   /* int seats = atoi(argv[1]);
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
    //After finishing mapping you can unlink
    if (shm_unlink(argv[2]) < 0) {
        fprintf(stderr, "Error: failed to unlink the memory object\n");
    }

    initGpid();
    for(i = 0; i < seats; i++){	
            if(sem_init(chopsticks[i], 1, 1) != 0){
                    fprintf(stderr,"Error: Failed to initialize semaphore\n");
            }
    }
    */
}

/*Philosopher "eats" for random time*/
void eat(struct chpInfo *ch){
    randTime();
    fprintf(stdout, "Philosopher #%d is eating\n", ch->pos);
}

/*Philosopher "thinks" for random time*/
void think(struct chpInfo *ch){
    randTime();
    fprintf(stdout, "Philosopher #%d is thinking\n", ch->pos);
}

void initSem(char **argv, struct chpInfo *ch){
    int i = 0;
    char chops[40];
        ch->seat = atoi(argv[1]);
        ch->pos = atoi(argv[2]);
        ch->chopstick = malloc(sizeof(sem_t*) *ch->seat);
        for (i = 1; i < ch->seat; i++) {
            //fill array/buffer before copying chopstick number into it
            memset(chops,0,40);
            sprintf(chops, "Chopstick%d", i);
            ch->chopstick[i] = sem_open(chops, O_CREAT, 0666, 1);
        }
        //added for part 2 to create a critical section
        ch->HnWChopstick = sem_open("allocate", O_CREAT, 0666, 1);
}

void initGpid(){
   FILE *pnt ; 
   struct stat buffer;
    
    /* Check if file PGROUP.txt currently exists */
    if (stat(pGroupFile, &buffer) == 0){
        //buffer for getline
        char *b = NULL;
        size_t size = 0;
        //PGROUP.txt already exists open it to be read with "r"
        pnt = fopen(pGroupFile, "r");
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
        pnt = fopen(pGroupFile, "w");
        sprintf(pid, "%d", gpid);
        fwrite(pid, sizeof(char), sizeof(pid), pnt);
        fclose(pnt);
    }
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

int holdAndWait(struct chpInfo *ch, int j) {
    if (j == 1) {
        int x;
        if((sem_trywait(ch->HnWChopstick) != -1))
            x = 1;
        else
            x = 0;
        return x;
    
    }
    else if (j == 0) {

        sem_post(ch->HnWChopstick);
        return 1;
    }
    return 1;
}