#include <iostream>
#include <cmath>
#include <mpi.h>

#include "solver.h"
#include "source.h"
#include "mesh.h"

/** Main driver function */
int main(int argc, char *argv[]) {
  
  // Initialize MPI
  MPI_Init(&argc, &argv);
  MPIUtil mpi{};
  if (mpi.rank == mpi.ROOT) {
    std::cout << "MPI tasks = " << mpi.np << std::endl;
  }
  
  // Initialize mesh
  if (mpi.rank == mpi.ROOT) {
    std::cout << "Initializing mesh..." << std::endl;
  }
  Point botLeft{0.0, 0.0, 0.0};
  Point topRight{1.0, 1.0, 1.0};
  int nx = 8;
  Mesh mesh{nx, nx, nx, botLeft, topRight, mpi};
  
  // Initialize sources
  int nsrcs = 1;
  Source::Params srcParams;
  srcParams.srcPos.realloc(Mesh::DIM, nsrcs);
  srcParams.srcPos(0,0) = 100.0;
  srcParams.srcPos(1,0) = 50.0;
  srcParams.srcAmps.realloc(nsrcs);
  srcParams.srcAmps(0) = 0.0;
  srcParams.type = Source::Wavelet::null;
  srcParams.halfSrc = 40;
  srcParams.maxF = 10.0;
  
  // Initialize solver
  if (mpi.rank == mpi.ROOT) {
    std::cout << "Initializing solver..." << std::endl;
  }
  int order = 4;
  double tf = 0.1;
  double dtSnap = 0.005;
  if (mpi.rank == mpi.ROOT) {
    std::cout << "order = " << order << std::endl;
    std::cout << "tf = " << tf << std::endl;
    std::cout << "nx = " << nx << std::endl;
  }
  Solver dgSolver{order, srcParams, dtSnap, tf, mesh};
  
  // Run time steps
  dgSolver.dgTimeStep();
  
  MPI_Finalize();
  return 0;
}
