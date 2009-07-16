//
// ---------- header ----------------------------------------------------------
//
// project       infinite
//
// license       GPL
//
// file          infinite/hole/sdht/Key.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef KEY_HH
#define KEY_HH

#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <elle/misc/Natural.hh>
#include <elle/misc/Integer.hh>
#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>
#include <elle/misc/Network.hh>


//
// ---------- namespace -------------------------------------------------------
//
using namespace elle;
using namespace elle::misc;

namespace hole
{
  
  //
  // ---------- define ----------------------------------------------------
  //
  /// KEYLEN is the len of the key, it cannot be lower than 20 (SHA1 len)
#define KEYLEN 20
  //
  // ---------- structures ------------------------------------------------------
  //
  
  
  ///
  /// This class represent a KEY or Index of the dht
  /// where you store datas 
  ///
  ///    
    class Key
    {
    public:
      Key(const Ip&, const Port&);
      Key();
      Key(unsigned char);
      Natural(16)	to_natural16() const;
      Integer(8)	compar(const Key&) const;
      bool		operator == (const Key&) const;
      bool		operator != (const Key&) const;
      bool		operator < (const Key&) const;
      bool		operator <= (const Key&) const;
      bool		operator > (const Key&) const;
      bool		operator >= (const Key&) const;
      Key		operator + (const Key&) const;
      Key		operator - (const Key&) const;
      Key		operator * (const unsigned char) const;
      std::string	to_string() const;
      Status		print() const;
      
      template <class Archive>
      void serialize(Archive &, const unsigned int &);
      

      unsigned char	k_[KEYLEN];
   };
    
    
    std::ostream&	operator <<  (std::ostream&, const Key&);
    Integer(8)		compar(const Key&, const Key&);
    
    struct cmp_key 
    {
      bool		 operator()(Key a, Key b) 
      {
	return (compar(a, b) < 0);
      }
    };
   
}

#include "Key.hxx"
    
#endif
