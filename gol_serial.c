#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include "generate_grid.h"

int grid_size = 0;

void print_state(int *state, int num_rows, int num_cols){
    int i, j;
    for(i=0; i<num_rows; i++){
        for(j=0; j<num_cols; j++){
            if(state[i*num_cols+j] == 0){
                printf(". ");
            }
            if(state[i*num_cols+j] == 1){
                printf("X ");
            }
        }
        printf("\n");
    }
}

int count_live_neighbours(int *state, int num_rows, int num_cols, int cell_row, int cell_col){
    int i=cell_row, j=cell_col, live_count=0;
    if(state[(i*grid_size+j)-grid_size] == 1){ //directly above cell
        live_count++;
    }
    if(state[(i*grid_size+j)+grid_size] == 1){ //directly below cell
        live_count++;
    }
    if(state[(i*grid_size+j)-1] == 1){ //directly left of cell
        live_count++;
    }
    if(state[(i*grid_size+j)+1] == 1){ //directly right of cell
        live_count++;
    }
    if(state[(i*grid_size+j)-(grid_size+1)] == 1){ //top-left of cell
        live_count++;
    }
    if(state[(i*grid_size+j)-(grid_size-1)] == 1){ //top-right of cell
        live_count++;
    }
    if(state[(i*grid_size+j)+(grid_size-1)] == 1){ //bottom-left of cell
        live_count++;
    }
    if(state[(i*grid_size+j)+(grid_size+1)] == 1){ //bottom-right of cell
        live_count++;
    }
    return live_count;
}

int *build_next(int *current, int *next, int *live_count, int num_rows, int num_cols){
    int i, j;
    for(i=0; i<num_rows; i++){
        for(j=0; j<num_cols; j++){
            if(current[i*num_cols+j] == 1 && (live_count[i*num_cols+j] < 2 || live_count[i*num_cols+j] > 3)){
                next[i*num_cols+j] = 0;
            }
            if(current[i*num_cols+j] == 1 && (live_count[i*num_cols+j] == 2 || live_count[i*num_cols+j] == 3)){
                next[i*num_cols+j] = 1;
            }
            if(current[i*num_cols+j] == 0 && live_count[i*num_cols+j] == 3){
                next[i*num_cols+j] = 1;
            }
        }
    }
    return next;
}

int main(int argc, char *argv[]){

    /*Gathering command-line arguments*/
    char *seed;
    int num_iter=0, d_flag=0;
    if(argc >= 3){
        // seed = (char*)malloc((strlen(argv[1])+1)*sizeof(char));
        // strncpy(seed, argv[1], strlen(argv[1])+1);
        grid_size = atoi(argv[1]);
        num_iter = atoi(argv[2]);

        if(argc == 4){
            d_flag = 1;
        }
    }else{
        fprintf(stdout, "Usage: ./gol_serial [grid_size] [num_iterations] [-d]\n");
    }

    int *current = (int*)malloc(grid_size*grid_size*sizeof(int*)), *next = (int*)malloc(grid_size*grid_size*sizeof(int*));

    // // FOR TESTING PURPOSES (using known seeds)
    // /*Reading and storing seed (initial state)*/
    // FILE *fp = fopen("diehard40X40.seed", "r");
    // if(fp == NULL){
    //     fprintf(stdout, "Error: seed DNE.\n");
    // }
    // char c;
    // int i=0, j=0;
    // while((c = fgetc(fp)) != EOF){
    //     if(c == '0'){
    //         current[i*grid_size+j] = 0; next[i*grid_size+j] = 0; j++;
    //     }
    //     if(c == '1'){
    //         current[i*grid_size+j] = 1; next[i*grid_size+j] = 1; j++;
    //         // printf("%d, %d\n", i, j);
    //     }
    //     if(c == '\n'){
    //         i++; j=0;
    //     }
    // }
    // fclose(fp);

    /*Generating random grid*/
    int i, j;
    current = generate_grid(grid_size);
    next = current;
    
    /*
     * Get timing data
     */
    double start, finish;
    GET_TIME(start);

    int iter=0;
    while(iter < num_iter){
        // system ("sleep 0.25");
        // system ("clear");
        
        /*Creating an array to store each cell's number of live neighbours*/
        int *live_count = (int*)malloc(grid_size*grid_size*sizeof(int*));
        for(i=0; i<grid_size; i++){
            for(j=0; j<grid_size; j++){
                live_count[i*grid_size+j] = count_live_neighbours(current, grid_size, grid_size, i, j);
            }
        }

        /*Building next generation state*/
        next = build_next(current, next, live_count, grid_size, grid_size);
        if(d_flag){
            print_state(next, grid_size, grid_size); printf("\n");
        }
        current = next;
        iter++;
    }
    
    /*
     * Get and display elapsed wall time time
     */
    GET_TIME(finish);
    printf("Elapsed time = %.4f seconds\n", finish - start);

    return 0;
}