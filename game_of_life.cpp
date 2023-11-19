/* Game of Life
Conway's Game Of Life is an easy realization of a cellular automaton, which
models dynamic but discrete systems. In this project, we create a simulation
that realizes such systems.

Background
We consider an arbitrary grid of squares. Each component of the grid is called a
(biological) cell. At the beginning (t=0), all cells are either "dead" or
"alive". Now we are going to add dynamics to the scene by applying the following
rules at any time step t = t + 1: A living cell

stays alive, if it hast exact 2 or 3 neighbors
dies due to loneliness if it has fewer neighbors
dies du to overpopulation if it has more neighbors

A dead cell

will be reborn if it has exactly 3 neighbors
remains dead otherwise

Be aware that updates to the cells are performed in parallel and that a neighbor
of a cell is defined by edges and vertices ,i.e. any cell that is not located at
the boundaries of the grid has exactly 8 neighbors.
*/

/* Sprint 1
In this sprint, we want to create a first prototype that simulates the system.
The basic logic needed to store, update and display grids of cells should be in
place by the end of the sprint.

Definition of "done":

Primitive data types for the grid and cells are specified and implemented
A file format to define grids of cells is specified. (this should be a simple
text-based format e.g. (0=dead, 1=alive), optimizations can come later) The
specified file format can be read into the specified data types (this tutorial
may be helpful), edge cases are handled appropriately The simulation per time
step as described above is implemented (hint: use an additional grid that holds
the grid at t+1) The grid gets printed to the console after every update Basic
test cases are implemented to verify that the simulation works as expected (some
grids found on wikipedia can be used) (optional) Unit tests are implemented
(preferably using a testing library like gtest )
*/

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

int g_timestep = 0;

// These functions are used to initialize the grid of cells.
// The first function initializes a square grid of N x N cells.
std::vector<std::vector<bool>> initialize_cells(int N, int seed = 0) {
  std::vector<std::vector<bool>> grid(N, std::vector<bool>(N, false));
  std::mt19937 gen(seed);
  std::uniform_int_distribution<> dis(0, 1);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      grid[i][j] = dis(gen);
    }
  }
  return grid;
}

// The second function initializes a grid of cells with row_num rows and
std::vector<std::vector<bool>> initialize_cells(int row_num, int column_num,
                                                int seed = 0) {
  std::vector<std::vector<bool>> grid(row_num,
                                      std::vector<bool>(column_num, false));
  std::mt19937 gen(seed);
  std::uniform_int_distribution<> dis(0, 1);
  for (int i = 0; i < row_num; i++) {
    for (int j = 0; j < column_num; j++) {
      grid[i][j] = dis(gen);
    }
  }
  return grid;
}

// Read the Cells_grid from a file.
std::vector<std::vector<bool>>
read_cells_grid_from_file(const std::string &file_name) {
  std::ifstream file(file_name);
  std::vector<std::vector<bool>> grid;
  std::string line;
  while (std::getline(file, line)) {
    std::vector<bool> row;
    for (auto c : line) {
      if (c == '0') {
        row.push_back(false);
      } else if (c == '1') {
        row.push_back(true);
      }
    }
    grid.push_back(row);
  }
  return grid;
}

// This function is used to count the number of neighbors of each cell.
std::vector<std::vector<int>>
count_neighbors_num(const std::vector<std::vector<bool>> &grid) {
  int row_num = grid.size();
  int column_num = grid[0].size();
  std::vector<std::vector<int>> neighbor_num(row_num,
                                             std::vector<int>(column_num, 0));
  for (int i = 0; i < row_num; i++) {
    for (int j = 0; j < column_num; j++) {
      int neighbor_count = 0;
      for (int k = -1; k <= 1; k++) {
        for (int l = -1; l <= 1; l++) {
          if (k == 0 && l == 0) {
            continue;
          }
          int row_index = i + k;
          int column_index = j + l;
          if (row_index < 0 || row_index >= row_num || column_index < 0 ||
              column_index >= column_num) {
            continue;
          }
          if (grid[row_index][column_index]) {
            neighbor_count++;
          }
        }
      }
      neighbor_num[i][j] = neighbor_count;
    }
  }
  // print out the neighbor_num
  // std::cout << "Neighbor_num at timesteps: " << g_timestep << std::endl;
  // for (auto row : neighbor_num) {
  //   for (auto cell : row) {
  //     std::cout << cell << " ";
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << "<------------------------>" << std::endl;
  return neighbor_num;
}

// This function checks if the grid of cells is empty.
bool all_cells_dead(const std::vector<std::vector<bool>> &grid) {
  for (const auto &row : grid) {
    for (const auto &cell : row) {
      if (cell) {
        return false;
      }
    }
  }
  std::cout << "All cells are dead at time step: " << g_timestep << std::endl;
  return true;
}

// Display the grid of cells.
void display_cells(const std::vector<std::vector<bool>> &grid) {
  int N = grid.size();
  std::cout << "Cells_grid at time step: " << g_timestep << std::endl;
  for (auto row : grid) {
    for (auto cell : row) {
      std::cout << cell << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

// This function is used to update the grid of cells.
// Every cell is updated simultaneously onece per time step.
void update_cells(std::vector<std::vector<bool>> &grid) {
  int N = grid.size();
  std::vector<std::vector<int>> neighbor_num = count_neighbors_num(grid);
  std::vector<std::vector<bool>> new_grid(
      size(grid), std::vector<bool>(size(grid[0]), false));
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (neighbor_num[i][j] == 3) {
        new_grid[i][j] = true;
      } else if (neighbor_num[i][j] == 2) {
        new_grid[i][j] = grid[i][j];
      } else {
        new_grid[i][j] = false;
      }
    }
  }
  grid = new_grid;
  g_timestep++;
}

int main() {
  // std::vector<std::vector<bool>> cells_grid = initialize_cells(10);
  std::vector<std::vector<bool>> cells_grid =
      read_cells_grid_from_file("./cells_grid.txt");
  while (true) {
    display_cells(cells_grid);
    update_cells(cells_grid);
    if (all_cells_dead(cells_grid)) {
      break;
    }
    g_timestep++;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}
