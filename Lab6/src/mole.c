#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>

int main(int argc, char **argv){
//these warning messages are very annoying
argc = argc;    

//grabs current working directory path
//Online claimed that it wasnt always safe to do it this way but was fine for this
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
        if (pw == NULL) {
            fprintf(stderr, "Error:, Failed to reteive home directory\n");
            exit(EXIT_FAILURE);
        }
//switch to the home directory
    chdir(pw->pw_dir);

    FILE * filePointer = fopen("lab6.log", "a");
    if (filePointer == NULL)        
        fprintf(stderr, "Error message: No log file created");
    else    
        fprintf(filePointer, "Pop mole%s\n", argv[0]);
    fclose(filePointer);
    chdir("/");
    return (EXIT_SUCCESS);
}