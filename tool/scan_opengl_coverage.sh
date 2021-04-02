#!/bin/sh

_out=""
while read line; do 
    a=""
    count=$(grep -rno "../impl" -e "$line" | wc -l)
    echo "$count : $line"
done < gles2_function_list

