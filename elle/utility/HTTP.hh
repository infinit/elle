//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue nov 22 09:06:08 2011]
//

#ifndef ELLE_UTILITY_HTTP_HH
#define ELLE_UTILITY_HTTP_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Character.hh>
#include <elle/core/String.hh>
#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>

#include <elle/utility/URI.hh>

#include <elle/idiom/Close.hh>
# if defined(INFINIT_MACOSX)
   ///
   /// a bug exists in which, on MacOS X, curl includes <osreldate.h> if
   /// __FreeBSD__ is defined, which it has to be for FUSE.
   ///
   /// therefore, __FreeBSD__ is undefined before including <curl/curl.h>
   ///
#  undef __FreeBSD__
#  include <curl/curl.h>
#  define __FreeBSD__ 10
# else
#  include <curl/curl.h>
# endif
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
    /// this class provides easy-to-use methods for sending and receiving
    /// HTTP requests/responses.
    ///
    class HTTP
    {
    public:
      //
      // enumerations
      //
      enum Code
        {
          CodeOk = 200,

          CodeUnauthorized = 401,
          CodeForbidden = 403,
          CodeNotFound = 404,

          CodeInternalServerError = 500
        };

      //
      // types
      //
      typedef String                    Content;

      //
      // constants
      //
      static const String               Agent;

      struct                            Trash
      {
        static Content                  _Content;
        static Code                     _Code;
      };

      static Character                  Error[CURL_ERROR_SIZE];

      //
      // static methods
      //
      static Status     Get(const URI&,
                            String& = Trash::_Content,
                            Code& = Trash::_Code);
      static Status     Post(const URI&,
                             const String&,
                             const String&,
                             String& = Trash::_Content,
                             Code& = Trash::_Code);
      static Status     Put(const URI&,
                            const String&,
                            const String&,
                            String& = Trash::_Content,
                            Code& = Trash::_Code);
      static Status     Delete(const URI&,
                               String& = Trash::_Content,
                               Code& = Trash::_Code);

    private:
      //
      // static methods
      //
      static size_t     Read(void*,
                             size_t,
                             size_t,
                             void*);
      static size_t     Write(void*,
                              size_t,
                              size_t,
                              void*);
    };

  }
}

#endif
