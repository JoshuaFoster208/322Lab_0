#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
void calcInfo(unsigned long int address, unsigned int *offset, 
              unsigned int *pageNum); 

int main(int argc, char** argv) {

    return (EXIT_SUCCESS);
}

/*Calculates and passes the offset and pagenumber from the address. 
Uses unsigned long int as it must be a 32 bit datatype*/
void calcInfo(unsigned long int address, unsigned long int *offset, 
              unsigned long int *pageNum){
    offset = address % 4096;
    pageNum = offset / 4096;
}