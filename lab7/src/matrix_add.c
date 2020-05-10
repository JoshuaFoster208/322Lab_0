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

/*I gotta say the linux manual page helped with this project more than any of 
 the others besides maybe lab4*/

/* for reference
 * http://man7.org/linux/man-pages/man7/aio.7.html
 * struct aiocb {
     The order of these fields is implementation-dependent 

   int             aio_fildes;     // File descriptor 
    off_t           aio_offset;     // File offset 
    volatile void  *aio_buf;        // Location of buffer 
    size_t          aio_nbytes;     // Length of transfer 
    int             aio_reqprio;    // Request priority 
    struct sigevent aio_sigevent;   // Notification method 
    int             aio_lio_opcode; // Operation to be performed;
                                       lio_listio() only 
   /* Various implementation-internal fields not shown 
};*/

void run(int argc, char **argv);
void matrix_add(int size, int blocks, int scalar, struct aiocb *data);
int main(int argc, char **argv) {
    run(argc, argv);

    return (EXIT_SUCCESS);
}

void run(int argc, char **argv){
    argc = argc;
    int startTime, scalar, size, blocks, blockSize, offset; 
    char *buf;
    struct aiocb last, current, next;
    
    /*Setup necessary variables*/
    startTime = time(NULL);
    printf("%d", startTime);
    size = strtol(argv[1], &buf, 10);
    blocks = strtol(argv[2], &buf, 10);    
    scalar = rand() % 101;
    scalar = scalar;
    blockSize = size / blocks;
    
    //zero out before the aioreadrequest so that nothing can interupt it
    memset(&current, 0, sizeof(struct aiocb));
    //4 bytes per col based on max size of value
    offset = (blockSize*4)*2;
    //set the fd to in for read
    current.aio_fildes = IN;
    //set the bytes per block size
    current.aio_nbytes = blockSize;
    //set the offset
    current.aio_offset = offset;
    /*missed these at first 
     * need to allocate space for read operations
     * have to set a priority even if said priority wont matter in this example
     * */
    current.aio_buf = malloc(blockSize);
    current.aio_reqprio = 0;
    //call aoi_read after the setup
    aio_read(&current);    
    //wait for read to finish
    //cant use suspendaio_suspend(&first, ,NULL);
    while(aio_error(&current) == EINPROGRESS){
    };

    //retrieve the aiocb information now that the i/o op is complete
    aio_return(&current);
       
     for (int i = offset; i < (size*4)*2; i += offset) {
    /*repeat steps above reminder come back and make it a function if this works*/
    
    //zero out before the aioreadrequest so that nothing can interupt it
    memset(&next, 0, sizeof(struct aiocb));
    //4 bytes per col based on max size of value
    offset = (blockSize*4)*2;
    //set the fd to in for read
    next.aio_fildes = IN;
    //set the bytes per block size
    next.aio_nbytes = blockSize;
    //set the offset
    next.aio_offset = offset;
    /*missed these at first 
     * need to allocate space for read operations
     * have to set a priority even if said priority wont matter in this example
     * */
    next.aio_buf = malloc(blockSize);
    next.aio_reqprio = 0;        
        /* Execute read on the next block */
        aio_read(&next);
        
        /* See busy wait justification above in first call to aio_error() */
        while(aio_error(&next) == EINPROGRESS){
        };

        /* Add scalar to last read block */
    matrix_add(size, blocks, scalar, &current);

        /*move the scalared values from current to last so that last can 
         * write the info*/
        memcpy(&last, &current, sizeof(struct aiocb));

        // sets up last for a write call this time 
    //zero out before the aioreadrequest so that nothing can interupt it
    memset(&last, 0, sizeof(struct aiocb));
    //4 bytes per col based on max size of value
    offset = (blockSize*4)*2;
    //set the fd to in for read
    last.aio_fildes = OUT;
    //set the bytes per block size
    last.aio_nbytes = blockSize;
    //set the offset
    last.aio_offset = offset;
    /*missed these at first 
     * have to set a priority even if said priority wont matter in this example
     * */
    last.aio_reqprio = 0;   
        aio_write(&last);
        
        //wait again but for the write call
        while (aio_error(&last) == EINPROGRESS){
        };
        aio_return(&last);
        /* Sync the write output to disk */
        aio_fsync(O_SYNC, &last);

        /* Overwrite the current block that has already been copied to 'last' and
         * written to stdout, with the next block prepped for aio_reads */
        memcpy(&current, &next, sizeof(struct aiocb));
    }

    /* Last drain the pump by handling the last block */
    matrix_add(size, blocks, scalar, &current);

    /* Write the last 'current' block to stdout */
    //zero out before the aioreadrequest so that nothing can interupt it
    memset(&last, 0, sizeof(struct aiocb));
    //4 bytes per col based on max size of value
    offset = (blockSize*4)*2;
    //set the fd to in for read
    last.aio_fildes = OUT;
    //set the bytes per block size
    last.aio_nbytes = blockSize;
    //set the offset
    last.aio_offset = offset;
    /*missed these at first 
     * have to set a priority even if said priority wont matter in this example
     * */
    last.aio_reqprio = 0;       aio_write(&current);
    while (aio_error(&current) == EINPROGRESS){};
    aio_return(&current);
    //only a request have to check if it failed to sync
    if(aio_fsync(O_SYNC, &current) == -1){
        fprintf(stderr,"Error: current failed to sync (line 156)\n");
    }

}

void matrix_add(int size, int blocks, int scalar, struct aiocb *data){
    //unsigned for negative values
    unsigned holdNum;
    //holds the 4 bytes plus the '\0' 
    char* buf1[5], buf2[5];
    /* loop thorugh the buffer in data while i < number of bytes
     * increment by 4 to account for the size
     * switch to unsigned int i to cover comparison issues*/
    for (unsigned int i = 0; i < data->aio_nbytes; i = i + 4) {
        /*from what I read online memset may be overkill: reset the string 
         * so that the following numbers copied dont overlap
         * such as in the case of the first number being -100 
         * and the second being 1 you would get -101*/
        memset(buf1, '\0', sizeof(buf1));
        memset(buf2, '\0', sizeof(buf2));

        /* move 4 bytes from the buffer
         * add i to the address it as it loops to get the next number*/
        memmove(buf1, (void *)data->aio_buf+i, 4);
        
        holdNum = atoi((void*)buf1) + scalar;

        sprintf(buf2, "%4d", holdNum);

        // replace the value in the buffer
        memmove((void *)data->aio_buf+i, buf2, 4);
        
        
        
    }
}