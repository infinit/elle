//
// ---------- header ----------------------------------------------------------
//
// project       infinite
//
// license       GPL
//
// file          infinite/hole/sdht/Data.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef DATA_HH
#define DATA_HH

#include <string>

#include <elle/misc/Status.hh>

//
// ---------- namespace -------------------------------------------------------
//

namespace hole
{

  //
  // ---------- define ----------------------------------------------------
  //
 
  //
  // ---------- structures ------------------------------------------------------
  //
  
  ///
  /// This class represent de Data to be store in the DHT
  ///
  ///
    class Data
    {
    public:
      Data();
      Data(std::string);
      std::string		GetData();
      void			SetData(std::string	str);
   
      std::string		data_;
    };
  
    std::ostream&	operator <<  (std::ostream&, const Data&);
    
}
    
#endif
