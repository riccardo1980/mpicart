/**
 * @file DistributedDescription.hpp
 * @author Riccardo Zanella
 * @date 03/2016
 *
 * Contacts: riccardo.zanella@unife.it
 *
 */

#ifndef DISTRIBUTED_DESCRIPTION_HPP
#define DISTRIBUTED_DESCRIPTION_HPP

#include "mpi.h"

template <typename T>
class DistributedDescription; 

#include "CartSplitter.hpp"
#include "vector_helper.hpp"
#include "mpi_info.hpp"

struct HaloType {
    enum type { Unused=0, Full=1, Tight=2 };
};

/**
 * This class provides a description of
 * the blocking procedure used to distribute data
 *
 */
template <typename T>
class DistributedDescription {
  private:
   // overall description: used by root in scatter/gather 
   std::vector< int > _dims;          //!< overall data dimension 
   std::vector< std::vector< int > > _subSizes; //!< internal size, for each node 
   std::vector< std::vector< int > > _starts;   //!< start of internal size, for each node

   std::vector< MPI_Datatype > _types; //!< MPI types to be used in scatter/gather by root 

   std::vector< int > _haloPre;  //!< requested halo size before internal part
   std::vector< int > _haloPost; //!< requested halo size after internal part

   // local description 
   std::vector< int > _localDims;  //! local size (internal + halos )
   std::vector< int > _localSubSizes; //! internal size
   std::vector< int > _localStarts;   //! start of internal size in local data

   std::vector< int > _localHaloPre; //! local values of halo pre
   std::vector< int > _localHaloPost; //! local values of halo post

   MPI_Datatype _localDatatype; //!< MPI types to be used in scatter/gather by not root 

   // halo types ( in same order as given directions )
   std::vector< MPI_Datatype > _sendTypes;    
   std::vector< MPI_Datatype > _receiveTypes;


   // constructor is private, CartSplitter is a friend
   friend class CartSplitter;

   DistributedDescription( const std::vector<int>& dims ) 
     : _dims( dims ), _subSizes(0), _starts(0), _types(0),
       _haloPre(0), _haloPost(0), _localDims(0), _localSubSizes(0),
       _localStarts(0), _localHaloPre(0), _localHaloPost(0), _localDatatype(0),
       _sendTypes(0), _receiveTypes(0) {};
   
   void fillInternalTypes();
  
   void fillHaloSizes( const std::vector<int>& haloPre, 
       const std::vector<int>& haloPost, HaloType::type haloType,
       const std::vector<int>& coords, 
       const std::vector<int>& gridDims ); 

   void fillLocalSizes( int rank ); 

   void fillLocalType(); 

   void fillHaloTypes ( const std::vector< std::vector<int> >& dirs ); 

  public:
    ~DistributedDescription () {

      for( unsigned int ii = 0; ii < _receiveTypes.size(); ++ii ){
        if ( _receiveTypes[ii] != 0 )
          mpiSafeCall( MPI_Type_free( &_receiveTypes[ii] ) );
      } 

      for( unsigned int ii = 0; ii < _sendTypes.size(); ++ii ){
        if ( _sendTypes[ii] != 0 )
          mpiSafeCall( MPI_Type_free( &_sendTypes[ii] ) );
      }
    
      mpiSafeCall( MPI_Type_free( &_localDatatype ) );

      for( unsigned int ii = 0; ii < _types.size(); ++ii ){
        if ( _types[ii] != 0 )
          mpiSafeCall( MPI_Type_free( &_types[ii] ) );
      } 
    
    }; 
    
    /**
     * Returns the number of elements (internal+halos) in local buffer
     * @return number of elements 
     */ 
    size_t getLocalSize() const { 
      using vector_helper::prod;
      return prod ( _localDims );
    }

    /**
     * Returns the number of elements that will be collected in
     * gather ( sum of internal elements of all nodes).  
     * @return number of elements 
     */
    size_t getTotalSize() const { 
      using vector_helper::prod;
      return prod ( _dims );
    }


    /**
     * Returns a handle to local dimension vector
     * @return size for each dimension (last is contiguous dimension)
     */ 
    const std::vector<int>& getLocalDims() const {
        return _localDims;
    }


    /**
     * Returns a handle to local internal dimension vector
     * @return size for each dimension (last is contiguous dimension)
     */ 
    const std::vector<int>& getLocalSubsizes() const {
        return _localSubSizes;
    } 


}; 

template <typename T>
void DistributedDescription<T>::fillInternalTypes() {

  int cartSize = _subSizes.size(); 
  _types.resize( cartSize );

  for ( int node = 0; node < cartSize; ++node ){
    mpiSafeCall( MPI_Type_create_subarray( _dims.size(), &_dims[0], &_subSizes[node][0], 
          &_starts[node][0], MPI_ORDER_C, mpi_info<T>::mpi_datatype, &_types[node] ) );
    mpiSafeCall( MPI_Type_commit( &_types[node] ) );
  }

}   

template<typename T>
void DistributedDescription<T>::fillHaloSizes( const std::vector<int>& haloPre, 
       const std::vector<int>& haloPost, HaloType::type haloType,
       const std::vector<int>& coords, 
       const std::vector<int>& gridDims ) {

     _haloPre = haloPre;
     _haloPost = haloPost;

     _localHaloPre = _haloPre;
     _localHaloPost = _haloPost;

     switch (haloType) {
       case HaloType::Unused:
         _haloPre = std::vector<int>( _dims.size(), 0);
         _haloPost = _haloPre;
         _localHaloPre = _haloPre;
         _localHaloPost = _haloPost;
         break; 
       case HaloType::Full: // FULL_HALOS: no need to modify values
         break;
       case HaloType::Tight: // TIGHT HALOS: no halos on cart boundaries
         {
           unsigned int D = coords.size();
           for (unsigned int dd = 0; dd < D; ++dd){
             _localHaloPre[dd] = ( coords[dd] > 0 ) ? _haloPre[dd] : 0;
             _localHaloPost[dd] = ( coords[dd] < gridDims[dd]-1 ) ?  _haloPost[dd] : 0;
           } 
         }
     }
   
   }

template<typename T>
void DistributedDescription<T>::fillLocalSizes( int rank ){
     using vector_helper::operator+;
     _localDims =  _subSizes[ rank ] + _localHaloPre + _localHaloPost ;
     _localSubSizes = _subSizes[ rank ];
     _localStarts = _localHaloPre; 
   } 

template<typename T>
void DistributedDescription<T>::fillLocalType() {
     
     mpiSafeCall( MPI_Type_create_subarray( _localDims.size(), &_localDims[0],
        &_localSubSizes[0], &_localStarts[0], MPI_ORDER_C, mpi_info<T>::mpi_datatype,
        &_localDatatype ) ); 
     mpiSafeCall( MPI_Type_commit( &_localDatatype ) );
   
   }

template<typename T>
void DistributedDescription<T>::fillHaloTypes( 
    const std::vector< std::vector<int> >& dirs ) {
  using vector_helper::prod;
  using vector_helper::operator-;

  const int Ndirs = dirs.size();

  // receive types
  _receiveTypes.resize( Ndirs );
  for( int ii = 0; ii < Ndirs; ++ii ){

    _receiveTypes[ii] = 0; // default value for unused datatype
    const std::vector<int>& off = dirs[ii];

    std::vector<int> start_coo ( off.size() );
    for( unsigned int dd = 0; dd < off.size(); ++dd ){
      switch ( off[dd] ){
        case +1: start_coo[dd] = 0; break;
        case  0: start_coo[dd] = _localStarts[dd]; break;
        case -1: start_coo[dd] = _localStarts[dd] + _localSubSizes[dd]; break;
        default:
                 throw std::runtime_error(" offset not handled"); 
      }
    }

    // one past last element in halo chunk
    std::vector<int> end_coo ( off.size() );
    for( unsigned int dd = 0; dd < off.size(); ++dd ){
      switch ( off[dd] ){
        case +1: end_coo[dd] = _localStarts[dd]; break;
        case  0: end_coo[dd] = _localStarts[dd] + _localSubSizes[dd]; break; 
        case -1: end_coo[dd] = _localDims[dd]; break;
        default:
                 throw std::runtime_error(" offset not handled"); 
      }
    }


    std::vector<int> halo_size = end_coo - start_coo;
    if ( prod( halo_size ) ) { 
      mpiSafeCall( MPI_Type_create_subarray( _localSubSizes.size(), 
            &_localDims[0], &halo_size[0], 
            &start_coo[0], MPI_ORDER_C, mpi_info<T>::mpi_datatype,
            &_receiveTypes[ii] ) );
      mpiSafeCall( MPI_Type_commit( &_receiveTypes[ii] ) );
    }
  }


  // send types
  _sendTypes.resize ( Ndirs ); 
  for( int ii = 0; ii < Ndirs; ++ii ){

    _sendTypes[ii] = 0; // default value for unused datatype

    const std::vector<int>& off = dirs[ii];

    std::vector<int> start_coo ( off.size() );
    for( unsigned int dd = 0; dd < off.size(); ++dd ){
      switch ( off[dd] ){
        case -1: start_coo[dd] = _localStarts[dd]; break;
        case  0: start_coo[dd] = _localStarts[dd]; break;
        case +1: start_coo[dd] = _localStarts[dd] 
                 + _localSubSizes[dd] - _haloPre[dd]; break;
        default:
                 throw std::runtime_error(" offset not handled"); 
      }
    }

    // one past last element in halo chunk
    std::vector<int> end_coo ( off.size() );
    for( unsigned int dd = 0; dd < off.size(); ++dd ){
      switch ( off[dd] ){
        case -1: end_coo[dd] = _localStarts[dd] + _haloPost[dd]; break; 
        case  0: end_coo[dd] = _localStarts[dd] + _localSubSizes[dd]; break; 
        case +1: end_coo[dd] = _localStarts[dd] + _localSubSizes[dd]; break; 
        default:
                 throw std::runtime_error(" offset not handled"); 
      }
    }


    std::vector<int> halo_size = end_coo - start_coo;
    if ( prod( halo_size ) ) { 
      mpiSafeCall( MPI_Type_create_subarray( _localSubSizes.size(), 
            &_localDims[0], &halo_size[0], 
            &start_coo[0], MPI_ORDER_C, mpi_info<double>::mpi_datatype, 
            &_sendTypes[ii] ) );
      mpiSafeCall( MPI_Type_commit( &_sendTypes[ii] ) );
    }
  } 
}

 
#endif //  DISTRIBUTED_DESCRIPTION_HPP

