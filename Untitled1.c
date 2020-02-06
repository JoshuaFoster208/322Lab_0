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
        Bool atEnd = false;
        if(*str != 0){ //base case nonempty string
            while(!atEnd){
                if(str[length] !='/0')
                length++;
                else{
                atEnd = true;
                ret = length;
                }
            }
        }
        else{
        ret = 0;
        }
    return ret;
    }

    void splitVal(char* str, char Hold, int size)
    {
        int aPtr =0, int i;
        bool less = false
        while(aptr < size){
            for(i=0; i<8; i++){
                if(str[aPtr] != '\0')
                Hold[i] = str[aPtr++];
                else{
                i=10;
                less = true;
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
        Bool atEnd = false;
        if(*str != 0){ //base case nonempty string
            while(!atEnd){
                if(length > 7)
                atEnd = true;
                else if(str[length] !='/0')
                length++;
                else{
                add = 7 - length;
                str[8] = '\0';
                for(i = add; i > 0; i--)
                str[length++] = 0;
                }
            }
        }
     }

     int binToDec(char* str)
     {
         int x = 32, sum = 0, i;
         for(i = 1; i < 8; i++)
         {
          if(str[i] == '1')
          sum += x;
          x = x/2;
         }
         return sum;
     }

     bool parIsEven(char* str)
     {
         bool even = true;
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
             even = false;
         }
         return even;
     }

     printLine(char* str)
     {
         int i, Dec;
         bool Par;
         char Parity[5];
         Dec = binToDec(str);
         Par = parIsEven(str);
         if(Par)
        Parity = "EVEN";
          else
        Parity = "ODD"
         printf("%s \t%c \t%d %s\n", str, char(Dec), Dec, Parity);
     }

    int main(int argc, char* argv[]) {
        FILE* fPtr;
        char str[100];
        char temp[15];
        char* fName = "c:\\Users\\josht\\OneDrive\\Documents\\Docs\\test.txt";
        int vLength = 0;
        if ((fPtr = fopen(fName, "r")) == NULL) {
            printf("Error file failed to open");
            // Program exits if the file pointer returns NULL.
            exit(1);//add different file name
            //01011101 10110111 11101011 111101
            printChart();
            while(fPtr != EOF){
                fscanf(fPtr, "%s", str);
                vLength = findLength(str, sizeof(str));
                if(vLength > 7)
                    splitVal(str, temp, vLength)
                else if(vLength < 7){
                    pad0(str, sizeof(str);
                    printLine(str);
                }
                else{
                printLine(str);
                }






            }
            }
    return 0;
        }
