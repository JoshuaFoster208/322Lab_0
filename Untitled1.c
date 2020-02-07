#include <stdio.h>
#include <stdlib.h>

/*
 *
 */
    void printChart()
    {
        printf("Original ASCII    Decimal  Parity  \n");
        printf("-------- -------- -------- --------\n");
    }

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

    void splitVal(char* str, char* Hold, int size)
    {
        int aPtr =0, i;
        int less = 0;
        while(aPtr < size){
            for(i=0; i<8; i++){
                if(str[aPtr] != '\0')
                Hold[i] = str[aPtr++];
                else{
                i=10;
                less = 1;
                }
            }
            if(less)
                Pad0(Hold,sizeof(Hold));
           printLine(Hold);
        }

    }

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

    int main(int argc, char* argv[]) {
        FILE* fPtr;
        char str[100];
        char temp[15];
        char fName[255];
        int vLength = 0;
        strcpy(fName, "c:\\Users\\josht\\OneDrive\\Documents\\Docs\\test.txt");
        if ((fPtr = fopen(fName, "r")) == NULL) {
            printf("Error file failed to open");
            // Program exits if the file pointer returns NULL.
            exit(1);//add different file name
            //01011101 10110111 11101011 111101
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
            
    return 0;
        }
