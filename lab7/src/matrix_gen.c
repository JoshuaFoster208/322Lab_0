#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void createMatrix(int size);

int main(int argc, char ** argv) {
    argc = argc;
    int size; 
    size = strtol(argv[1], NULL, 10);//grab the int from parameter one
    createMatrix(size);
    return (EXIT_SUCCESS);
}
/*creates the matrix with the random numbers of size size by size
 spaces out the columns to be 4 bytes/chars to be able to hold -100 might make 
 * it 5 to add more space now that ive seen how it looks*/
void createMatrix(int size) {

    int randomVar;
/*Random seed*/
    srand(time(NULL));
    
    
    for (int i = 0; i < size; i++) {//row
        for (int j = 0; j < size; j++) {//col
            //rand mod 101 randomly generates a number from 0 to 100
            randomVar = rand() % 101;
            if(rand() % 2)//if odd negative number
               randomVar = randomVar * -1; 
            
            fprintf(stdout, "%4d", randomVar);
        }
        fprintf(stdout, "\n");//skip line to space
    
}

