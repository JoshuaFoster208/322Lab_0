#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <aio.h>
#include <string.h>

#define IN STDIN_FILENO
#define OUT STDOUT_FILENO
int count=0;
/*I gotta say the linux manual page helped with this project more than any of 
 the others besides maybe lab4*/

void run(int argc, char **argv);
void matrix_add(int size, int scalar, struct aiocb *data);
void setIn(off_t offset, int displacement, struct aiocb *data);
void setOut(off_t offset, struct aiocb *data);
void outputTime(int sTime);

int main(int argc, char **argv) {
    run(argc, argv);
    return (EXIT_SUCCESS);
}

void run(int argc, char **argv){
    argc = argc;
    int startTime, scalar, size, blocks, blockSize, offset, displacement; 
    char *buf;
    struct aiocb last, current, next;
    
    /*Setup necessary variables*/
    startTime = time(NULL);
    srand(time(NULL));//changes the random seed

    //printf("%d\n", startTime);
    size = strtol(argv[1], &buf, 10);
    blocks = strtol(argv[2], &buf, 10);    
    scalar = rand() % 101;/*0-100 makes it so that it should have a mix of 
    * positive and negative numbers after the addition*/
    if(time(NULL) % 2)
        scalar = scalar * -1;// if rand is odd make negative scalar
    displacement = 0;
    blockSize = size / blocks;
//verify if parameters divide evenly        
     if (size % blocks) {
        fprintf(stderr, "Error: Invalid Entries(line 60)\n");
        exit(EXIT_FAILURE); 
    }    
    //4 bytes per col based on max size of value and ^2 for the part 2 for loop
    offset = (blockSize*4);
    offset = offset*offset;
    
    setIn(offset, displacement, &current);

    //call aio_read after the setup
    aio_read(&current);   
    
    //wait for read to finish
    //cant use suspend aio_suspend(&first, ,NULL); so switched to while
    while(aio_error(&current) == EINPROGRESS){
    }
    //retrieve the aiocb completion now that the i/o op is complete
    aio_return(&current);
    //loop throught the full size of the new space
     for (displacement = offset; displacement < (size*size)*4; 
             displacement = displacement + offset) {
       
    /*repeat steps to setup read from before reminder come back and make it a 
     * function if this works: it did work added setIn and setOut*/
    setIn(offset, displacement, &next);
  
    //call read on the next struct
    aio_read(&next);
        
    /*wait for read I've determined it can work without this but this is a good 
    practice to have so I kept it in*/
    while(aio_error(&next) == EINPROGRESS){
    }

    //add the scalar
    matrix_add(size, scalar, &current);
    /*move the scalared values from current to last*/
    memcpy(&last, &current, sizeof(struct aiocb));

    // sets up last for a write call this time  
    setOut(offset, &last);

    aio_write(&last);

    //wait again but for the write call
    while (aio_error(&last) == EINPROGRESS){
    }
    aio_return(&last);
        
    //it is only a request so you have to check if it failed to sync
    if(aio_fsync(O_SYNC, &last) == -1){
        fprintf(stderr,"Error: last failed to sync (line 99)\n");
    }

    /* now that current is free copy over the next struct
     *  might switch to safer memmove*/
    memcpy(&current, &next, sizeof(struct aiocb));
    }

    //do the last struct
    matrix_add(size, scalar, &current);
    //finish the current struct
    setOut(offset, &current);
    aio_write(&current);
    while (aio_error(&current) == EINPROGRESS){};
    aio_return(&current);
    //only a request have to check if it failed to sync
    if(aio_fsync(O_SYNC, &current) == -1){
        fprintf(stderr,"Error: current failed to sync (line 116)\n");
    }
    outputTime(startTime);
}

void matrix_add(int size, int scalar, struct aiocb *data){
    //unsigned for negative values
    unsigned int holdNum;
    //holds the 4 bytes plus the '\0' 
    char buf[5];
    /* loop through the buffer in data while i < number of bytes
     * increment by 4 to account for the size
     * switch to unsigned int i to cover comparison issues*/
    for (unsigned int i = 0; i < data->aio_nbytes; i = i + 4) {
        // increment the counter that stands for total number of loops
        count++;
/*cant just reset the first value the way you would normally reset strings in c:
 *  reset the whole string so that the following numbers copied dont overlap 
 * such as in the case of the first number being -100 and the second being 1 
 * you would get -101*/
        memset(buf, '\0', sizeof(buf));
/* move 4 bytes from the buffer
 * add i to the address as it loops to get the next number
 * reminder to look up what this technique was called in assembly*/
        memmove(buf, (void *)data->aio_buf+i, 4);
        //atoi couldnt match types passed in, strtol neccesary
        holdNum = strtol(buf, NULL, 10) + scalar;
        //print to the file
        fprintf(stdout, "%4d", holdNum);
        //increment count if count %size is 0 skip line 
        //whenever count %size is 0 output a line space to format the file 
        if(count%size == 0){
           fprintf(stdout, "\n");
        }
    }
}

/*sets up the aiocb for aioread calls*/
void setIn(off_t offset, int displacement, struct aiocb *data){
  //zero out before the aioreadrequest so that nothing can interupt it
    memset(data, 0, sizeof(struct aiocb));
    //set the fd to IN macro for read
    data->aio_fildes = IN;
    //set the bytes per block size
    data->aio_nbytes = offset;
    //set the offset
    data->aio_offset = displacement;
    /*missed these at first 
     * need to allocate space for read operations
     * have to set a priority 
     * */
    data->aio_buf = malloc(offset);
    /*not sure if necessary but all examples I've seen have had them
     * to be safe set all to 0*/
    data->aio_reqprio = 0;   

}

/*sets up the aiocb for aiowrite calls*/
void setOut(off_t offset, struct aiocb *data){
/*a classmate told me that I shouldnt be zeroing out stdout, 
 * I'm gonna look into why but for now it works*/
    //set the fd to OUT macro for write
    data->aio_fildes = OUT;
    //set the bytes per block size
    data->aio_nbytes = offset;
    //set the offset
    data->aio_offset = 0;
    /*missed this at first 
     * have to set a priority */
    data->aio_reqprio = 0;   
}

/*outputs the start and endtimes to the end of the file*/
void outputTime(int sTime){
    fprintf(stdout, "\nstart time %d - end time %ld\n", sTime, time(NULL));
}