# Conway's Game of Life
Conway's Game of Life consists of a two-dimension grid of cells. Each cell is either alive or dead with eight neighbours. Each neighbouring cell is either horizontal, vertical, or diagonally adjacent.

### At each step in time (or tick/generation), the following transitions occur:
  1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
  2. Any live cell with two or three live neighbours lives on to the next generation.
  3. Any live cell with more than three live neighbours dies, as if by overpopulation.
  4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
- The intial pattern consistutes the seed of the system. The first generation is created by applying the above rules simultaneously to every cell in the seed; births and deaths occur simultaneously.

Reference: https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

### Data Parallelism
This algorithm (gol_data.c) will require the threads to read overlapping cells to calculate the next state of the grid. The grid is divided evenly among the number of threads, with each thread calculating the next state of its section of the board. Thus, the work is divided creating a parallelized version of gol_serial.

### Task Parallelism
Instead of having all threads perform the same action (data parallelism), each thread specializes in a different operation. With three threads total:
- One thread counts the number of neighbours for each cell in the grid and stores it in a live/dead queue (indicating the cell will live/dead in the next generation)
- One thread reads through the live queue and updates the next generation with all live cells
- One thread reads through the dead queue and updates the next generation with all dead cells
All threads run simultaneously with each thread manipulating shared data. Therefore, mutex locks are used more frequently compared to the data parallel algorithm.

## How to run script:
- To compare runtimes for each program, you can run do_this_please.sh to view a report of each program running for 10 iterations with varying number of threads. To run the bash file, do the following command: `source do_this_please`

## How to run each individual program:
  1. Compile all programs (gol_serial.c, gol_data.c, and gol_task.c) by utilizing the make tool provided by the Makefile: `make` or `make all`
- Execute gol_serial and gol_task with the following command line arguments: `./<program_name> <grid_size> <num_iterations> <-d>`
- Execute gol_task with the following command line arguments: `./<program_name> <num_threads> <grid_size> <num_iterations> <-d>`
    - -d is an optional flag to view each iteration of the board
    - These command line arguments must be used in this order. Thorough error checking has not been implemented yet

#### Example command-line arguments:
- The following will step through (and display) the first 5 generations in a randomly generated 10x10 grid:
```
./gol_serial 10 5 -d
```
- The following will step through (and display) the first 200 generations in a randomly generated 100x100 grid using 4 threads:
```
./gol_data 4 100 200
```
- The following will step through (but not display) the first 100 generations in a randomly generated 50x50 grid:
```
./gol_task 50 100
```