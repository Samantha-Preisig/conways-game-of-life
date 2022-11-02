#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_num_live_cells(int *state, int num_rows, int num_cols){
    int i, j, count=0;
    for(i=0; i<num_rows; i++){
        for(j=0; j<num_cols; j++){
            if(state[i*num_cols+j] == 1){
                count++;
            }
        }
    }
    return count;
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
    int i, j, live_count=0;
    for(i=0; i<num_rows; i++){
        for(j=0; j<num_cols; j++){
            if(i == cell_row && j == cell_col){
                if(state[(i*num_cols+j)-num_cols] == 1){ /*directly above cell*/
                    // printf("here1\n");
                    live_count++;
                }
                if(state[(i*num_cols+j)+num_cols] == 1){ /*directly below cell*/
                    // printf("here2\n");
                    live_count++;
                }
                if(state[(i*num_cols+j)-1] == 1){ /*directly left of cell*/
                    // printf("here3\n");
                    live_count++;
                }
                if(state[(i*num_cols+j)+1] == 1){ /*directly right of cell*/
                    // printf("here4\n");
                    live_count++;
                }
                if(state[(i*num_cols+j)-(num_cols+1)] == 1){ /*top-left of cell*/
                    // printf("here5\n");
                    live_count++;
                }
                if(state[(i*num_cols+j)-(num_cols-1)] == 1){ /*top-right of cell*/
                    // printf("here6\n");
                    live_count++;
                }
                if(state[(i*num_cols+j)+(num_cols-1)] == 1){ /*bottom-left of cell*/
                    // printf("here7\n");
                    live_count++;
                }
                if(state[(i*num_cols+j)+(num_cols+1)] == 1){ /*bottom-right of cell*/
                    // printf("here8\n");
                    live_count++;
                }
            }
        }
    }
    return live_count;
}

int main(int argc, char *argv[]){

    /*Gathering command-line arguments*/
    char *seed;
    int num_rows=0, num_cols=0;
    if(argc == 4){
        seed = (char*)malloc((strlen(argv[1])+1)*sizeof(char));
        strncpy(seed, argv[1], strlen(argv[1])+1);
        num_rows = atoi(argv[2]); num_cols = atoi(argv[3]);
    }else{
        fprintf(stdout, "Usage: ./serial_cgol [seed] [num_rows] [num_cols]\n");
    }

    int *current = (int*)malloc(num_rows*num_cols*sizeof(int*)), *next = (int*)malloc(num_rows*num_cols*sizeof(int*));

    /*Reading and storing seed (initial state)*/
    FILE *fp = fopen(seed, "r");
    if(fp == NULL){
        fprintf(stdout, "Error: seed DNE.\n");
    }
    char c;
    int i=0, j=0;
    while((c = fgetc(fp)) != EOF){
        if(c == '0'){
            current[i*num_cols+j] = 0; next[i*num_cols+j] = 0; j++;
        }
        if(c == '1'){
            current[i*num_cols+j] = 1; next[i*num_cols+j] = 1; j++;
            // printf("%d, %d\n", i, j);
        }
        if(c == '\n'){
            i++; j=0;
        }
    }
    fclose(fp);

    print_state(current, num_rows, num_cols);

    int live_cells = get_num_live_cells(current, num_rows, num_cols);

    while(live_cells > 0){
        system ("sleep 0.25");
        system ("clear");
        
        /*Creating an array to store each cell's number of live neighbours*/
        int *live_count = (int*)malloc(num_rows*num_cols*sizeof(int*));
        for(i=0; i<num_rows; i++){
            for(j=0; j<num_cols; j++){
                live_count[i*num_cols+j] = count_live_neighbours(current, num_rows, num_cols, i, j);
            }
        }

        /*Building next generation state*/
        next = build_next(current, next, live_count, num_rows, num_cols);
        print_state(next, num_rows, num_cols);
        current = next;
        live_cells = get_num_live_cells(current, num_rows, num_cols);
    }
    return 0;
}