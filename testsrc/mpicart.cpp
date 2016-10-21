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
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <algorithm>

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

class pretty;

std::ostream& operator<< ( std::ostream& os, const pretty& data );

class pretty {
  public:
    pretty ( const std::vector<int>& data, bool showpos=false, const char sep=','):
      _data(data), _showpos(showpos), _sep(sep){};
    friend std::ostream& operator<< ( std::ostream& os, const pretty& data );
  private:
    const std::vector<int>& _data;
    bool _showpos;
    const char _sep;
};

std::ostream& operator<< ( std::ostream& os, const pretty& data ){

  std::vector<int>::const_iterator it = data._data.begin();
  std::vector<int>::const_iterator it_end = data._data.end();

  std::ios::fmtflags f ( os.flags() );

  if ( data._showpos )
    os << std::showpos;

  for( ; it < it_end-1; ++it)
    os << std::setw(1) 
      << " " << *it << data._sep;

  os << std::setw(1) << " " << *it;

  os.flags( f ); 
  return os;
}

using namespace vector_helper;
                        
int main(int argc, char *argv[]){
  try {

    std::vector<int> dims      = { 1000, 1000, 1000 };
    std::vector<int> tileSplit = {    3,    3,    3 };
    std::vector<int> periodic  = {    0,    1,    0 }; 
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
      for( unsigned int ii = 0; it < it_end; ++ii, ++it )
        *it = cs.getRankByOffset( directions[ii] );

      // print my neighbours
      for( int rank = 0; rank < cs.getSize(); ++rank ){

        if ( cs.getRank() == rank ){
          cout << "Rank: " << rank << endl;
          for( unsigned int ii = 0; ii < neighbours.size(); ++ii ){
            cout << " Node " << std::setw(2) << cs.getRank() 
              << " of "  << std::setw(2) << cs.getSize()
              << " (" << pretty ( coords ) << " ) "
              << std::setw(2) << neighbours[ii] 
              << " (" << pretty ( directions[ii], true ) 
              << " ) " << endl; 
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

