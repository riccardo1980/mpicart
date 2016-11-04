#ifndef PARSING_HELPERS_HPP
#define PARSING_HELPERS_HPP

#include <stdexcept>
#include <vector>
#include <map>
#include <string>
#include <sstream>

template <typename T>
void vectorFromString( std::vector < T >& TN, 
    const std::string & s, const std::string delimiter = "x")
{
  size_t		last = 0;
  size_t		next = 0;
  std::string token;
  TN.clear();

  while ((next = s.find(delimiter, last)) != std::string::npos) {
    token = s.substr(last, next - last);
    T temp;
    std::istringstream( token ) >> temp;
    TN.push_back( temp );
    last = next + 1;
  }
  
  T temp;
  token = s.substr(last, next - last);
  std::istringstream( token ) >> temp;
  TN.push_back( temp );
}

/**
 * Controls the string containing requested halo type and
 * sets output accordingly
 *
 * @param in const string storing halo command line parameter
 * @return requested halo type
 *
 * If uppercase of input string is gound in global halo_set map,
 * corrispondent value is returned, otherwise a runime error 
 * is thrown. Exception message will then contain accepted strings for
 * halo command line parameter.
 *
 */
template < typename F, typename S, typename C, typename A >
S valueFromKey( const F& in, const std::map<F,S,C,A>& mymap){

  typename std::map< F, S, C, A >::const_iterator ht 
    = mymap.find( in );  

  if ( ht != mymap.end() )
    return ht->second;
  else
    throw std::runtime_error("can't find requested key");

}

struct case_insensitive_less : std::binary_function< std::string, std::string, bool>{
  struct case_insensitive_compare : std::binary_function< const unsigned char&, const unsigned char&, bool>{
    bool operator() (const unsigned char& a, const unsigned char& b) const {
      return tolower(a) < tolower(b);
    }
  };

  bool operator() ( const std::string& a, const std::string& b ) const {
    return std::lexicographical_compare( a.begin(), a.end(),
        b.begin(), b.end(),
        case_insensitive_compare());
  }
};


#endif // PARSING_HELPERS_HPP

