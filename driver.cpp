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
  
  std::string mshFile{"/home/mfranco/repos/Local-DG/input/square.msh"};
  // TODO: readProps() not yet implemented!
  std::string vpFile{""};
  std::string vsFile{""};
  std::string rhoFile{""};
  
  Mesh mesh{mshFile, vpFile, vsFile, rhoFile, mpi};
  if (mpi.rank == mpi.ROOT) {
    std::cout << "dx: min = " << mesh.dxMin << ", max = " << mesh.dxMax << std::endl;
  }
  
  // Initialize sources
  int nsrcs = 1;
  Source::Params srcParams;
  srcParams.srcPos.realloc(Mesh::DIM, nsrcs);
  srcParams.srcPos(0,0) = 150.0;
  srcParams.srcPos(1,0) = 50.0;
  srcParams.srcAmps.realloc(nsrcs);
  srcParams.srcAmps(0) = 1.0;
  srcParams.type = Source::Wavelet::rtm;
  //srcParams.maxF = 10.0;
  
  // Initialize solver
  if (mpi.rank == mpi.ROOT) {
    std::cout << "Initializing solver..." << std::endl;
  }
  int order = 4;
  double tf = 1.0;
  double dtSnap = 0.05;
  
  if (mpi.rank == mpi.ROOT) {
    std::cout << "order = " << order << std::endl;
    std::cout << "tf = " << tf << std::endl;
    std::cout << "Loaded mesh = " << mshFile << std::endl;
  }
  Solver dgSolver{order, srcParams, dtSnap, tf, mesh};
  
  // Run time steps
  dgSolver.dgTimeStep();
  
  MPI_Finalize();
  return 0;
}
