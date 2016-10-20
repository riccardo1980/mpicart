/**
 * @file mpicart.cpp
 * @author Riccardo Zanella
 * @date 03/2016
 *
 * Contacts: riccardo.zanella@gmail.com
 *
 */

#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>

#include "mpi.h"
#include "safecheck.hpp"
#include "logger.hpp"
#include "CartSplitter.hpp"
#include "vector_helper.hpp"

using std::exception;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;


using namespace vector_helper;
                        
int main(int argc, char *argv[]){
  try {

    std::vector<int> dims      = { 1000, 1000 };
    std::vector<int> tileSplit = {    3,    3 };
    std::vector<int> periodic  = {    0,    0 }; 
    int reorder    = 1;


    mpiSafeCall( MPI_Init(&argc, &argv) );
    int worldRank, worldSize;
    MPI_Comm_rank ( MPI_COMM_WORLD, &worldRank );
    MPI_Comm_size ( MPI_COMM_WORLD, &worldSize );

    Logger Log;

    CartSplitter cs( tileSplit, periodic, MPI_COMM_WORLD, reorder );
    
    if ( cs.inGrid() ){

      // discover my coordinates 
      std::vector<int> coords = cs.getCoordinates();

      // get offsets
      vector< vector<int> > directions = cs.getDirections( );

      // get neighbours
      vector< int > neighbours ( directions.size() );

      vector<int>::iterator it = neighbours.begin();
      vector<int>::iterator it_end = neighbours.end();
      for( unsigned int ii = 0; ii < neighbours.size(); ++ii )
        neighbours[ii] = cs.getRankByOffset( directions[ii] );

      // print my neighbours
      for( int rank = 0; rank < cs.getSize(); ++rank ){
        if ( cs.getRank() == rank ){
          cout << "Rank: " << rank << endl;
          for( unsigned int ii = 0; ii < neighbours.size(); ++ii ){
            Log.log( " Node %2d of %2d ( %2d, %2d ) %2d ( %+2d, %+2d ) \n", 
                cs.getRank(), cs.getSize(), coords[0], coords[1], 
                neighbours[ii], directions[ii][0], directions[ii][1] );
          }
          cout << endl;
        }
        cs.barrier();
        //mpiSafeCall( MPI_Barrier( cs.getCommunicator() ) );
      }
    }
    else
      Log.log( " Node %2d of %2d not in grid\n", worldRank, worldSize );
    
  }
  catch ( exception &e){
    cerr << "Error: " << e.what() << endl;
    return (EXIT_FAILURE);
  } 
  
  mpiSafeCall( MPI_Barrier( MPI_COMM_WORLD ) );
  mpiSafeCall( MPI_Finalize() );
  return (EXIT_SUCCESS);
}

