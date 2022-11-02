/** Assignment 1: Conway's Game of Life (Data and Task Parallelism Using Pthreads)
 * @author Samantha Preisig (ID: 1084328)
 * @date October 12, 2022
 * @file gol_data.c
 * @brief Conway's Game of Life using data parallelism
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "timer.h"
#include "generate_grid.h"

/*Global command line information*/
int d_flag = 0;
int total_threads = 0;
int grid_size = 0;
int total_iterations = 0;

/*Global game board states*/
int *live_count;
int *current;
int *next;

pthread_barrier_t barrier; //to allow synchronized board updates (current -> next)

/*Each thread needs multiple arguments, so we create a dedicated struct*/
typedef struct {
    int worker_id;
    int personal_grid_start;
    int thread_num_rows;
} thread_args;

/** Prints the game board, given the state passed in
 * @param state 1D int array containing the game board (current or next)
**/
void print_state(int *state){
    int i, j;
    for(i=0; i<grid_size; i++){
        for(j=0; j<grid_size; j++){
            if(state[i*grid_size+j] == 0){
                printf(". ");
            }
            if(state[i*grid_size+j] == 1){
                printf("X ");
            }
        }
        printf("\n");
    }
}

/** Counts the number of live neighbours of a given cell
 * @param state 1D int array containing the game board (current or next)
 * @param start_pos the starting position for a particular partition
 * @param thread_num_rows the row number the particular partition covers up to
 * @param cell_row the row position of the cell
 * @param cell_col the column position of the cell
 * @return on success: the number of live neighbours of a given cell
**/
int count_live_neighbours(int *state, int start_pos, int thread_num_rows, int cell_row, int cell_col){
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

/** Builds the next generation of the game board using the live_count of each cell
 * and the current state of the board
 * @param start_pos the starting position for a particular partition
 * @param thread_num_rows the row number the particular partition covers up to
 * @return on success: the next generation of the game board
**/
int *build_next(int start_pos, int thread_num_rows){
    int i, j;
    for(i=start_pos; i<thread_num_rows; i++){
        for(j=0; j<grid_size; j++){
            if(current[i*grid_size+j] == 1 && (live_count[i*grid_size+j] < 2 || live_count[i*grid_size+j] > 3)){
                next[i*grid_size+j] = 0;
            }
            if(current[i*grid_size+j] == 1 && (live_count[i*grid_size+j] == 2 || live_count[i*grid_size+j] == 3)){
                next[i*grid_size+j] = 1;
            }
            if(current[i*grid_size+j] == 0 && live_count[i*grid_size+j] == 3){
                next[i*grid_size+j] = 1;
            }
        }
    }
    return next;
}

/** Populates the live_count array which keeps track of the number of live
 * neighbours each cell has
 * @param start_pos the starting position for a particular partition
 * @param thread_num_rows the row number the particular partition covers up to
**/
void play_gol(int start_pos, int thread_num_rows){
    int i,j;
    for(i=start_pos; i<thread_num_rows; i++){
        for(j=0; j<grid_size; j++){
            live_count[i*grid_size+j] = count_live_neighbours(current, start_pos, thread_num_rows, i, j);
        }
    }
}

/** Thread worker function
 * Populate live_count array
 * Build next generation based on live_count
 * Wait at barrier for all threads to build next generation (for their partition of the board)
 * Set current state to next state
 * Repeat for specified number of iterations
 * @param arg thread arguments (see thread_args struct)
**/
void *worker_func(void *arg){
    thread_args *args = (thread_args*)arg;
    int i;

    for(i=0; i<total_iterations; i++){
        play_gol(args->personal_grid_start, args->thread_num_rows);

        pthread_barrier_wait(&barrier);
        next = build_next(args->personal_grid_start, args->thread_num_rows);

        if(d_flag && (args->worker_id == 1)){
            print_state(next); printf("\n");
        }

        pthread_barrier_wait(&barrier);
        current = next;
    }
}

/** Driver
 * Read command line arguments and store information locally/globally
 * Generate randomized game board
 * Create threads, join threads once they've terminated
 * @param arg thread arguments (see thread_args struct)
**/
int main(int argc, char *argv[]){

    /*Gathering info from command line*/
    if(argc >= 4){
        total_threads = atoi(argv[1]);
        grid_size = atoi(argv[2]);
        total_iterations = atoi(argv[3]);

        if(argc == 5){
            d_flag = 1;
        }
    }else{
        fprintf(stdout, "Usage: ./gol_data [num_threads] [grid_size] [num_iterations] [-d]\n");
    }
    pthread_barrier_init(&barrier, NULL, total_threads);

    live_count = (int*)malloc(grid_size*grid_size*sizeof(int*));
    current = (int*)malloc(grid_size*grid_size*sizeof(int*));
    next = (int*)malloc(grid_size*grid_size*sizeof(int*));

    // FOR TESTING PURPOSES (using known seeds)
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
    // print_state(current);

    /*Creating worker threads (inspired by in-class example: parallelSort.c*/
    pthread_t tid[total_threads];
    thread_args args[total_threads];

    /*Paritioning grid*/
    int err, personal_grid = grid_size/total_threads;
    if(grid_size % total_threads != 0){
        personal_grid++;
    }

    double start, finish;
    GET_TIME(start);

    /*Information for each thread*/
    for(i=0; i<total_threads; i++){
        args[i].worker_id = i+1;
        args[i].personal_grid_start = i*personal_grid;
        args[i].thread_num_rows = args[i].personal_grid_start + personal_grid; //i*personal_grid + personal_grid;
        if(args[i].thread_num_rows > grid_size){
            args[i].thread_num_rows = grid_size;
        }
        err = pthread_create(&tid[i], NULL, worker_func, &args[i]);
        if(err != 0){
            printf("Cannot create thread, error: %d", err);
            exit(-1);
        }
    }

    /*Wait for worker threads to terminate*/
    for (i = 0; i < total_threads; i++) {
        pthread_join(tid[i], NULL);
    }

    GET_TIME(finish);
    printf("[gol_data %d %d %d] Elapsed time = %.4f seconds\n", total_threads, grid_size, total_iterations, (finish - start));

    pthread_barrier_destroy(&barrier);

    return 0;
}