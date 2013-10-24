#!/usr/bin/env bash

for i in tests/*.t
do
    ./turtle $i -s -d -o out.p &> /dev/null
    ../turtle_2/Turtle/tools/DisASM/DisASM ${i/.t/.p} out.asm > /dev/null
    sdiff out.p out.asm > /dev/null

    if [ $? -eq 0 ]
    then
        echo $i " passed"
    else
        echo $i " failed"
    fi
    rm -f out.p out.asm
done
