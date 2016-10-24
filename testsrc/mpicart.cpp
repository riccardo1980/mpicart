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

template <typename T>
class pretty;

template <typename T>
std::ostream& operator<< ( std::ostream& os, 
    const pretty<T>& data );

template <typename T>
class pretty {
  public:
    explicit pretty( const std::vector<T>& data ) :
      _data( data ), _preamble(), _separator(), 
      _epilogue(), _showpos(false) {};
    friend 
      std::ostream& operator<<<> ( std::ostream& os,
          const pretty<T>& data );

    pretty<T>& preamble (const std::string& pre){
      _preamble = pre;
      return *this;
    } 

    pretty<T>& separator (const std::string& sep){
      _separator = sep;
      return *this;
    } 

    pretty<T>& epilogue (const std::string& epi){
      _epilogue = epi;
      return *this;
    } 

    pretty<T>& showpos (){
      _showpos = true;
      return *this; 
    } 

    pretty<T>& noshowpos (){
      _showpos = false;
      return *this; 
    } 

    pretty<T>& preamble (const char *pre){
      return preamble( std::string(pre) ); 
    } 

    pretty<T>& separator (const char *sep){
      return separator( std::string(sep) ); 
    } 

    pretty<T>& epilogue (const char *epi){
      return epilogue( std::string(epi) ); 
    } 

  private:
    std::string _preamble;
    std::string _separator;
    std::string _epilogue;
    bool _showpos;
    const std::vector<T>& _data;
};

template <typename T>
std::ostream& operator<< ( std::ostream& os, 
    const pretty<T>& data ){

  std::ios::fmtflags f ( os.flags() );

  if ( data._showpos )
    os << std::showpos;

  os << std::scientific;
  os << data._preamble;
  typename std::vector<T>::const_iterator it = data._data.begin(); 
  for ( ; it < data._data.end()-1; ++it )
   os << *it << data._separator;  

  os << *it << data._epilogue;
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
            cout << " Node " << cs.getRank() 
              << " of " << cs.getSize()
              << pretty<int>( coords ).preamble(" ( ").epilogue(" ) ").separator(", ")
              << neighbours[ii] 
              << pretty<int>( directions[ii] ).preamble(" ( ").epilogue(" ) ")
              .separator(", ").showpos() 
              << endl; 
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

