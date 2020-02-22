#include <stdio.h>
#include <stdlib.h>

/*
 *
 */
//Prints the top of the chart
    void printChart(){
        printf("Original ASCII    Decimal  Parity  \n");
        printf("-------- -------- -------- --------\n");
    }

    //Function to find the length of the scanned input
    //passes in string and size of string in bytes
    int findLength(char* str, size_t str_length)
    {
        int length = 0, add = 0, ret;
        int atEnd = 0;
        if(str[0] != '\0'){ //base case nonempty string
            while(!atEnd){
                if(str[length] !='\0')
                length++;
                else{
                atEnd = 1;
                ret = length;
                }
            }
        }
        else{
        ret = 0;
        }
    return ret;
    }

    //Takes strings greater than 8 in size 
    //and splits them then outputs it
    void splitVal(char* str, char* Hold, int size)
    {
        int aPtr =0, i;
        int less = 0;
        while(aPtr < size){
            for(i=0; i<8; i++){
                if(str[aPtr] != '\0')
                Hold[i] = str[aPtr++];
                else{
                less = 1;
                Hold[i] = '\0';
                i=10;
                }
            }
            if(less)
                Pad0(Hold,sizeof(Hold));
           printLine(Hold);
        }

    }

    //adds 0s to the right side of the string until it is 8 digits
    //in length
     void Pad0(char* str, size_t str_length)
     {
        int length = 0, add = 0, i;
        int atEnd = 0;
        if(*str != 0){ //base case nonempty string
            while(!atEnd){
                if(length > 7)
                atEnd = 1;
                else if(str[length] !='\0')
                length++;
                else{
                //add = 8 - length;
                str[8] = '\0';
                for(i = length; i < 8; i++)
                str[i] = '0';
                }
            }
        }
     }
//converts an 8 digit string of 1s and 0s
//into binary
     int binToDec(char* str)
     {
         int x = 64, sum = 0, i;
         for(i = 1; i < 8; i++)
         {
          if(str[i] == '1')
          sum += x;
          x = x/2;
         }
         return sum;
     }

//essentially a boolian function to check if number of 1s
//are even or not
     int parIsEven(char* str)
     {
         int even = 1;
         int numOnes = 0, i;
         for(i = 0; i <8; i++)
         {
             if(str[i] == '1')
             {
                 numOnes++;
             }
         }
         if(numOnes % 2)
         {
             even = 0;
         }
         return even;
     }

     //prints the output for an 8 digit string
     //in the table format
     printLine(char* str)
     {   
         int i, Dec;
         int Par;
         char Parity[5];
         Dec = binToDec(str);
         Par = parIsEven(str);
         if(Par)
            strcpy(Parity, "EVEN");
         else
            strcpy(Parity, "ODD");
              printf("%s\t%c\t%d %s\n", str, Dec , Dec, Parity);
     }

    //main function
    int main(int argc, char* argv[]) {
        //argc length of argv number of params
        //
        FILE* fPtr;
        char str[100];
        char temp[9];
        char fName[255];
        int vLength = 0;
        if(argc == 1 || argv[1] == '-')//no file name
        {
          printf("Please enter file name or a path to the file\n"); 
          scanf("%s", &fName);
        }
        else{
            strcpy(fName, argv[1]);
        }
        if ((fPtr = fopen(fName, "r")) == NULL) {
            printf("Error file failed to open");
            // Program exits if the file pointer returns NULL.
            exit(1);
        }
            printChart();
            while(fscanf(fPtr, "%s", str) != EOF){

                vLength = findLength(str, sizeof(str));
                if(vLength > 8)
                    splitVal(str, temp, vLength);
                else if(vLength < 8){
                    Pad0(str, sizeof(str));
                    printLine(str);
                }
                else{
                printLine(str);
                }
            }
            fclose(fPtr);
    return 0;
        }
