/**
 * @file safecheck.hpp
 * @author Riccardo Zanella
 * @date 02/2016
 *
 * Contacts: riccardo.zanella@unife.it
 *
 */

#ifndef MPI_SAFECHECK_HPP
#define MPI_SAFECHECK_HPP

#ifdef DISABLE_MPI_CHECKS

#define mpiSafeCall( X ) do { } while(0) 

#else

#include <stdexcept>
#include <sstream>

/**
  * Macro for MPI errors checking
  * @param X MPI_Error
  */

#define mpiSafeCall( X )                                \
  do {                                                  \
     if ( X != MPI_SUCCESS ) {                          \
      std::stringstream ss;                             \
       ss << "MPI error on file " << __FILE__           \
          << " line " << __LINE__ << std::endl;         \
       int len;                                         \
       char e[MPI_MAX_ERROR_STRING];                    \
       MPI_Error_string( X, e, &len);                   \
       ss << e;                                         \
       throw std::runtime_error(  ss.str() );           \
     }                                                  \
  } while(0)

#endif //DISABLE_MPI_CHECKS
#endif // MPI_SAFECHECK_HPP


