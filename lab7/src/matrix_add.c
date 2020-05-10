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
void matrix_add(int size, int block, int scalar);
int main(int argc, char **argv) {
    run(argc, argv);

    return (EXIT_SUCCESS);
}

void run(int argc, char **argv){
    argc = argc;
    int startTime, scalar, size, blocks, blockSize, offset; 
    char *buf;
    struct aiocb first;
    
    /*Setup necessary variables*/
    startTime = time(NULL);
    printf("%d", startTime);
    size = strtol(argv[1], &buf, 10);
    blocks = strtol(argv[2], &buf, 10);    
    scalar = rand() % 101;
    scalar = scalar;
    blockSize = size / blocks;
    
    //zero out before the aioreadrequest so that nothing can interupt it
    memset(&first, 0, sizeof(struct aiocb));
    //4 bytes per col based on max size of value
    offset = (blockSize*4)*2;
    //set the fd to in for read
    first.aio_fildes = IN;
    //set the bytes per block size
    first.aio_nbytes = blockSize;
    //set the offset
    first.aio_offset = offset;
    /*missed these at first 
     * need to allocate space for read operations
     * have to set a priority even if said priority wont matter in this example
     * */
    first.aio_buf = malloc(blockSize);
    first.aio_reqprio = 0;
    //call aoi_read after the setup
    aio_read(&first);    
    aio_return(&first);
    
       // matrix_add(size, block, scalar);

}

void matrix_add(int size, int block, int scalar){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            //block[i][j] += scalar;
            scalar = scalar;
            block = block;

        }
    }
}