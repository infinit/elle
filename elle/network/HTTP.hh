//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien.quintard   [tue nov 22 09:06:08 2011]
//

#ifndef ELLE_NETWORK_HTTP_HH
#define ELLE_NETWORK_HTTP_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Character.hh>
#include <elle/core/String.hh>
#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>

#include <elle/network/URI.hh>

#include <elle/idiom/Close.hh>
# include <curl/curl.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides easy-to-use HTTP-specific GET/POST/PUT/DELETE
    /// methods.
    ///
    class HTTP
    {
    public:
      //
      // constants
      //
      static const String		Agent;

      static String			Response;
      static Natural32			Code;

      static Character			Error[CURL_ERROR_SIZE];

      //
      // static methods
      //
      static Status	GET(const URI&,
			    String& = Response,
			    Natural32& = Code);
      static Status	POST(const URI&,
			     const String&,
			     const String&,
			     String& = Response,
			     Natural32& = Code);
      static Status	PUT(const URI&,
			    const String&,
			    const String&,
			    String& = Response,
			    Natural32& = Code);
      static Status	DELETE(const URI&,
			       String& = Response,
			       Natural32& = Code);

    private:
      //
      // static methods
      //
      static size_t	Read(void*,
			     size_t,
			     size_t,
			     void*);
      static size_t	Write(void*,
			      size_t,
			      size_t,
			      void*);
    };

  }
}

#endif
