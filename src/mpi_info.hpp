/**
 * @file mpi_info.hpp 
 * @author Riccardo Zanella
 * @date 03/2016
 *
 * Contacts: riccardo.zanella@unife.it
 *
 */

#ifndef MPI_INFO_HP
#define MPI_INFO_HP
#include "mpi.h"

/**
 * Trait class for matching
 * C++ types to mpi types
 */ 
template <typename T>
struct mpi_info {
static const MPI_Datatype mpi_datatype; //!< MPI_Datatype
};

/** char specialization */
template <>
struct mpi_info<char> {
static const MPI_Datatype mpi_datatype = MPI_CHAR; //!< MPI_Datatype
};

/** short specialization */
template <>
struct mpi_info<short> {
static const MPI_Datatype mpi_datatype = MPI_SHORT; //!< MPI_Datatype
};

/** int specialization */
template <>
struct mpi_info<int> {
static const MPI_Datatype mpi_datatype = MPI_INT; //!< MPI_Datatype
};

/** long specialization */
template <>
struct mpi_info<long> {
static const MPI_Datatype mpi_datatype = MPI_LONG; //!< MPI_Datatype
};

/** long long specialization */
template <>
struct mpi_info<long long> {
static const MPI_Datatype mpi_datatype = MPI_LONG_LONG; //!< MPI_Datatype
};

/** unsigned char specialization */
template <>
struct mpi_info<unsigned char> {
static const MPI_Datatype mpi_datatype = MPI_UNSIGNED_CHAR; //!< MPI_Datatype
};

/** unsigned short specialization */
template <>
struct mpi_info<unsigned short> {
static const MPI_Datatype mpi_datatype = MPI_UNSIGNED_SHORT; //!< MPI_Datatype
};

/** unsigned int specialization */
template <>
struct mpi_info<unsigned int> {
static const MPI_Datatype mpi_datatype = MPI_UNSIGNED; //!< MPI_Datatype
};

/** unsigned long specialization */
template <>
struct mpi_info<unsigned long> {
static const MPI_Datatype mpi_datatype = MPI_UNSIGNED_LONG; //!< MPI_Datatype
};

/** unsigned long long specialization */
template <>
struct mpi_info<unsigned long long> {
static const MPI_Datatype mpi_datatype = MPI_UNSIGNED_LONG_LONG; //!< MPI_Datatype
};

/** float specialization */
template <>
struct mpi_info<float> {
static const MPI_Datatype mpi_datatype = MPI_FLOAT; //!< MPI_Datatype
};

/** double specialization */
template <>
struct mpi_info<double> {
static const MPI_Datatype mpi_datatype = MPI_DOUBLE; //!< MPI_Datatype
};

/** long double specialization */
template <>
struct mpi_info<long double> {
static const MPI_Datatype mpi_datatype = MPI_LONG_DOUBLE; //!< MPI_Datatype
};

/*

   bool needs a complete specialization of methods,
   since vector<bool> C++ implementetion differs as 
   compared to other vectors 

*/

#ifdef Complex
/** complex float specialization */
template <>
struct mpi_info< Complex<float> > {
static const MPI_Datatype mpi_datatype = MPI_COMPLEX; //!< MPI_Datatype
};

/** complex double specialization */
template <>
struct mpi_info< Complex<double> > {
static const MPI_Datatype mpi_datatype = MPI_DOUBLE_COMPLEX; //!< MPI_Datatype
};

/** complex long double specialization */
template <>
struct mpi_info< Complex<long double> > {
static const MPI_Datatype mpi_datatype = MPI_LONG_DOUBLE_COMPLEX; //!< MPI_Datatype
};

#endif

#endif // MPI_INFO_HP


