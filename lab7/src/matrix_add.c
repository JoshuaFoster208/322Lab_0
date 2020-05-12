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
#include <errno.h>

//file descriptor passed that redirects (<)input
#define IN STDIN_FILENO
//the program writes to the file redirected by >
#define OUT STDOUT_FILENO
int count=0;
/*I gotta say the linux manual page helped with this project more than any of 
 the others besides maybe lab4, however there were no resources online that could
 help with the error I had. After calling aio_write in the while loop for 
 aio_error, when einprogress finished, instead of getting a 0 back for success
 the error message 38 that it was not implemented was returned. For the life of 
 me I could not see what to do about that error or what causes it, but I was able to
 fix it in the end but not sure how, Oh I also attempted to add a 4th struct that
 I could use to add line breaks to the second file for formatting as read doesnt 
 pick them up but didnt have time to get the buffer size and displacement offset
 to be exactly right, so now it sometimes has a segfault error if there was a 
 better way that you wouldnt mind messaging me I'd love to know thanks professor*/

void run(int argc, char **argv);
void matrix_add(int size, int scalar, struct aiocb *data, struct aiocb *output);
void setIn(off_t offset, int displacement, struct aiocb *data);
void setOut(off_t offset,int displacement, struct aiocb *data);
void outputTime(int sTime);
int setOutput(int blockSize, int offset, int size, struct aiocb *output);

int main(int argc, char **argv) {
    run(argc, argv);
    return (EXIT_SUCCESS);
}

/*runs the whole program, I like to keep main from having declarations or code
 other than function calls this time its only run though*/
void run(int argc, char **argv){
    argc = argc;
    int startTime, scalar, size, blocks, blockSize, offset, errnum; 
    int displacement, displacement2, bool;
    char *buf;
    struct aiocb last, current, next, output;
    
//Sets up the starting time and sets a random seed
    startTime = time(NULL);
    srand(time(NULL));
    
//retrieve the two parameters size and blocks from argv
    size = strtol(argv[1], &buf, 10);
    blocks = strtol(argv[2], &buf, 10);    
    
//if the second parameter is 0 exit as the block size has to exist
    if(blocks == 0){
       fprintf(stderr, "Error: blocks cannot be 0 (line 63)\n");
       exit(EXIT_FAILURE); 
    }
    
//Setup a random scalar in the range from -100 to 100
//Limited it to keep the range relatively small, so that i can see if negatives 
//were behaving correctly, and so that the columns stayed aligned
    scalar = rand() % 101;
    if(time(NULL) % 2)
        scalar = scalar * -1;// if rand is odd make negative scalar

//Displacement will be used to represent the aoicb_offset 
//I made another variable with the name offset before implementing this
//Was gonna go back and update but ran out of time
//displacement2 keeps track fo the offset for the write to account for the 
//added new lines used for formatting
    displacement = 0;
    displacement2 = 0;
//blockSize is the size of the blocks before the change to one for loop
//It is multiplied by 4 as the input files' columns are spaced by 4 for each number
    blockSize = size / blocks;
    blockSize = blockSize * 4;
    
//verify if parameters divide evenly, exits upon failure  
     if (size % blocks) {
        fprintf(stderr, "Error: Invalid Entries(line 88)\n");
        exit(EXIT_FAILURE); 
    }    
    
//4 bytes per col based on max size of value and ^2 for the part 2 for loop
offset = blockSize*blockSize;

/*
 Prime the pump as reccomended
 * setup current and read the first block
 */

//setup current struct for the initial read call
    setIn(offset, displacement, &current);

//call aio_read after the setup
    aio_read(&current);   
    

//originally attempted aio_suspend(&first, ,NULL); but ran into difficulties 
//Now loops until aio_error returns 0 or a postive val error message as required
    while(aio_error(&current) == EINPROGRESS){
    }
    
//retrieves the number of bytes successfully read 
    aio_return(&current);
  
/*/
 Main loop that should loop up until the final block
 */
   
//for loop will go for the total size of the size of the matrix passed in
//the size*size then *4 for the size of the columns
//displacement holds the value of how far we are offset in the original file
//displacement2 holds how far we should be displaced in the file being written too
//both are incremented by the offset which is the updated blockSize
    
//i now realize that this would make the first write be one block into the file
//displacement2 = offset;
for (displacement = offset; displacement < ((size*size)*4); 
             displacement = displacement + offset) {
       
 //setup next to read the next block in the file
    setIn(offset, displacement, &next);
  
 //begin reading the next block into the buffer setup_fi
    aio_read(&next);
 //bool holds whether or not to add a byte for the line skip
 //output will be used to hold the updated buffer for writing
     bool = setOutput(blockSize, offset, size, &output);
 
//adds the scalar to the matrix and stores the updated values and new lines if
//if necessary into the prepared output buffer
     matrix_add(size, scalar, &current, &output);

//copy output into the last aiocb struct for writing
//looking back output wasn't necessary i could have used last for this portion
    memcpy(&last, &output, sizeof(struct aiocb));
  
//if the I/O hasn't finished by the time addition is done then wait for it to finish
    while(aio_error(&next) == EINPROGRESS){
    }

// setOut will set up last for an aio_write call but will not replace the buffer
    setOut(offset, displacement2, &last);

//sends last to begin reading with a priority 0
    aio_write(&last);

//preps displacement2 based on if a new line was added to the new file
//resets the int boolean called before while to save time
if(bool){
        displacement2 = displacement2 + (offset+1);
        bool = 0;
    }else{
        displacement2 = displacement2 + offset;
        bool = 0;
    }
    
//loops until the I/0 is finished writing or at least until EINPROGRESS is no 
//longer being returned
//aio_return/aio_error, issue ,fixed but not sure how or why,where ENOSYS is 
//returned after EINPROGRESS. ENOSYS claims that the function is not implemented
//and I could find any resources to help fix the bug, majority of my final time
//was spent on this        
    while (aio_error(&last) == EINPROGRESS){
    }
    
    aio_return(&last);
 
//fsync isnt 100% necessary from what i have read online, however it does cover 
//in the case of a crash
//it is important to remember that it is only a request so you have to check if 
//it failed to sync
    if(aio_fsync(O_SYNC, &last) == -1){
        fprintf(stderr,"Error: last failed to sync (line 182)\n");
    }

//copy the next block that has finished reading to the current struct for when 
//it loops through again
    memcpy(&current, &next, sizeof(struct aiocb));
    }//end of for loop

/*
 drain the pump/finish the last block setup
 */

//setup the output one last time in case there is need of one more new line
//boolean doesn't matter now as there will be no more writes to consider after the
//current one
    setOutput(blockSize, offset, size, &output);
    
//add the scalar and store into output once again
    matrix_add(size, scalar, &current, &output);
    
//setup output for I/O writing
    setOut(offset, displacement2, &output);
    
//call aio_write on output
    aio_write(&output);
    
//nothing to put ahead this time so call wait once more.    
    while (aio_error(&output) == EINPROGRESS){};
    
//return the number of bytes written or a -1 for error
    aio_return(&output);
 
//sync the final block, still only a request so check for failure to sync
    if(aio_fsync(O_SYNC, &output) == -1){
        fprintf(stderr,"Error: current failed to sync (line 216)\n");
    }
    
//outputs the startTime, calculates the end time and outputs that as well
    outputTime(startTime);
}


/*Matrix Add is a function that is passed 2 structs and takes the numbers passed
 * from the buffer of one and adds a scalar to the values before storing in the 
 * second buffer*/
void matrix_add(int size, int scalar, struct aiocb *data, struct aiocb *output){
//signed for ability to hold negative values
    signed int holdNum, extra = 0;
//numBuf holds the 4 bytes plus the '\0' 
//lnSkipBuf never changes, holds the value for a new line in it and a '\0'
    char numBuf[5], lnSkipBuf[2] ={'\n'};
    
//loop through the buffer in data while i < number of bytes starting from 0,
//increment by 4 to account for the size of the values being copied 
//use signed int i to cover comparison issues
    for (unsigned int bytes = 0; bytes < data->aio_nbytes; bytes = bytes + 4) {
// increment the counter that stands for total number of loops/number of copied 
//values from file
         count++;
         
//Call memeset so that there is no overlapping of values in the strings copied 
//over. May be a bit overkill strings can normally be 'reset' by replacing the 
//first value to '\0'
         memset(numBuf, '\0', sizeof(numBuf));
         
//called memmove to be safe but have come to the conclusion that since we are not
//accessing the same memory at any point it may be unneccesary but better to be safe
//add bytes to the buffer to stand the current position in the buffer
        memmove(numBuf, (void *)data->aio_buf+bytes, 4);
        
//Convert the string to an int add the scalar and store in holdNum
//atoi couldn't match types passed in, so used strtol
        holdNum = strtol(numBuf, NULL, 10) + scalar;

//reset the numBuffer again to prepare to copy the string back into it
        memset(numBuf, '\0', sizeof(numBuf));
        
//convert the number back into numBuf
//formatted size to %4d to keep formatting consistent between files
        sprintf(numBuf, "%4d", holdNum);
        
//move the updated number from the buffer into the output struct
        memmove((void *)output->aio_buf + (bytes+extra), numBuf, 4);
        
//extra should be never be more than 1 but added as a precaution
//if the total number of loops / numbers written to the file is a mulitiple of 
//size, then increment extra and add a new line char to the buffer '\0'
//output was prepared specifically to have 1 more byte available, if count 
//would have reached that point during this matrix_add call
        if(count%size == 0){
            extra++;
        memmove((void *)output->aio_buf + (bytes + extra), lnSkipBuf, 1);
        }
    }
}

/*sets up the aiocontrol box struct for an aioread call
 * I originally set the off_t to a signed int value as that is what its is but was
 * getting errors from it (not sure why) so switched to an off_t*/
void setIn(off_t offset, int displacement, struct aiocb *data){
//Zero out before the aioreadrequest so that nothing else has a chance to interrupt
//The I/O read call
    memset(data, 0, sizeof(struct aiocb));
    
//Set the fd to IN macro, for the file descriptor setup in the original < call
    data->aio_fildes = IN;

//Offset holds the updated size of the blocks the name is confusing with more time 
//I'd go back and fix it
    data->aio_nbytes = offset;

//Displacement is the offset that says where in the file to read from
    data->aio_offset = displacement;

//Use malloc to allocate space for the buffer the size of the offset
    data->aio_buf = malloc(offset);

}

/*sets up the aiocontrol box struct for an aiowrite call
 * does not update the buffer as that is copied in during the run function*/
void setOut(off_t offset,int displacement, struct aiocb *data){
//Set the fd to OUT macro, for the file descriptor setup in the original > call
    data->aio_fildes = OUT;
    
//Offset holds the updated size of the blocks the name is confusing with more time 
//I'd go back and fix it
    data->aio_nbytes = offset;
    
//Displacement is the offset that says where in the file to write to
//would be unnecessary if the fd had O_APPEND but the original was not >>
    data->aio_offset = displacement;
    
}


/*outputs the start and endtimes to the file*/
void outputTime(int sTime){
//stdout writes to the file descriptor in macro OUT
    fprintf(stdout, "\nstart time %d - end time %ld\n", sTime, time(NULL));
}

/*seOutput checks if the next block to write will need a new line and calls 
 *setIn to match the size needed*/
int setOutput(int blockSize, int offset, int size, struct aiocb *output){
    int i=0;
//lockSize equivalent to the amount of numbers being added to the file per block
    blockSize = blockSize/4;
//If the remainder from the count%blocksize + blockSize is greater than or equal
//to the size then add 1 byte to the offset for room for the newLine
    if(((count%blockSize)+ blockSize) >= size){
        setIn(offset+1, 0, output);
//set i to won for the 'boolean' variable on return to be true
        i = 1;
    }
    else{
        setIn(offset, 0, output);
    }
    return i;
}