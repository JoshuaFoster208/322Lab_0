#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
void calcInfo(unsigned long int address, unsigned long int *offset, 
              unsigned long int *pageNum); 
void validParams(int argc);
void output(char** argv);

int main(int argc, char** argv) {
    validParams(argc);
    output(argv);
    return (EXIT_SUCCESS);
}

/*Calculates and passes the offset and pagenumber from the address. 
Uses unsigned long int as it must be a 32 bit datatype*/
void calcInfo(unsigned long int address, unsigned long int *offset, 
              unsigned long int *pageNum){
    *offset = address % 4096;
   *pageNum = address / 4096;
}

void validParams(int argc){
    if(argc < 2){
        fprintf(stderr, "Error: Not enough arguments\n");
        fprintf(stderr, "Only 1 argument needed: pass in address to calculate.\n");
        exit(EXIT_FAILURE);
    } else if(argc > 3){
        fprintf(stderr, "Error: Too many arguments\n ");
        fprintf(stderr, "Only 1 argument needed: pass in address to calculate.\n");
        exit(EXIT_FAILURE);        
    }
}

void output(char** argv){
    unsigned long int add, off, page;
    add = atoi(argv[1]);
    calcInfo(add, &off, &page);
    fprintf(stdout, "The address %lu contains:\n", add);
    fprintf(stdout, "page number = %lu\n", page);
    fprintf(stdout, "offset = %lu\n", off);
}