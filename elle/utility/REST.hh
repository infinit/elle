#ifndef ELLE_UTILITY_REST_HH
#define ELLE_UTILITY_REST_HH

//#include <elle/types.hh>
//
//
//#include <elle/utility/URI.hh>
//#include <elle/utility/HTTP.hh>
//#include <elle/utility/JSON.hh>
//
//#include <elle/idiom/Close.hh>
//# include <curl/curl.h>
//#include <elle/idiom/Open.hh>

namespace elle
{

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
      ////
      //// constants
      ////
      //struct                    Trash
      //{
      //  static JSON::Document   _Document;
      //};

      ////
      //// static methods
      ////
      //static Status     Get(const URI&,
      //                      JSON::Document&,
      //                      HTTP::Code& = HTTP::Trash::_Code);
      //static Status     Post(const URI&,
      //                       const JSON::Document&,
      //                       JSON::Document& = Trash::_Document,
      //                       HTTP::Code& = HTTP::Trash::_Code);
      //static Status     Put(const URI&,
      //                      const JSON::Document&,
      //                      JSON::Document& = Trash::_Document,
      //                      HTTP::Code& = HTTP::Trash::_Code);
      //static Status     Delete(const URI&,
      //                         JSON::Document& = Trash::_Document,
      //                         HTTP::Code& = HTTP::Trash::_Code);
    };

  }
}

#endif
