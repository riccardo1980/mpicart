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
#include <cstdlib>
#include <cmath>

#include <vector>
#include <algorithm>

#include "mpi.h"

#include "safecheck.hpp"
#include "logger.hpp"
#include "vector_helper.hpp"
#include "CartSplitter.hpp"

using std::exception;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::runtime_error;

using namespace vector_helper;
//#define PRINT_ALL
//#define PRINT_LOCAL


int main (int argc, char *argv[]){
  try{
    mpiSafeCall( MPI_Init(&argc, &argv) );

    int ht = 1;

    // 1 no halo
    // 2 full halos
    // 3 tight halos

    if ( argc == 2)
      ht = atoi( argv[1] );
    
    int dh = 20; // default halo size

    HaloType::type haloType;
    switch (ht){
      case 1: haloType = HaloType::Unused; break;
      case 2: haloType = HaloType::Full; break;
      case 3: haloType = HaloType::Tight; break;
      default:
        throw runtime_error("Halo types 1 to 3");
    }

    vector<int> tileSplit = { 3, 3};
    vector<int> periodicity = { 0, 0};
    vector<int> dims = {1200,1200}; 
    vector<double> data; 


    CartSplitter cs( tileSplit, periodicity, MPI_COMM_WORLD );
    
    int worldRank, worldSize;
    mpiSafeCall( MPI_Comm_rank ( MPI_COMM_WORLD, &worldRank ) );
    mpiSafeCall( MPI_Comm_size ( MPI_COMM_WORLD, &worldSize ) );

    if ( cs.inGrid() ){
      MPI_Comm comm = cs.getCommunicator();

      int cartRank = cs.getRank();
      int cartSize = cs.getSize();

      int ROOT = 0;  // data generator
      int COLLECTROOT = 1; // data collector

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

      vector<double> newData;
      if ( COLLECTROOT == cartRank ){
        newData = vector<double>( prod( dims ) );
      }
      
      // gathering internal part
      cs.gather( localData, newData, COLLECTROOT, dd );

      // data print
      if ( COLLECTROOT == cartRank ) {
        cout << "Node: " << cartRank 
          << " collects data " << dims << endl;
#ifdef PRINT_ALL
        matPrint( cout, data, dims, 4 );
#endif
      }

      // check
      if ( cartRank == ROOT ){
        vector<double> dataBack ( data.size() ); 
        MPI_Status status;
        mpiSafeCall( MPI_Recv( &dataBack[0], data.size(), MPI_DOUBLE, COLLECTROOT, 11, comm, &status ) ); 

        int ee = 0; 
        for( unsigned int ii = 0; ii <  data.size() ; ++ii )
          ee += ( std::abs( dataBack[ii] - data[ii] ) / std::abs( data[ii] ) > 1e-12 );

        cout << "Errors: " << ee << endl;
      } 
      if ( cartRank == COLLECTROOT ) {
        mpiSafeCall( MPI_Send( &newData[0], newData.size(), MPI_DOUBLE, ROOT, 11, comm ) ); 
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

