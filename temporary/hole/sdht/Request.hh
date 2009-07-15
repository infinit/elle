//
// ---------- header ----------------------------------------------------------
//
// project       infinite
//
// license       GPL
//
// file          infinite/hole/sdht/Request.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef REQUEST_HH
#define REQUEST_HH

#include <list>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../Hole.hh"
#include "../Key.hh"
#include "../Map_Data.hh"
#include "Node.hh"

#include <elle/misc/Natural.hh>

//
// ---------- namespace -------------------------------------------------------
//

namespace hole
{
  ///
  /// This is the namespace of the Sdht implementation
  ///
  namespace sdht
  {


  //
  // ---------- define ----------------------------------------------------
  //
  
  //
  // ---------- structures ------------------------------------------------------
  //
    
    class Request
    {
    public: 
      typedef Natural(32)	t_id;
      typedef Natural(32)	t_type;
      
      Request();
      Request(const Node &, const t_type);
      Status		print() const;
      Key		getKey() const;
      
      enum
	{
	  UNINIT,
	  PING,
	  PONG,
	  LOOKUP
	};
      
      Port		port_;
      Ip		ip_;
      Key		data_key_;
      t_id		id_;
      t_type		type_;
      
      template <class Archive>
      void serialize(Archive &, const unsigned int &);
    };
    
    std::ostream&	operator <<  (std::ostream&, const Request&);
    
    struct cmp_id 
    {
      bool		 operator()(Request::t_id a, Request::t_id b) 
      {
	return (a < b);
      }
    };
    
    
  }
}



#include "Request.hxx"

#endif
