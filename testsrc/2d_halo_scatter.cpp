/**
 * @file 2d_halo_scatter.cpp
 * @author Riccardo Zanella
 * @date 03/2016
 *
 * Contacts: riccardo.zanella@gmail.com
 *
 */

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cmath>

#include <vector>
#include <algorithm>
#include <map>

#include "mpi.h"

#include "safecheck.hpp"
#include "logger.hpp"
#include "vector_helper.hpp"
#include "CartSplitter.hpp"
#include "test_helpers.hpp"

using std::exception;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::runtime_error;

using namespace vector_helper;
//#define PRINT_ALL
//#define PRINT_LOCAL

static const std::map < std::string, HaloType::type,  case_insensitive_less > halo_set = { 
    {"NO", HaloType::Unused}, 
    {"FULL", HaloType::Full},
    {"TIGHT", HaloType::Tight} 
  };

int main (int argc, char *argv[]){
  try{
    mpiSafeCall( MPI_Init(&argc, &argv) );
    int worldRank, worldSize;
    mpiSafeCall( MPI_Comm_rank ( MPI_COMM_WORLD, &worldRank ) );
    mpiSafeCall( MPI_Comm_size ( MPI_COMM_WORLD, &worldSize ) );
   
    HaloType::type haloType = HaloType::Unused; 
    vector<int> tileSplit = { 3, 3 };
    vector<int> periodicity = { 0, 0 };
    vector<int> dims = { 1200, 1200 }; 
    int dh = 20; // default halo size

    if ( worldRank == 0) {
      switch ( argc ){
        case 2:
          try {
          haloType = valueFromKey( 
              std::string( argv[1]), halo_set );
          }
          catch ( exception& e ){
               std::stringstream ss;
               ss << e.what() << endl
                 << "halo type must be one of: "
                 << make_pretty(halo_set).tuple_separator(" | ").preamble("[ ")
                 .epilogue(" ]");
               throw runtime_error( ss.str() ); 
          }
      }
    }

    // Broadcast of test parameters
    MPI_Bcast( &haloType, 1, MPI_INT, 0, MPI_COMM_WORLD );

    vector<double> data; 
    CartSplitter cs( tileSplit, periodicity, MPI_COMM_WORLD );
    
    if ( cs.inGrid() ){
      MPI_Comm comm = cs.getCommunicator();

      int cartRank = cs.getRank();
      int cartSize = cs.getSize();

      int ROOT = 0;  // data generator
      int COLLECTROOT = 0; // data collector

      // ROOT fills data to be scattered
      if ( ROOT == cartRank ){
        data = vector<double>( prod( dims ) );

        for( int r = 0; r < dims[0]; ++r){
          for( int c = 0; c < dims[1]; ++c){
            //data[ r*dims[1] + c ] = (c > ( dims[1] +1 )/2 -1 ) + 2* ( r > ( dims[0] +1 )/2 -1);
            data[ r*dims[1] + c ] = 100*(r+1) + c;
          }
        }
        cout << "Node: " << cartRank 
          << " generates data " << dims << endl;
#ifdef PRINT_ALL
        matPrint( cout, data, dims, 4 );
#endif
      }

      DistributedDescription<double> * dd = 
        cs.createDistributedDescription<double>( dims, dh, dh, haloType ); 
      
      vector<double> localData( dd->getLocalSize( ) );
      const vector< int >& localDims = dd->getLocalDims(); 

      // data distribution
      cs.scatter( data, localData, ROOT, dd );
      
      // halo update from neighbours
      cs.haloUpdate( localData, dd );

#ifdef PRINT_LOCAL
      for( int node = 0; node < cartSize; ++node ){
        if ( node== cartRank ){
          cout << "Node: " << cartRank << " size: " << localDims << endl;
          matPrint( cout, localData, localDims, 4 );
        }
        cs.barrier();
      } 
#endif      

      vector<double> dataBack;
      if ( COLLECTROOT == cartRank ){
        dataBack = vector<double>( prod( dims ) );
      }
      
      // gathering internal portions
      cs.gather( localData, dataBack, COLLECTROOT, dd );

      // data print
      if ( COLLECTROOT == cartRank ) {
        cout << "Node: " << cartRank 
          << " collects data " << dims << endl;
#ifdef PRINT_ALL
        matPrint( cout, data, dims, 4 );
#endif
      }

      if ( COLLECTROOT != ROOT ){
      // move collected data to ROOT 
        if ( cartRank == ROOT ){
          dataBack = vector<double>( prod( dims ) );
          MPI_Status status;
          mpiSafeCall( MPI_Recv( &dataBack[0], data.size(), MPI_DOUBLE, COLLECTROOT, 11, comm, &status ) ); 
        } 
        if ( cartRank == COLLECTROOT ) {
          mpiSafeCall( MPI_Send( &dataBack[0], dataBack.size(), MPI_DOUBLE, ROOT, 11, comm ) ); 
        }
      }
     
      // ROOT checks for errors 
      if ( cartRank == ROOT ){
          int ee = 0; 
          for( unsigned int ii = 0; ii <  data.size() ; ++ii )
            ee += ( std::abs( dataBack[ii] - data[ii] ) / std::abs( data[ii] ) > 1e-12 );

          cout << "Errors: " << ee << endl;
      }

      delete dd;
    }
    else{
      std::stringstream ss;
      ss << "Node " << worldRank 
         << " / " << worldSize 
         << "(world) not in cart." << endl;
      cout << ss.str(); 
    }

    MPI_Barrier( MPI_COMM_WORLD );
  }
  catch ( exception &e){
    cerr << "Error: " << e.what() << endl;
    return (EXIT_FAILURE);
  } 

  mpiSafeCall( MPI_Barrier( MPI_COMM_WORLD ) );
  mpiSafeCall( MPI_Finalize() );
  return (EXIT_SUCCESS);
}

