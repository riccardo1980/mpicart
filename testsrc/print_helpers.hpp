#ifndef PRINT_HELPERS_HPP
#define PRINT_HELPERS_HPP

#include <vector>
#include <map>
#include <string>

template <typename T>
class pretty {};

template <typename T>
pretty<T> make_pretty ( const T& in ){
    return pretty<T>( in );
}

template <typename T>
std::ostream& operator<< ( std::ostream& os, 
    const pretty< std::vector<T> >& data );

template <typename T>
class pretty< std::vector<T> > {
  public:
    explicit pretty( const std::vector<T>& data ) :
      _data( data ), _preamble(), _separator(), 
      _epilogue(), _showpos(false) {}

    friend 
      std::ostream& operator<<<> ( std::ostream& os,
          const pretty< std::vector<T> >& data );

    pretty< std::vector<T> >& preamble (const std::string& pre){
      _preamble = pre;
      return *this;
    } 

    pretty< std::vector<T> >& separator (const std::string& sep){
      _separator = sep;
      return *this;
    } 

    pretty< std::vector<T> >& epilogue (const std::string& epi){
      _epilogue = epi;
      return *this;
    } 

    pretty< std::vector<T> >& showpos (){
      _showpos = true;
      return *this; 
    } 

    pretty< std::vector<T> >& noshowpos (){
      _showpos = false;
      return *this; 
    } 

  private:
    std::string _preamble;
    std::string _separator;
    std::string _epilogue;
    bool _showpos;
    const std::vector<T>& _data;
};

template <typename T>
std::ostream& operator<< ( std::ostream& os, 
    const pretty< std::vector<T> >& data ){

  std::ios::fmtflags f ( os.flags() );

  if ( data._showpos )
    os << std::showpos;

  os << std::scientific;
  os << data._preamble;
  typename std::vector<T>::const_iterator it = data._data.begin(); 
  for ( ; it < data._data.end()-1; ++it )
   os << *it << data._separator;  

  os << *it << data._epilogue;
  os.flags( f );

  return os; 
}

template <typename F, typename S, typename C, typename A>
std::ostream& operator<< ( std::ostream& os, 
    const pretty< std::map<F,S,C,A> >& data );

template <typename F, typename S, typename C, typename A>
class pretty< std::map<F,S,C,A> > {

  public:
    explicit pretty ( const std::map<F,S,C,A>& data ) :
      _data( data ), _preamble(), _tuple_separator(),
      _keyval_separator(), 
      _epilogue(), _printval(false) {}

    friend
      std::ostream& operator<< <> ( std::ostream& os, 
          const pretty< std::map<F,S,C,A> >& data );  

    pretty< std::map<F,S,C,A> >& preamble (const std::string& pre){
      _preamble = pre;
      return *this;
    } 

    pretty< std::map<F,S,C,A> >& tuple_separator (const std::string& sep){
      _tuple_separator = sep;
      return *this;
    } 

    pretty< std::map<F,S,C,A> >& keyval_separator (const std::string& sep){
      _keyval_separator = sep;
      return *this;
    } 

    pretty< std::map<F,S,C,A> >& epilogue (const std::string& epi){
      _epilogue = epi;
      return *this;
    } 

    pretty< std::map<F,S,C,A> >& printval (){
      _printval = true;
      return *this; 
    } 

    pretty< std::map<F,S,C,A> >& noprintval (){
      _printval = false;
      return *this; 
    } 

  private:
    std::string _preamble;
    std::string _tuple_separator;
    std::string _keyval_separator;
    std::string _epilogue;
    bool _printval;
    const std::map<F,S,C,A>& _data;

};

template <typename F, typename S, typename C, typename A>
std::ostream& operator<< ( std::ostream& os, 
    const pretty< std::map<F,S,C,A> >& data ){

  typename std::map< F,S,C,A >::const_iterator it 
    = data._data.begin(); 
  typename std::map< F,S,C,A >::const_iterator it_end 
    = data._data.end(); 

  os << data._preamble << it->first;
  if (data._printval ) 
    os << data._keyval_separator << it->second; 
  ++it; 
  for( ; it != it_end; ++it ){
    os << data._tuple_separator << it->first;
    if (data._printval ) 
      os << data._keyval_separator << it->second; 
  }

  os << data._epilogue;

  return os;

}  

#endif // PRINT_HELPERS_HPP

