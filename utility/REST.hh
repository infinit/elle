//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed nov 23 10:53:53 2011]
//

#ifndef ELLE_UTILITY_REST_HH
#define ELLE_UTILITY_REST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Character.hh>
#include <elle/core/String.hh>
#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>

#include <elle/utility/URI.hh>
#include <elle/utility/HTTP.hh>
#include <elle/utility/JSON.hh>

#include <elle/idiom/Close.hh>
# include <curl/curl.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace utility
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides easy-to-use REST-specific GET/POST/PUT/DELETE
    /// methods.
    ///
    class REST
    {
    public:
      //
      // constants
      //
      struct			Trash
      {
	static JSON::Document	_Document;
      };

      //
      // static methods
      //
      static Status	Get(const URI&,
			    JSON::Document&,
			    HTTP::Code& = HTTP::Trash::_Code);
      static Status	Post(const URI&,
			     const JSON::Document&,
			     JSON::Document& = Trash::_Document,
			     HTTP::Code& = HTTP::Trash::_Code);
      static Status	Put(const URI&,
			    const JSON::Document&,
			    JSON::Document& = Trash::_Document,
			    HTTP::Code& = HTTP::Trash::_Code);
      static Status	Delete(const URI&,
			       JSON::Document& = Trash::_Document,
			       HTTP::Code& = HTTP::Trash::_Code);
    };

  }
}

#endif
