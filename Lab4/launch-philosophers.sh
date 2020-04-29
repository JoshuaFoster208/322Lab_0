#!/bin/bash
# Delete files if they already existed
rm -f PGROUP.txt
rm -f UNLINKED.txt

# Launch dining-p with sequential position
i=0
total=$1
while [ $i -lt $total ]
do
    ./dining-p $total $i &
    ((i++)) 
done