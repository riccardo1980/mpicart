/**
 * @file CartSplitter.cpp 
 * @author Riccardo Zanella
 * @date 03/2016
 *
 * Contacts: riccardo.zanella@gmail.com
 *
 */

#include <stdexcept>
#include <iostream>
#include <cmath>
#include <algorithm>


#include "safecheck.hpp"
#include "CartSplitter.hpp"
#include "DistributedDescription.hpp"


#include "vector_helper.hpp"

using std::runtime_error;
using std::vector;
using namespace vector_helper;

CartSplitter::CartSplitter( const std::vector<int>& dims,
                  const std::vector<int>& periodicity,
                  MPI_Comm origComm, int reorder ) 
  : _dims( dims ), _periodicity( periodicity ), _comm(0), _reorder( reorder ),
  _inGrid(false), _cartRank( MPI_PROC_NULL ), _cartSize(0), _coordinates(0),
  _directions( _dims.size() ), _destNeighbours(0), _srcNeighbours(0)
{

  if ( dims.size() != periodicity.size() )
    throw runtime_error("CartSplitter: dims and periodicity size mismatch");

  int commSize;
  MPI_Comm_size( origComm, &commSize );
  if( prod( dims.begin(), dims.end() ) > commSize )
    throw runtime_error("CartSplitter: not enough nodes in original communicator");
 
  _comm = MPI_COMM_NULL;

  mpiSafeCall( MPI_Cart_create( origComm, _dims.size(), 
          &_dims[0], &_periodicity[0], _reorder, &_comm ) );

  _inGrid = _comm != MPI_COMM_NULL;

  if ( _inGrid ){
    mpiSafeCall( MPI_Comm_rank ( _comm, &_cartRank ) );
    mpiSafeCall( MPI_Comm_size ( _comm, &_cartSize ) );
    _coordinates = getCoordinates( _cartRank );
    fillDirections( _dims.size() );
    int Ndirs = _directions.size();
    _destNeighbours = vector< int > ( Ndirs );
    _srcNeighbours = vector< int > ( Ndirs );
    for( int ii = 0; ii < Ndirs; ++ii ){
      _destNeighbours[ii] = getRankByOffset( _directions[ii] );
      _srcNeighbours[ii] = getRankByOffset( -1 * _directions[ii] );
    } 

  }
}

CartSplitter::~CartSplitter( ) {
  try {
    if( _inGrid )
      mpiSafeCall( MPI_Comm_free( &_comm ) );
  } catch ( std::exception &e ){
        std::cerr << "Errors on CartSplitter dtor: " 
          << e.what() << std::endl;
      }

} 

std::vector<int> CartSplitter::getCoordinates( int rank ) const {
  if ( !_inGrid )
    throw runtime_error(
        "CartSplitter::getCoodinates() called in node outside topology");

  if ( rank < 0 || rank > _cartSize )
    throw runtime_error("CartSplitter::getCoordinates() rank is not in the grid");

  std::vector<int> coords( _dims.size() );
  mpiSafeCall( MPI_Cart_coords( _comm, rank, coords.size(), &coords[0] ) ); 
  return coords; 
}

bool CartSplitter::coordsCheck( const std::vector<int>& coords ) const {
  unsigned int N = coords.size();
  if ( N != _dims.size() )
    throw runtime_error("CartSplitter::coordsCheck(): mismatch on vector sizes");

  bool passedTest = true;
  
  for ( unsigned int ii = 0; passedTest && ii < N; ++ii )
    passedTest = _periodicity[ii] || ( coords[ii] > -1 && coords[ii] < _dims[ii] );

  return passedTest;
}

int CartSplitter::getRank( const std::vector<int>& coordinates ) const{
  if ( !_inGrid )
    throw runtime_error(
        "CartSplitter::getRank() called in node outside topology");
  
  if ( coordinates.size() != _dims.size() )
    throw runtime_error(
        "CartSplitter::getRank() coordinates size mismatch");
 
  int ret = MPI_PROC_NULL; 
  if ( coordsCheck( coordinates ) ) 
    mpiSafeCall( MPI_Cart_rank( _comm, &coordinates[0], &ret ) );

  return ret;
}

int CartSplitter::getRankByOffset( const std::vector<int>& offset ) const {
  if ( !_inGrid )
    throw runtime_error(
        "CartSplitter::getRankByOffset() called in node outside topology");

  if ( offset.size() != _dims.size() )
    throw runtime_error(
        "CartSplitter::getRankByOffset() offset size mismatch");
 
  std::vector<int> coords( _coordinates + offset );
 
  return getRank( coords );

}

void CartSplitter::evalDimsOffsets ( const std::vector<int>& dataDims, 
                       std::vector< std::vector<int> >& localDims,
                       std::vector< std::vector<int> >& localOffset ) const{

  if ( !_inGrid )
    throw runtime_error(
        "CartSplitter::evalDimsOffsets() called in node outside topology");

  int D = dataDims.size();

  // floor division
  std::vector<int> tileSize = dataDims / _dims;

  // rem
  std::vector<int> reminder = dataDims % _dims;

  localDims.clear(); localDims.resize( _cartSize );
  localOffset.clear(); localOffset.resize( _cartSize );

  for ( int node = 0; node < _cartSize; ++node ){
    std::vector<int> coo = getCoordinates( node );
    localDims[node].clear(); localDims[node].resize( D );
    localOffset[node].clear(); localOffset[node].resize( D );

    for ( int dd = 0; dd < D; ++dd ) {
      localDims[node][dd] = tileSize[dd] + ( coo[dd] < reminder[dd] );      
      localOffset[node][dd] 
        = coo[dd] * tileSize[dd] + (coo[dd] < reminder[dd] ? coo[dd] : reminder[dd]);
    }    
    
  }


}


void CartSplitter::fillDirections( int d ){

  // 0 must be last item in alphabet
  vector<int> alphabet = { -1, +1, 0 };  

  // number of offsets, skipping { 0, ..., 0 }
  int N = int ( std::pow( alphabet.size() , d ) ) - 1; 
  _directions = vector< vector <int> >( N );

  for ( int ii = 0; ii < N; ++ii )
    _directions[ii] = vector<int> ( d );

  int burstSize = 1;
  for( int jj = 0; jj < d; ++jj){
    for ( int ii = 0; ii < N; ++ii ){
      _directions[ ii ] [ jj ] = alphabet[ ( ii / burstSize ) % alphabet.size()  ];
    }
    burstSize *= alphabet.size();
  }

  // {0, ..., 0 } is not generated
}


