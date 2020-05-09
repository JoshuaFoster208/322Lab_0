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
    char *buf;
    size = strtol(argv[1], &buf, 10);
    createMatrix(size);
    return 0;
}


/**
 * @brief generate_matrix - Prints matrix contents to stdout 
 *
 * @param size - Size for SizeXSize matrix
 */
void createMatrix(int size) {

    int randomVar;

    srand(time(NULL));
    
    /* Generate matrix one column at a time. Each column is 4 bytes,
     * being the maximum amount of characters a number -100 to 100 can
     * occupy. The matrix_add program will need to parse numbers 4 on 
     * intervals of 4 bytes each */
    for (int i = 0; i < size; i++) {//row
        for (int j = 0; j < size; j++) {//col
            //rand mod 101 randomly generates a number from 0 to 100
            randomVar = rand() % 101;
            if(rand() % 2)//if odd rand number
               randomVar = randomVar * -1; 
            
            fprintf(stdout, "%4d", randomVar);
        }
        fprintf(stdout, "\n");
    }
    
}

