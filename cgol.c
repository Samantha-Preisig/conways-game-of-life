#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 20
#define COLS 40

/*
 * Program name: cgol.c
 * Authour: Samantha Preisig
 * Purpose: A version of Conway's Game of Life, a cellular automaton zero-player game.
 */

int main(int argc, char *argv[]){
  int flush (char *input);

  FILE *fp;
  int i, j, reqTicks, borderCounter, exit, validStart, validRepeat, neighbourCount, aliveCounter, checked1, checked2, checked3, checked4, checked5, checked6, checked7, checked8, repeatCheck, totalTicks, aliveArea, deadArea, area, seedArea;
  char filename[20];
  int current[ROWS][COLS*2];
  int future[ROWS][COLS*2];
  char start[2];
  char repeat[2];
  int tick = 0;
  int loop = 0;

  /*Command-line error checking*/
  if(argc < 2 || argc > 3){
    printf("Usage: ./cgol filename.seed time-interval\n");
    return 1;
  }else if(argc == 2){
    strcpy(filename, argv[1]);
    reqTicks = 50;
  }else{
    strcpy(filename, argv[1]);
    reqTicks = atoi(argv[2]);
    if(reqTicks < 0){
      return 1;
    }
  }
  totalTicks = reqTicks;

  /*Validating seed and storing grid*/
  if((fp = fopen(filename, "r")) != NULL){
    for(i=0; i<ROWS; i++){
      for(j=0; j<COLS*2; j+=2){
        fscanf(fp, "%d", &current[i][j]);
        if(current[i][j] != 0 && current[i][j] != 1){
          return 1;
        }
      }
    }
    fclose(fp);
  }else{
    printf("Cannot open %s\n", filename);
    return 1;
  }

  /*Printing grid (+border)*/
  system("clear");
  /*Top of border*/
  borderCounter = 0;
  while(borderCounter < COLS){
    if(borderCounter == 0){
      printf(" ");
    }
    printf("-");
    borderCounter++;
  }
  borderCounter = 0; /*reset for making bottom border*/
  printf("\n");

  for(i=0; i<ROWS; i++){
    for(j=0; j<COLS*2; j+=2){
      /*Left side of border*/
      if(j == 0){
        printf ("|");
      }
      if(current[i][j] == 1){
        printf("X");
      }else if(current[i][j] == 0){
        printf(" ");
      }
    }
    /*Right side of border*/
    printf("|\n");
  }

  /*Bottom of border*/
  while(borderCounter < COLS){
    if (borderCounter == 0){
      printf(" ");
    }
    printf("-");
    borderCounter++;
  }
  printf("%d\n", tick);

  exit = 0;
  while(exit == 0){
    validStart = 0;
    if(loop == 0){
      printf ("Start? [y or n]: ");
    }

    if(loop == 0){
      while(validStart == 0){
        flush(start);
        scanf("%s", start);

        if((strncmp(start, "y", 2) == 0) || (strncmp(start, "Y", 2) == 0) || (strncmp(start, "n", 2) == 0) || (strncmp(start, "N", 2) == 0)){
          validStart = 1;
        }else{
          printf("Invalid response.\n");
          printf("Start? [y or n]: ");
        }
      }
    }

    if(((strncmp(start, "y", 2) == 0) || (strncmp(start, "Y", 2) == 0)) || loop == 1){
      repeatCheck = 0;
      while(repeatCheck == 0){
        while(tick < totalTicks){

          /*Printing current grid*/
          system("sleep 0.25");
          system("clear");
            /*Top of border*/
            borderCounter = 0;
            while(borderCounter < COLS){
              if(borderCounter == 0){
                printf(" ");
              }
              printf("-");
              borderCounter++;
            }
            borderCounter = 0; /*reset for making bottom border*/
            printf("\n");

            for(i=0; i<ROWS; i++){
              for (j=0; j<COLS*2; j+=2){
                /*Left side of border*/
                if(j == 0){
                  printf("|");
                }
                if(current[i][j] == 1){
                  printf("X");
                }else if(current[i][j] == 0){
                  printf(" ");
                }
              }
              /*Right side of border*/
              printf("|\n");
            }

            /*Bottom of border*/
            while(borderCounter < COLS){
              if(borderCounter == 0){
                printf(" ");
              }
              printf("-");
              borderCounter++;
            }
            printf("%d\n", tick);

            for (i = 0; i < ROWS; i++) {
              for (j = 0; j < COLS*2; j+=2) {
                checked1 = 0;
                checked2 = 0;
                checked3 = 0;
                checked4 = 0;
                checked5 = 0;
                checked6 = 0;
                checked7 = 0;
                checked8 = 0;
                neighbourCount = 0;
                aliveCounter = 0;

            /*Edges*/
            if(j == 0 && (i != 0 && i != 19)){ /*Left edge of grid (excluding corners)*/
              while(neighbourCount < 5){
                if(current[i-1][j] == 1 && checked2 == 0){
                  checked2 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j+2] == 1 && checked3 == 0){
                  checked3 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i][j+2] == 1 && checked5 == 0){
                  checked5 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j] == 1 && checked7 == 0){
                  checked7 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j+2] == 1 && checked8 == 0){
                  checked8 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j] == 0 || current[i-1][j+2] == 0 || current[i][j+2] == 0 || current[i+1][j] == 0 || current[i+1][j+2] == 0){
                  neighbourCount++;
                }
              }
            }else if(i == 19 && (j != 0 && j != 39)){ /*Bottom edge of grid (excluding corners)*/
              while(neighbourCount < 5){
                if(current[i-1][j-2] == 1 && checked1 == 0){
                  checked1 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j] == 1 && checked2 == 0){
                  checked2 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j+2] == 1 && checked3 == 0){
                  checked3 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i][j-2] == 1 && checked4 == 0){
                  checked4 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i][j+2] == 1 && checked5 == 0){
                  checked5 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i-1][j-2] == 0 || current[i-1][j] == 0 || current[i-1][j+2] == 0 || current[i][j-2] == 0 || current[i][j+2] == 0){
                  neighbourCount++;
                }
              }
            }else if(j == 39 && (i != 0 && i != 19)){ /*Right edge of grid (excluding corners)*/
              while(neighbourCount < 5){
                if(current[i-1][j-2] == 1 && checked1 == 0){
                  checked1 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j] == 1 && checked2 == 0){
                  checked2 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i][j-2] == 1 && checked4 == 0){
                  checked4 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j-2] == 1 && checked6 == 0){
                  checked6 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j] == 1 && checked7 == 0){
                  checked7 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j-2] == 0 || current[i-1][j] == 0 || current[i][j-2] == 0 || current[i+1][j-2] == 0 || current[i+1][j] == 0){
                  neighbourCount++;
                }
              }
            }else if(i == 0 && (j != 0 && j != 39)){ /* Top edge of grid (excluding corners)*/
              while(neighbourCount < 5){
                if(current[i][j-2] == 1 && checked4 == 0){
                  checked4 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i][j+2] == 1 && checked5 == 0){
                  checked5 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i+1][j-2] == 1 && checked6 == 0){
                  checked6 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i+1][j] == 1 && checked7 == 0){
                  checked7 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i+1][j+2] == 1 && checked8 == 0){
                  checked8 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i-1][j-2] == 0 || current[i-1][j] == 0 || current[i-1][j+2] == 0 || current[i][j-2] == 0 || current[i][j+2] == 0 || current[i+1][j-2] == 0 || current[i+1][j] == 0 || current[i+1][j+2] == 0){
                  neighbourCount++;
                }
              }
            
            /*Corners*/
            }else if(i == 0 && j == 0){
              while(neighbourCount < 3){
                if(current[i][j+2] == 1 && checked5 == 0){
                  checked5 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j] == 1 && checked7 == 0){
                  checked7 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j+2] == 1 && checked8 == 0){
                  checked8 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i][j+2] == 0 || current[i+1][j] == 0 || current[i+1][j+2] == 0){
                  neighbourCount++;
                }
              }
            }else if(i == 19 && j == 0){
              while(neighbourCount < 3) {
                if(current[i-1][j] == 1 && checked2 == 0){
                  checked2 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j+2] == 1 && checked3 == 0){
                  checked3 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i][j+2] == 1 && checked5 == 0){
                  checked5 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j] == 0 || current[i-1][j+2] == 0 || current[i][j+2] == 0){
                  neighbourCount++;
                }
              }
            }else if(i == 19 && j == 39){
              while(neighbourCount < 3){
                if(current[i-1][j-2] == 1 && checked1 == 0){
                  checked1 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i-1][j] == 1 && checked2 == 0){
                  checked2 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i][j-2] == 1 && checked4 == 0){
                  checked4 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j-2] == 0 || current[i-1][j] == 0 || current[i][j-2] == 0){
                  neighbourCount++;
                }
              }
            }else if(i == 0 && j == 39){
              while(neighbourCount < 3){
                if(current[i][j-2] == 1 && checked4 == 0){
                  checked4 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j-2] == 1 && checked6 == 0){
                  checked6 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j] == 1 && checked7 == 0){
                  checked7 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if (current[i][j-2] == 0 || current[i+1][j-2] == 0 || current[i+1][j] == 0){
                  neighbourCount++;
                }
              }

            /*Middle of grid*/
            }else{
              while(neighbourCount < 8){
                if(current[i-1][j-2] == 1 && checked1 == 0){
                  checked1 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j] == 1 && checked2 == 0){
                  checked2 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j+2] == 1 && checked3 == 0){
                  checked3 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i][j-2] == 1 && checked4 == 0){
                  checked4 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i][j+2] == 1 && checked5 == 0){
                  checked5 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j-2] == 1 && checked6 == 0){
                  checked6 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j] == 1 && checked7 == 0){
                  checked7 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i+1][j+2] == 1 && checked8 == 0){
                  checked8 = 1;
                  aliveCounter++;
                  neighbourCount++;
                }else if(current[i-1][j-2] == 0 || current[i-1][j] == 0 || current[i-1][j+2] == 0 || current[i][j-2] == 0 || current[i][j+2] == 0 || current[i+1][j-2] == 0 || current[i+1][j] == 0 || current[i+1][j+2] == 0){
                  neighbourCount++;
                }
              }
            }

            if(current[i][j] == 1 && aliveCounter < 2){
              future[i][j] = 0;
            }else if(current[i][j] == 1 && (aliveCounter == 2 || aliveCounter == 3)){
              future[i][j] = 1;
            }else if(current[i][j] == 0 && aliveCounter == 3){
              future[i][j] = 1;
            }else if(current [i][j] == 1 && aliveCounter > 3){
              future[i][j] = 0;
            }else if(current[i][j] == 0 && aliveCounter == 0){
              future[i][j] = 0;
            }
          }
        }

        /*Printing future grid*/
        system("sleep 0.25");
        system("clear");
        /*Top of border*/
        borderCounter = 0;
        while(borderCounter < COLS){
          if(borderCounter == 0){
            printf(" ");
          }
          printf("-");
          borderCounter++;
        }
        borderCounter = 0; /*reset for making bottom border*/
        printf("\n");
        for(i=0; i<ROWS; i++){
          for(j=0; j<COLS*2; j+=2){
            /*Left side of border*/
            if(j == 0){
              printf("|");
            }
            if(future[i][j] == 1){
              printf("X");
            }else{
              printf(" ");
            }
          }
          /*Right side of border*/
          printf("|\n");
        }

        /*Bottom of border*/
        while(borderCounter < COLS){
          if(borderCounter == 0){
            printf(" ");
          }
          printf("-");
          borderCounter++;
        }
        tick++;
        printf("%d\n", tick);

        /*Checking if current array (that has just been evaluated) is equal to the future array of the last tick*/
        area = 0;
        aliveArea = 0;
        deadArea = 0;
        for(i=0; i<ROWS; i++){
          for(j=0; j<COLS*2; j+=2){
            if(current[i][j] == 1 && (current[i][j] == future[i][j])){
              aliveArea++;
            }else if(current[i][j] == 0 && (current[i][j] == future[i][j])){
              deadArea++;
            }
          }
        }
        seedArea = ROWS*(COLS);
        area = aliveArea + deadArea;
        if(area == seedArea){
          tick = totalTicks;
          repeatCheck = 1;
          exit = 1;
        }

        /*Coping future array to current array*/
        for(i=0; i<ROWS; i++) {
          memcpy(&current[i], &future[i], sizeof(future[0]));
        }
      }
      loop = 1;

      if(tick == totalTicks && repeatCheck == 0){
        validRepeat = 0;
        printf("Continue? [y or n]: ");
        while(validRepeat == 0){
          flush(repeat);
          scanf("%s", repeat);

          if((strncmp(repeat, "y", 2) == 0) || (strncmp(repeat, "Y", 2) == 0) || (strncmp(repeat, "n", 2) == 0) || (strncmp(repeat, "N", 2) == 0)){
            validRepeat = 1;
          }else{
            printf("Invalid response.\n");
            printf("Continue? [y or n]: ");
          }
        }
      }

      if((strncmp(repeat, "y", 2) == 0) || (strncmp(repeat, "Y", 2) == 0)){
        flush(repeat);
        totalTicks += reqTicks;
        }else if((strncmp(repeat, "n", 2) == 0) || (strncmp(repeat, "N", 2) == 0)){
          repeatCheck = 1;
          exit = 1;
        }
      } /*repeatCheck loop ends*/
    }else if((strncmp(start, "n", 2) == 0) || (strncmp(start, "N", 2) == 0)){
      exit = 1;
    }
  }
  return 0;
}
