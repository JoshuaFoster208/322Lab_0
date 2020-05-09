
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>

int main(int argc, char **argv){
    char oldPath[FILENAME_MAX];
    struct passwd *pw;
    //grabs current working directory path
    getcwd(oldPath, sizeof(oldPath));
    pw=getpwent();
    chdir(pw->pw_dir);

    FILE * filePointer = fopen("lab6.log", "a");
    
            printf("bye%s\n", argv[0]);

    if (filePointer == NULL)        
        fprintf(stderr, "Error message: No log file created");
    else    
        fprintf(filePointer, "Pop mole%s\n", argv[1]);
        fclose(filePointer);
        chdir(oldPath);
    return 0;
}