/** Assignment 1: Conway's Game of Life (Data and Task Parallelism Using Pthreads)
 * @author Samantha Preisig (ID: 1084328)
 * @date October 12, 2022
 * @file generate_grid.h
 * @brief Generates randomized game board with a given grid size
**/

#ifndef GENERATE_GRID_H
#define GENERATE_GRID_H

#include <stdlib.h>
#include <time.h>

int *generate_grid(int grid_size){
    srand(time(NULL));
    int i, *grid = (int*)malloc(grid_size*grid_size*sizeof(int));

    for(i=0; i<(grid_size*grid_size); i++){
        grid[i] = rand() % 2;
    }
    return grid;
}

#endif