/**
 * @file logger.cpp 
 * @author Riccardo Zanella
 * @date 03/2016
 *
 * Contacts: riccardo.zanella@gmail.com
 *
 */
#include <stdexcept>
#include <iostream>

#include "logger.hpp"

int Logger::log(const char *fmt, ...){
	va_list va;
	va_start(va, fmt);
	return std::vprintf(fmt, va);
}

int Logger::vlog(const char *fmt, va_list va){
	return std::vprintf(fmt, va);
}

int Logger::log(FILE *stream, const char *fmt, ...){
	va_list va;
	va_start(va, fmt);
	return std::vfprintf(stream, fmt, va);
}

int Logger::vlog(FILE *stream, const char *fmt, va_list va){
	return std::vfprintf(stream, fmt, va);
}

//////////////////////////////////////////////////////////////////////////

MPILogger::MPILogger(MPI_Comm comm, int root )
  : _rank(0), _np(0), _root ( root ), _comm(0) {
  mpiSafeCall( MPI_Comm_dup( comm, &_comm ) );
  mpiSafeCall(	MPI_Comm_rank( _comm, &_rank ) );
  mpiSafeCall(	MPI_Comm_size( _comm, &_np ) );
  if ( root != -1 && ( root < 0 || root > _np-1 ) )
    throw std::runtime_error("MPILogger: requested rank is outside boundaries!");
}

MPILogger::~MPILogger() {
  try {
  mpiSafeCall( MPI_Comm_free( &_comm ) ) ;
  } catch ( std::exception &e ){
    std::cerr << "Errors on MPILogger dtor: " 
      << e.what() << std::endl;
  }
}

int MPILogger::log(const char *fmt, ...){
  if ( _root == -1 )
    for ( int ii = 0; ii < _np; ++ii ){
      if (_rank == ii ){
        va_list va;
        va_start(va, fmt);
        Logger::vlog(fmt, va);
      }
      MPI_Barrier( _comm );
    }
  else
    if ( _rank == _root ){
      va_list va;
      va_start(va, fmt);
      return Logger::vlog(fmt, va);
    }
  return 0;
}

int MPILogger::log(FILE *stream, const char *fmt, ...){
  if ( _root == -1 )
    for ( int ii = 0; ii < _np; ++ii ){
      if (_rank == ii ){
        va_list va;
        va_start(va, fmt);
        Logger::vlog( stream, fmt, va);
      }
      MPI_Barrier( _comm );
    }
  else
    if ( _rank == _root ){
      va_list va;
      va_start(va, fmt);
      return Logger::vlog(stream, fmt, va);
    }
  return 0;
}

