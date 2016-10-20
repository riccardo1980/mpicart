/**
 * @file vector_helper.hpp 
 * @author Riccardo Zanella
 * @date 02/2016
 *
 * Contacts: riccardo.zanella@unife.it
 *
 */

#ifndef VECTOR_HELPER
#define VECTOR_HELPER

#include <iomanip>

/**
  * Contains simple helper functions and operators for vectors.
  */
namespace vector_helper {

  /**
   * Returns the product of the elements in range [first, last)
   * @param first iterator pointing first element
   * @param last iterator pointing one element past end of range 
   * @param init initial value [default=1]
   * @return product of elements in range [first, last)
   */
  template <typename InputIt>
    typename InputIt::value_type prod( InputIt first, InputIt last, 
        typename InputIt::value_type init = typename InputIt::value_type(1.0) ){

      for( ; first < last; ++first)
        init *= *first;

      return init;
    }

  /**
   * Retuns the product of the elements
   * @param v vector of elements
   * @return product
   */
  template <typename T>
    T prod( const std::vector<T>& v ){
      return prod( v.begin(), v.end() ); 
    }

  /** 
   * Returns c = a + b
   * @param a
   * @param b
   * @return c = a + b
   */
  template <typename T>
    std::vector<T> operator+ ( const std::vector<T>& a,
        const std::vector<T>& b){
      std::vector<T> res (a);
      typename std::vector<T>::iterator it = res.begin(); 
      typename std::vector<T>::const_iterator b_it = b.begin(); 
      typename std::vector<T>::iterator it_end = res.end(); 

      for( ; it < it_end; ++it, ++b_it )
        *it += *b_it;

      return res;
    }

  /** 
   * Returns c = a - b
   * @param a
   * @param b
   * @return c = a - b
   */
  template <typename T>
    std::vector<T> operator- ( const std::vector<T>& a,
        const std::vector<T>& b){
      std::vector<T> res (a);
      typename std::vector<T>::iterator it = res.begin(); 
      typename std::vector<T>::const_iterator b_it = b.begin(); 
      typename std::vector<T>::iterator it_end = res.end(); 

      for( ; it < it_end; ++it, ++b_it )
        *it -= *b_it;

      return res;
    }

  /** 
   * Returns c = num ./ den
   * @param num
   * @param den
   * @return c = num ./ den
   */
  template <typename T>
    std::vector<T> operator/ (const std::vector<T>& num, 
        const std::vector<T>& den){

      typename std::vector<T> res ( num );
      typename std::vector<T>::iterator it     = res.begin();
      typename std::vector<T>::const_iterator den_it = den.begin();
      typename std::vector<T>::iterator it_end = res.end();

      for( ; it < it_end; ++it, ++den_it )
        *it = *it / *den_it;

      return res;
    }

  /** 
   * Returns c = num % den
   * @param num
   * @param den
   * @return c = num % den
   */
  template <typename T>
    std::vector<T> operator% (const std::vector<T>& num, 
        const std::vector<T>& den){

      typename std::vector<T> res ( num );
      typename std::vector<T>::iterator it     = res.begin();
      typename std::vector<T>::const_iterator den_it = den.begin();
      typename std::vector<T>::iterator it_end = res.end();

      for( ; it < it_end; ++it, ++den_it )
        *it = *it % *den_it;

      return res;
    }

  /** 
   * Returns c * v 
   * @param c 
   * @param v 
   * @return c*v 
   */
  template <typename T>
    std::vector<T> operator* (const T& c, 
        const std::vector<T>& v){

      typename std::vector<T> res ( v );
      typename std::vector<T>::iterator it     = res.begin();
      typename std::vector<T>::iterator it_end = res.end();

      for( ; it < it_end; ++it  )
        *it *= c; 

      return res;
    }

  /**
   * Prints elements in range [first, last) 
   * @param os output stream to be used
   * @param first iterator pointing first element
   * @param last iterator pointing one element past end of range
   * @param w width field (default is 0) 
   */
  template <typename IT>
    void osPrint( std::ostream& os, IT first, IT last, int w = 0 ){
      if ( w == 0){
        for( ; first < last; ++first )
          os << *first << " "; 
      }
      else
        for( ; first < last; ++first )
          os << std::setw( w ) << *first << " "; 


    }

  /**
   * Prints vector elements
   * @param os output stream to be used
   * @param data vector 
   */
  template <typename T>
    std::ostream& operator<< ( std::ostream& os, const std::vector<T>& data ){

      osPrint( os, data.begin(), data.end() );

      return os;
    }


  /**
    * N-d array print
    * @param os output stream to be used
    * @param data vector 
    * @param split
    * @param w width 
    * a carriage return after split[0] elements
    */
  template <typename T>
    std::ostream& matPrint( std::ostream& os, const std::vector<T>& data, 
        const std::vector<int>& split, int w=0 ){

      if ( split.size() == 1 )
        os << data;
      else{
        int chunkSize = prod( split.begin()+1, split.end() );

        typename std::vector<T>::const_iterator it = data.begin();
        typename std::vector<T>::const_iterator it_end = data.end();

        for( ; it < it_end; it+=chunkSize ){
          osPrint( os, it, it+chunkSize, w ); 
          os << std::endl;
        }

      } 
      return os;

    }


} // end of namespace vector_helper


#endif // VECTOR_HELPER


