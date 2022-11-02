#!/bin/bash

cat /proc/cpuinfo | grep processor | wc -l
printf "\n"

# Compiling giol_serial, gol_data, and gol_task
make
printf "\n"

# gol_serial (10 runs)
i=1;
while [ $i -le 10 ];
do
    ./gol_serial 100 10000;
    ((i++));
done

# gol_data using 1 thread
i=1;
while [ $i -le 10 ];
do
    ./gol_data 1 100 10000;
    ((i++));
done
printf "\n"

# gol_data using 2 threads
i=1;
while [ $i -le 10 ];
do
    ./gol_data 2 100 10000;
    ((i++));
done
printf "\n"

# gol_data using 3 threads
i=1;
while [ $i -le 10 ];
do
    ./gol_data 3 100 10000;
    ((i++));
done
printf "\n"

# gol_data using 4 threads
i=1;
while [ $i -le 10 ];
do
    ./gol_data 4 100 10000;
    ((i++));
done
printf "\n"

# gol_task (10 runs)
i=1;
while [ $i -le 10 ];
do
    ./gol_task 100 10000;
    ((i++));
done