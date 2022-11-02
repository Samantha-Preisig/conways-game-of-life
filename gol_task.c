/** Assignment 1: Conway's Game of Life (Data and Task Parallelism Using Pthreads)
 * @author Samantha Preisig (ID: 1084328)
 * @date October 12, 2022
 * @file gol_task.c
 * @brief Conway's Game of Life using task parallelism
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "timer.h"
#include "generate_grid.h"

#define NUM_THREADS 3

/*Global command line information*/
int d_flag = 0;
int grid_size = 0;
int total_iterations = 0;

/*Global game board states*/
int *live_count;
int *current;
int *next;

pthread_barrier_t queues_filled_barrier, queues_empty_barrier, board_switch_barrier;
pthread_mutex_t board_switch;

/*Cell location (x, y)*/
typedef struct {
    int x;
    int y;
} Location;

/*Live/dead cell nodes*/
typedef struct cell_node {
    Location loc;
    int status; /*1 - alive, 0 - dead*/
    struct cell_node *next;
} CellNode;

/*Live cell queue*/
typedef struct {
    CellNode *head;
    CellNode *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond; /*condition variable*/
} LiveQueue;

/*Dead cell queue*/
typedef struct {
    CellNode *head;
    CellNode *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond; /*condition variable*/
} DeadQueue;

/*Each thread needs multiple arguments, so we create a dedicated struct*/
typedef struct {
    int worker_id;
    LiveQueue *q_live;
    DeadQueue *q_dead;
} thread_args;

/** For testing purposes
 * Prints LiveQueue linked list with all current live cells
 * @param q LiveQueue struct pointer
**/
void print_live_queue(LiveQueue *q){
    CellNode* head = q->head;
    if(head == NULL){
        fprintf(stdout, "Error: live queue empty.\n");
    }
    
    while(head != NULL){
        printf("Alive at location: (%d, %d)\n", head->loc.x, head->loc.y);
        head = head->next;
    }
}

/** For testing purposes
 * Prints DeadQueue linked list with all current dead cells
 * @param q DeadQueue struct pointer
**/
void print_deadqueue(DeadQueue *q){
    CellNode* head = q->head;
    if(head == NULL){
        fprintf(stdout, "Error: dead queue empty.\n");
    }

    while(head != NULL){
        printf("Dead at location: (%d, %d)\n", head->loc.x, head->loc.y);
        head = head->next;
    }
}

/** Checks if LiveQueue passed in is empty
 * @param q LiveQueue struct pointer
 * @return on success: 1 (true), on failure: 0 (false)
**/
int is_live_empty(LiveQueue *q){
    if(q->head == NULL){
        return 1;
    }
    return 0;
}

/** Checks if DeadQueue passed in is empty
 * @param q DeadQueue struct pointer
 * @return on success: 1 (true), on failure: 0 (false)
**/
int is_dead_empty(DeadQueue *q){
    if(q->head == NULL){
        return 1;
    }
    return 0;
}

/** Creates LiveQueue
 * @return on success: LiveQueue struct pointer
**/
LiveQueue *create_live_queue(){
    LiveQueue *q = (LiveQueue*)malloc(sizeof(LiveQueue));
    q->head = q->tail = NULL;
    pthread_mutex_init(&q->mutex, NULL);

    /*Initializing the condition variable*/
    pthread_cond_init(&q->cond, NULL);
    return q;
}

/** Creates DeadQueue
 * @return on success: DeadQueue struct pointer
**/
DeadQueue *create_dead_queue(){
    DeadQueue *q = (DeadQueue*)malloc(sizeof(DeadQueue));
    q->head = q->tail = NULL;
    pthread_mutex_init(&q->mutex, NULL);

    /*Initializing the condition variable*/
    pthread_cond_init(&q->cond, NULL);
    return q;
}

/** "Send" a message - append it onto the live queue
 * @param q LiveQueue struct pointer
 * @param x_loc row location of the live cell
 * @param y_loc column location of the live cell
**/
void append_live(LiveQueue *q, int x_loc, int y_loc){
    CellNode* node = (CellNode*)malloc(sizeof(CellNode));
    node->loc.x = x_loc;
    node->loc.y = y_loc;
    node->next = NULL;

    /*Critical section*/
    pthread_mutex_lock(&q->mutex);
    if (q->tail != NULL) {
        q->tail->next = node; //append after tail
        q->tail = node;
    } else {
        q->tail = q->head = node; //first node
    }
    
    /*Signal the consumer thread woiting on this condition variable*/
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    // fprintf(stderr, "Worker %d enqueues the message, signals cond variable, unlocks mutex, and goes to sleep\n", sender);
}

/** "Send" a message - append it onto the dead queue
 * @param q DeadQueue struct pointer
 * @param x_loc row location of the dead cell
 * @param y_loc column location of the dead cell
**/
void append_dead(DeadQueue *q, int x_loc, int y_loc){
    CellNode* node = (CellNode*)malloc(sizeof(CellNode));
    node->loc.x = x_loc;
    node->loc.y = y_loc;
    node->next = NULL;

    /*Critical section*/
    pthread_mutex_lock(&q->mutex);
    if (q->tail != NULL) {
        q->tail->next = node; //append after tail
        q->tail = node;
    } else {
        q->tail = q->head = node; //first node
    }

    /*Signal the consumer thread woiting on this condition variable*/
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    // fprintf(stderr, "Worker %d enqueues the message, signals cond variable, unlocks mutex, and goes to sleep\n", sender);
}

/** "Receive" a message - remove it from the live queue
 * @param q LiveQueue struct pointer
 * @param loc_out Location (x, y) of the live cell which is being removed
 * @return on success: 1, on failure: 0
**/
int remove_live(LiveQueue *q, Location *loc_out){
    
    int success = 0;
    
    /*Critical section*/
    pthread_mutex_lock(&q->mutex);
    
    /*
     *Wait for a signal telling us that there's something on the queue
     *If we get woken up but the queue is still null, we go back to sleep
     */
    while(q->head == NULL){
        fprintf(stderr,"Message queue is empty and getMessage goes to sleep (pthread_cond_wait)\n");
        pthread_cond_wait(&q->cond, &q->mutex);
        fprintf(stderr,"getMessage is woken up - someone signalled the condition variable\n");
    }
    
    /*By the time we get here, we know q->head is not null, so it's all good*/
    CellNode* oldHead = q->head;
    *loc_out = oldHead->loc; //copy out the message
    q->head = oldHead->next;
    if (q->head == NULL) {
        q->tail = NULL; //last node removed
    }
    free(oldHead);
    success = 1;
    
    /*Release lock*/
    pthread_mutex_unlock(&q->mutex);

    return success;
}

/** "Receive" a message - remove it from the dead queue
 * @param q DeadQueue struct pointer
 * @param loc_out Location (x, y) of the dead cell which is being removed
 * @return on success: 1, on failure: 0
**/
int remove_dead(DeadQueue *q, Location *loc_out){
    
    int success = 0;
    
    /*Critical section*/
    pthread_mutex_lock(&q->mutex);
    
    /*
     *Wait for a signal telling us that there's something on the queue
     *If we get woken up but the queue is still null, we go back to sleep
     */
    while(q->head == NULL){
        fprintf(stderr,"Message queue is empty and getMessage goes to sleep (pthread_cond_wait)\n");
        pthread_cond_wait(&q->cond, &q->mutex);
        fprintf(stderr,"getMessage is woken up - someone signalled the condition variable\n");
    }
    
    /*By the time we get here, we know q->head is not null, so it's all good*/
    CellNode* oldHead = q->head;
    *loc_out = oldHead->loc; //copy out the message
    q->head = oldHead->next;
    if (q->head == NULL) {
        q->tail = NULL; //last node removed
    }
    free(oldHead);
    success = 1;
    
    /*Release lock*/
    pthread_mutex_unlock(&q->mutex);

    return success;
}

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
 * @param i the row position of the cell
 * @param j the column position of the cell
 * @return on success: the number of live neighbours of a given cell
**/
int count_live_neighbours(int *state, int i, int j){
    int live_count=0;
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

/** Determines if the cell (i, j) will live in the next generation
 * @param i the row position of the cell
 * @param j the column position of the cell
 * @return on success: 1 (it lives to the next generation), on failure: 0
**/
int next_alive(int i, int j){
    if(current[i*grid_size+j] == 1 && (live_count[i*grid_size+j] < 2 || live_count[i*grid_size+j] > 3)){
        return 0;
    }
    if(current[i*grid_size+j] == 1 && (live_count[i*grid_size+j] == 2 || live_count[i*grid_size+j] == 3)){
        return 1;
    }
    if(current[i*grid_size+j] == 0 && live_count[i*grid_size+j] == 3){
        return 1;
    }
    return 0;
}

/** Builds next generation of the game board with live cells and removes the
 * live cell from the live queue
 * @param q_live LiveQueue struct pointer
**/
void build_next_live(LiveQueue *q_live){
    CellNode *head = q_live->head;

    while(head != NULL){
        next[head->loc.x*grid_size+head->loc.y] = 1;
        remove_live(q_live, &head->loc);
        head = head->next;
    }
}

/** Builds next generation of the game board with dead cells and removes the
 * dead cell from the dead queue
 * @param q_dead DeadQueue struct pointer
**/
void build_next_dead(DeadQueue *q_dead){
    CellNode *head = q_dead->head;
    
    while(head != NULL){
        next[head->loc.x*grid_size+head->loc.y] = 0;
        remove_dead(q_dead, &head->loc);
        head = head->next;
    }
}

/** Populates live_count array
 * live_count array stores the number of live neighbours for each cell in the game board
**/
void populate_live_count(){
    int i,j;
    for(i=0; i<grid_size; i++){
        for(j=0; j<grid_size; j++){
            live_count[i*grid_size+j] = count_live_neighbours(current, i, j);
        }
    }
}

/** Thread 1 worker function
 * Populate live_count array
 * Builds live and dead queues
 * Waits at queues_empty_barrier (waiting for thread 2 and 3 to empty their queues)
 * Set current state to next state
 * Repeat for specified number of iterations
 * @param arg thread arguments (see thread_args struct)
**/
void *worker1_func(void *arg){
    thread_args *args = (thread_args*)arg;
    int i, j, k;

    for(i=0; i<total_iterations; i++){
        
        populate_live_count();

        for(j=0; j<grid_size; j++){
            for(k=0; k<grid_size; k++){

                if(next_alive(j, k) == 1){
                    append_live(args->q_live, j, k); //building live queue
                }
                if(next_alive(j, k) == 0){
                    append_dead(args->q_dead, j, k); //building dead queue
                }
            }
        }
        // printf("thread 1 finished filling live and dead queues\n");
        // print_state(current);

        /*Waiting for live and dead queues to empty*/
        // printf("thread 1 waiting for thread 2 and 3\n");
        pthread_barrier_wait(&queues_filled_barrier);
        // printf("\t>[filled barrier reached in thread 1] queues are full\n");
        
        
        // printf("thread 1 waiting for thread 2 and 3\n");
        pthread_barrier_wait(&queues_empty_barrier);
        // printf("\t>[empty barrier reached in thread 1] queues are empty\n");
        if(d_flag && (args->worker_id == 1)){
            print_state(next); printf("\n");
        }
        current = next;
        
        pthread_barrier_wait(&board_switch_barrier);
    }
}

/** Thread 2 worker function
 * Waits for queues to be filled by thread 1
 * Reads through the live queue and updates to the next iteration with all cells that
 * are/become alive while removing these cells from the live queue
 * Repeat for specified number of iterations
 * @param arg thread arguments (see thread_args struct)
**/
void *worker2_func(void *arg){
    thread_args *args = (thread_args*)arg;
    int i;

    for(i=0; i<total_iterations; i++){

        /*Wait for live and dead queues to be filled by thread 1*/
        // printf("thread 2 waiting for thread 1\n");
        pthread_barrier_wait(&queues_filled_barrier);
        // printf("\t>[filled barrier reached in thread 2] queues are full\n");
        
        /*Build next grid with the live queue*/
        pthread_mutex_lock(&board_switch);
        build_next_live(args->q_live);
        pthread_mutex_unlock(&board_switch);

        /*If live queue is empty, sit at empty queue barrier*/
        if(is_live_empty(args->q_live)){
            // printf("live queue is empty.\n");
            pthread_barrier_wait(&queues_empty_barrier);
            // printf("\t>[empty barrier reached in thread 2] queues are empty\n");

        }

        // printf("thread 2 on standby....\n");
        pthread_barrier_wait(&board_switch_barrier);
        // printf("\t>[board switch barrier reached in thread 2]\n");
    }
}

/** Thread 3 worker function
 * Waits for queues to be filled by thread 1
 * Reads through the dead queue and updates to the next iteration with all cells that
 * are/become dead while removing these cells from the dead queue
 * Repeat for specified number of iterations
 * @param arg thread arguments (see thread_args struct)
**/
void *worker3_func(void *arg){
    thread_args *args = (thread_args*)arg;
    int i;

    for(i=0; i<total_iterations; i++){
        /*Wait for live and dead queues to be filled by thread 1*/
        // printf("thread 3 waiting for thread 1\n");
        pthread_barrier_wait(&queues_filled_barrier);
        // printf("\t>[filled barrier reached in thread 3] queues are full\n");
        

        /*Build next grid with dead queue*/
        pthread_mutex_lock(&board_switch);
        build_next_dead(args->q_dead);
        pthread_mutex_unlock(&board_switch);

        /*If dead queue is empty, sit at empty queue barrier*/
        if(is_dead_empty(args->q_dead)){
            // printf("dead queue is empty.\n");
            pthread_barrier_wait(&queues_empty_barrier);
            // printf("\t>[empty barrier reached in thread 3] queues are empty\n");
        }

        // printf("thread 3 on standby....\n");
        pthread_barrier_wait(&board_switch_barrier);
        // printf("\t>[board switch barrier reached in thread 3]\n");
    }
}

/** Driver
 * Read command line arguments and store information locally/globally
 * Generate randomized game board
 * Create 3 threads, join threads once they've terminated
 * @param arg thread arguments (see thread_args struct)
**/
int main(int argc, char *argv[]){

    /*Gathering info from command line*/
    if(argc >= 3){
        grid_size = atoi(argv[1]);
        total_iterations = atoi(argv[2]);

        if(argc == 4){
            d_flag = 1;
        }
    }else{
        fprintf(stdout, "Usage: ./gol_task gridSize nIterations -d\n");
    }

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
    pthread_t tid[NUM_THREADS];
    thread_args args[NUM_THREADS];
    LiveQueue *q_live = create_live_queue();
    DeadQueue *q_dead = create_dead_queue();
    
    pthread_barrier_init(&queues_filled_barrier, NULL, NUM_THREADS);
    pthread_barrier_init(&queues_empty_barrier, NULL, NUM_THREADS);
    pthread_barrier_init(&board_switch_barrier, NULL, NUM_THREADS);
    pthread_mutex_init(&board_switch, NULL);

    /*Information for each thread*/
    for(i=0; i<2; i++){
        args[i].worker_id = i+1;
        args[i].q_live = q_live;
        args[i].q_dead = q_dead;
    }

    double start, finish;
    GET_TIME(start);

    int err = pthread_create(&tid[0], NULL, worker1_func, &args[0]);
    if(err != 0){
        printf("Cannot create thread 1, error: %d", err);
        exit(-1);
    }
    err = pthread_create(&tid[1], NULL, worker2_func, &args[1]);
    if(err != 0){
        printf("Cannot create thread 2, error: %d", err);
        exit(-1);
    }
    err = pthread_create(&tid[1], NULL, worker3_func, &args[1]);
    if(err != 0){
        printf("Cannot create thread 3, error: %d", err);
        exit(-1);
    }

    /*Wait for worker threads to terminate*/
    for (i = 0; i < 2; i++) {
        pthread_join(tid[i], NULL);
    }

    GET_TIME(finish);
    printf("[gol_task %d %d] Elapsed time = %.4f seconds\n", grid_size, total_iterations, (finish - start));

    pthread_barrier_destroy(&queues_filled_barrier);
    pthread_barrier_destroy(&queues_empty_barrier);
    pthread_barrier_destroy(&board_switch_barrier);
    pthread_mutex_destroy(&board_switch);

    return 0;
}