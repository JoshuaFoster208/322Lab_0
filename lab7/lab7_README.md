# 322Lab7
Current status of the project
Created and tested occasional segfault error
## Output
### matrix_gen
![output](https://media.discordapp.net/attachments/636118401080885259/708954928583933973/gen.PNG)
### matrix_add
![output](https://media.discordapp.net/attachments/636118401080885259/709522703589179552/somewhat_works.PNG?width=1223&height=359)
## Log
1. May 9th
   - Create Lab7
     - create matrix_gen.c
   - Added matrix_add.c
     - setup the process for aio_read and aio_return
   - Removed errors and warnings
     - currently looking for makefile error with aio_read
   - Removed Makefile error with inclusion of -lrt library
   - Setup Matrix_add for part2
   - alter function calls in matrix_add, casted (void *)
   - pushed a rough draft of the full projects
   - fixed segmentation faults
   - altered to work better with the call format
2. May 10th
   - Change many small things program works relatively correctly
     - Changed print to format the lines
     - printed the start and end time to the file
     - added and removed comments that were to remind me to do things and the reference paragraph at the top
     - several other small things
    - smoothed out comments and made the scalar range be from -100 to 100 so that there will be a mix of postiives and negatives in the file
    - added back a closing bracket from matrix_gen
3. May 11th
   - Made many more changes to attempt to fix an error and to format better
     - Added 4th struct specifically for output/line spaces
     - Formatted and heavily commented what I coded with my thought process to hopefully get partial credit
     - added functions
     - updated readMe
     - many other small changes/improvements I noticed after finally having some sleep
     - to all those who clone my repository you don't want mine trust me it doesnt work exactly correctly lol
