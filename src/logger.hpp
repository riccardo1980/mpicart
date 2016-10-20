/**
 * @file logger.hpp
 * @author Riccardo Zanella
 * @date 02/2016
 *
 * Contacts: riccardo.zanella@unife.it
 *
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <mpi.h>
#include <cstdlib>

#include <cstdarg>
#include <cstdio>

#include "safecheck.hpp"

/**
  * logger base class
  */
class Logger {
  public:
    Logger(){};
    virtual ~Logger(){};
    /**
     * logging function
     * @param fmt format (like C printf) 
     */
    virtual	int log(const char *fmt, ...);
    /**
     * logging function
     * @param stream open file to write to
     * @param fmt format (like C printf) 
     */
    virtual	int log(FILE *stream, const char *fmt, ...);
  protected:

    /**
      * Variable number of input log function implementation
      * @param fmt format (like C printf)
      * @param va list of parameters
      */
    int vlog(const char *fmt, va_list va);

    /**
      * Variable number of input log function implementation
      * @param stream open file to write to
      * @param fmt format (like C printf)
      * @param va list of parameters
      */
    int vlog(FILE *stream, const char *fmt, va_list va);
};

/**
  * MPI logger base class
  */
class MPILogger : public Logger {
  private:
    int _rank;
    int _np;
    int _root;
    MPI_Comm _comm;
  public:
    /**
     * Creates a logger
     * @param comm MPI communicator
     * @param root 
     *
     * Root must be a valid rank in comm, or -1.
     * If root is -1, all nodes print the message, in rank order.
     * In this case, all nodes must call log function.
     * If root is a valid rank, only root will print the message.
     */
    MPILogger(MPI_Comm comm = MPI_COMM_WORLD, int root = -1);
    ~MPILogger();

    int log(const char *fmt, ...);
    int log(FILE *stream, const char *fmt, ...);
};


#endif // LOGGER_HPP

