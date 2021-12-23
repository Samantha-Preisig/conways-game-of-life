Conway's Game of Life consists of a two-dimension grid of cells. Each cell is either alive or dead with eight neighbours Each neighbouring cell is either horizontal, vertical, or diagonally adjacent.

At each step in time (or tick/generation), the following transitions occur:
  1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
  2. Any live cell with two or three live neighbours lives on to the next generation.
  3. Any live cell with more than three live neighbours dies, as if by overpopulation.
  4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

The intial pattern consistutes the seed of the system. The first generation is created by applying the above rules simultaneously to every cell in the seed; births and deaths occur simultaneously.

Reference: https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

How to run the program:
  1. Compile the cgol.c program by utilizing the make tool provided by the Makefile. To do this, type 'make cgol' in the command line.
  2. Execute the program with the desired seed. All seed files will have the file extension ".seed". An example: './cgol diehard20X40.seed' runs this program using the seed "diehard20X40.seed". If you would like to see a specific number of generations, enter the number or generations as the thrid command line argument (e.g. './cgol diehard20X40.seed' 20' will step through the first 20 generations in "diehard20X40.seed"). If no number is given on the command line, the program will run to the first 50 generations and ask the user if they want to continue. If all cells die out before 50 generations, the program will halt accordingly.
  4. The program will display the starting generation (generation 0) and will wait for you to initiate the game. (Enter Y/y to start, enter N/n to quit).
  5. If the seed makes it to 50 generations without all cells dying, the user will be asked if they want to continue for another 50 generations. (Enter Y/y to continue, enter N/n to quit).
