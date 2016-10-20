/**
 * @file CartSplitter.hpp
 * @author Riccardo Zanella
 * @date 02/2016
 *
 * Contacts: riccardo.zanella@unife.it
 *
 */

#ifndef CARTSPLITTER_HPP 
#define CARTSPLITTER_HPP 

#include <vector>
#include <stdexcept>

#include "vector_helper.hpp"
#include "DistributedDescription.hpp"

#include "mpi.h"
/*
template <typename T>
struct mpi_info {
static const MPI_Datatype mpi_datatype;
};
*/


// forward declare DistributedDescription
template <typename T>
class DistributedDescription;

/**
 *  Cartesian Topology Splitter
 */
class CartSplitter {
  private:
    std::vector<int> _dims;         //!< dimensions
    std::vector<int> _periodicity;  //!< periodicity ( 1=periodic 0=not periodic)
    MPI_Comm _comm;                 //!< communicator with cartesian topology
    int _reorder;                   //!< MPI can reorder nodes in new comm
    bool _inGrid;                   //!< true if I'm in the grid
    int _cartRank;                  //!< rank of current node in cart comm
    int _cartSize;                  //!< size of current cart comm
    std::vector<int> _coordinates;  //!< coodinates of current node in cart comm

    /**
      * when considering a direction _directions[ii] we 
      * exploit a send-receive MPI_function as follows:
      *
      * _srcNeighbours[ii] -> me -> _destNeigbours[ii]
      *
      * If any od _srcNeighbours[ii] or _destNeighbours[ii] is
      * not valid, it's value is set to MPI_PROC_NULL.
      */ 
    std::vector< std::vector<int> > _directions; //!< directions to reach first neigh.
    std::vector< int > _destNeighbours; 
    std::vector< int > _srcNeighbours;

    CartSplitter ( const CartSplitter& );
    CartSplitter& operator= ( const CartSplitter& );

    /**
     * Fill directions for
     * first neighbours, when coordinate size is d
     *
     * @param d
     * @return vector of directions: each element is a 
     * vector of size d, with at least one element
     * not zero.
     *
     * Example:
     * d = 2;
     * output size:  3^2 - 1 elements
     * output: 
     * { { -1, -1 }, 
     *   { -1, +1 }, 
     *   { -1,  0 },
     *   { +1, -1 },
     *   { +1, +1 },
     *   { +1,  0 },
     *   {  0, -1 },
     *   {  0, +1 } }
     *
     */
    void fillDirections( int d );

  public:
    /** 
      * Creates a Cartesian Splitter
      * @param dims dimensions of the grid
      * @param periodicity periodicity for each direction
      * @param comm original communicator 
      * @param reorder reorder flag
      *
      * Must be called by every node in original communicator.
      * Creates a communicator with cartesian topology.
      *
      * If the number of nodes in original communicator is not sufficient 
      * to fill the requested grid a runtime_error exception is thrown
      *
      * If the number of nodes in original communicator is greater,
      * some nodes are not inserted in the grid.
      * For checking if current node is in the grid, use inGrid() method.
      *   
      */
    CartSplitter( const std::vector<int>& dims,
                  const std::vector<int>& periodicity,
                  MPI_Comm comm, int reorder = 1 );
    ~CartSplitter( ); 
  
    /**
      * Returns MPI communicator (debug use only)
      * @return MPI_comm with requested cartesian topology
      */ 
    MPI_Comm getCommunicator() const {
      if ( !_inGrid )
        throw std::runtime_error
          ("CartSplitter::getCommunicator() called in node outside topology");
      return _comm; 
    } 
  
    /**
     * Returns a handle to precomputed directions
     * used for reaching first neighbours
     * @return const handle to direction vector 
     *
     * direction ii is the one exploited in:
     * srcNeighbours[ii] -> me -> destNeigbours[ii]
     */ 
    const std::vector< std::vector<int> >& getDirections() const {
        return _directions;
    }
    
    /**
     * Returns a handle to precomputed source neighbours
     * @returns handle to source neighbours
     *
     */  
    const std::vector< int >& getSrcNeighbours() const {
        return _srcNeighbours;
    }

    /**
     * Returns a handle to precomputed destination neighbours
     * @returns handle to destination neighbours
     *
     */  
    const std::vector< int >& getDestNeighbours() const {
        return _destNeighbours;
    }

    /**
      * Returns true if current node is in the grid
      * @return true/false
      */ 
    bool inGrid() const { return _inGrid; } 
    
    /**
      * Returns the number of nodes in new communicator
      * @return size of new communicator
      */
    int getSize() const {   
      if ( !_inGrid )
        throw std::runtime_error
          ("CartSplitter::getSize() called in node outside topology");
        return _cartSize; 
    }

    /**
      * Returns grid dimension 
      */
    std::vector<int> getDims() const {   
      if ( !_inGrid )
        throw std::runtime_error
          ("CartSplitter::getDims() called in node outside topology");
        return _dims;
    }

   /**
      * Returns the rank of the current node.  If grid is 
      * obtained with reorder=1, rank in the new communicator may be
      * different than rank in original one.
      * @return rank in new communicator
      */
    int getRank() const { 
      if ( !_inGrid )
        throw std::runtime_error
          ("CartSplitter::getRank() called in node outside topology");
      return _cartRank; 
    }
    
    /**
      * Given a vector of coordinates, returns the rank of the corresponding node
      * @param coordinates
      * @return rank ( may be MPI_PROC_NULL if given coordinates are not in the grid )
      */
    int getRank ( const std::vector<int>& coordinates ) const ;

    /**
      * Given a vector of offsets, returns the rank of the corresponding node
      * @param offset
      * return rank ( may be MPI_PROC_NULL if computed coordinates are not in the grid )
      */
    int getRankByOffset ( const std::vector<int>& offset ) const;

    /**
      * Returns the coordinates of current node
      * @return returns a copy of the vector of coordinates
      */
    std::vector<int> getCoordinates() const { 
      if ( !_inGrid )
        throw std::runtime_error
          ("CartSplitter::getCoordinates() called in node outside topology");
      return _coordinates; 
    }

    /**
      * Returns the coordinates of node having provided rank
      * @param rank  
      * @return returns a copy of the vector of coordinates
      */
    std::vector<int> getCoordinates ( int rank ) const;

    /**
      * Checks wether coordinate vector is in the grid
      * @param coords provided coordinates
      * @return true/false
      * 
      * Coordinates are checked only in non periodic directions.
      */
    bool coordsCheck ( const std::vector<int>& coords ) const;

    /**
      * Evaluates local dimensions and offsets for N-dim signal 
      * whose sizes are in dataDims
      * @param dataDims sizes for each direction (contiguous data on last direction)
      * @param localDims local data dimensions for each node 
      * @param localOffset offset for each dimension in each node
      */
    void evalDimsOffsets ( const std::vector<int>& dataDims, 
                       std::vector< std::vector<int> >& localDims,
                       std::vector< std::vector<int> >& localOffset ) const;

    /**
      * Synchronization barrier on nodes in cart
      * must be called by all nodes in cart
      */
    void barrier() const {
      mpiSafeCall( MPI_Barrier( _comm ) );
    }

    /**
     * Creates an instance of DistributedDescription class 
     * @param dims dimension of nd-data to be distributed
     * @param haloPre number of elements in halo, 
     * before internal data 
     * @param haloPost number of elements in halo, 
     * after internal data 
     * @param haloType ( 1=no halos, 2=full, 3=tight)
     * create a distributed description for data
      *
      */ 
    template <typename T>
    DistributedDescription<T>* 
    createDistributedDescription( const std::vector<int>& dims,
        const std::vector<int>& haloPre,
        const std::vector<int>& haloPost,
        HaloType::type haloType = HaloType::Full );

    /**
     * Creates an instance of DistributedDescription class 
     * @param dims dimension of nd-data to be distributed
     * @param haloPre number of elements in halo, 
     * before internal data (for all directions)
     * @param haloPost number of elements in halo, 
     * after internal data (for all directions)
     * @param haloType ( HaloType::Full or HaloType::Tight )
     */ 
    template <typename T>
    DistributedDescription<T>* 
    createDistributedDescription( const std::vector<int>& dims,
        int haloPre = 0,
        int haloPost = 0,
        HaloType::type haloType = HaloType::Full );

    /**
     * Scatters data contained in data
     * @param data source data (must be valid at root)
     * @param localData local data to be filled (must be allocated
     * correctly: see DistributedDescription.getLocalSize() )
     * @param root source node
     * @param dd pointer to DistributedDescription
     *
     * This methods scatters only the internal portion, see
     * haloUpdate.
     */ 
    template <typename T>
    void scatter( const std::vector<T>& data,
                  std::vector<T>& localData, int root,
                  const DistributedDescription<T> * dd );
    
    /**
     * Gathers internal part of localData
     * @param localData source data (must be valid for all nodes)
     * @param newData data to be filled (must be allocated at root,
     * correctly: see DistributedDescription.getTotalSize() )
     * @param root destination node
     * @param dd pointer to DistributedDescription
     */ 
    template <typename T>
      void gather( const std::vector<T>& localData, 
          std::vector<T>& newData, 
          int root, const DistributedDescription<T> * dd );
    
    /**
     * Starts neighbours data exchange for halo filling 
     * @param localData source data (must be valid for all nodes)
     * @param dd pointer to DistributedDescription
     */ 
    template <typename T>
      void haloUpdate( std::vector<T>& localData, 
          const DistributedDescription<T> * dd );

};

template <typename T>
DistributedDescription<T>* 
    CartSplitter::createDistributedDescription( const std::vector<int>& dims,
        const std::vector<int>& haloPre,
        const std::vector<int>& haloPost,
        HaloType::type haloType  ){

      DistributedDescription<T> * dd = new DistributedDescription<T>( dims );

      // evaluates internal sizes and offsets for each node
      evalDimsOffsets( dims, dd->_subSizes, dd->_starts );

      // creates MPI_Datatype for each internal portion ( root side )
      dd->fillInternalTypes();

      // evaluates local halo dimensions 
      dd->fillHaloSizes( haloPre, haloPost, haloType, _coordinates, _dims );

      // creates MPI_Datatype for internal portion ( local side )
      dd->fillLocalSizes( _cartRank );

      // creates local type for scatter/gather
      dd->fillLocalType();

      // creates halo types
      dd->fillHaloTypes( _directions );


      return dd;
}

template <typename T>
DistributedDescription<T>* 
    CartSplitter::createDistributedDescription( const std::vector<int>& dims,
        int haloPre,
        int haloPost,
        HaloType::type haloType  ){
   
      std::vector<int> v_haloPre( dims.size(), haloPre );
      std::vector<int> v_haloPost( dims.size(), haloPost );

      return createDistributedDescription<T> ( dims, v_haloPre, 
          v_haloPost, haloType ); 
}

template <typename T>
void CartSplitter::scatter( const std::vector<T>& data,
    std::vector<T>& localData, int root,
    const DistributedDescription<T> * dd )
{

  // data distribution ( needs types, data, data type, localData )
  if ( root == _cartRank ){

    std::vector< MPI_Request > requests( _cartSize );
    for ( int node = 0; node < _cartSize; ++node ) 
      mpiSafeCall( MPI_Isend( &data[0], 1, dd->_types[node], 
            node, 333, _comm, &requests[node] ) );

    MPI_Status status; 
    for ( int node = 0; node < _cartSize; ++node ){
      if ( node != root )
        mpiSafeCall( MPI_Wait( &requests[node], &status ) );    
    }

    // my matching receive
    mpiSafeCall( MPI_Recv( &localData[0], 1, dd->_localDatatype, 
          root, 333, _comm, &status ) );

    // collect the status of root->root 
    mpiSafeCall( MPI_Wait( &requests[root], &status ) );    

  }
  else{    
    // data receive
    MPI_Status status;
    mpiSafeCall( MPI_Recv( &localData[0], 1, dd->_localDatatype, 
          root, 333, _comm, &status ) );
  }

}


template <typename T>
void CartSplitter::gather( const std::vector<T>& localData, 
    std::vector<T>& newData, 
    int root, const DistributedDescription<T> * dd ){

  // data collection ( needs types, newdata, data type, localData )
  if ( root == _cartRank ){

    std::vector< MPI_Request > requests( _cartSize );
    for ( int node = 0; node < _cartSize; ++node ) 
      mpiSafeCall( MPI_Irecv( &newData[0], 1, dd->_types[node], 
            node, 666, _comm, &requests[node] ) );

    // my matching send
    mpiSafeCall( MPI_Send( &localData[0], 1, dd->_localDatatype, 
          root, 666, _comm ) );

    MPI_Status status; 
    for ( int node = 0; node < _cartSize; ++node ){
      if ( node != root )
        mpiSafeCall( MPI_Wait( &requests[node], &status ) );    
    }

    // collect the status of ROOT->ROOT 
    mpiSafeCall( MPI_Wait( &requests[root], &status ) );    

  }
  else{    
    // data send
    mpiSafeCall( MPI_Send( &localData[0], 1, dd->_localDatatype, 
          root, 666, _comm ) );
  }

}

template <typename T>
void CartSplitter::haloUpdate( std::vector<T>& localData, 
          const DistributedDescription<T> * dd ){
  
  for( unsigned int ii = 0; ii < _directions.size(); ++ii ){
        MPI_Status status;
        int sendcnt = 0, recvcnt = 0;
        MPI_Datatype sendtype = MPI_INT, recvtype = MPI_INT;

        if ( _destNeighbours[ii] != MPI_PROC_NULL && dd->_sendTypes[ii] != 0 ){
           sendcnt = 1;
           sendtype = dd->_sendTypes[ii];
        }
        if ( _srcNeighbours[ii] != MPI_PROC_NULL && dd->_receiveTypes[ii] != 0 ){
           recvcnt = 1;
           recvtype = dd->_receiveTypes[ii];
        }

        mpiSafeCall( MPI_Sendrecv( &localData[0], sendcnt, sendtype, 
              _destNeighbours[ii], 11, 
              &localData[0], recvcnt, recvtype, _srcNeighbours[ii], 11,  
              _comm, &status) );
      }
      

}

#endif // CARTSPLITTER_HPP

