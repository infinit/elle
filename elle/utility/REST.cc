//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien.quintard   [wed nov 23 11:24:19 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/REST.hh>

namespace elle
{
  namespace utility
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable is used whenever the caller is not interested in
    /// retrieving the response-specific JSON document.
    ///
    JSON::Document		REST::Trash::_Document;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method performs a GET request on the resource given by _uri_.
    ///
    Status		REST::GET(const URI&			uri,
				  JSON::Document&		response,
				  HTTP::Code&			code)
    {
      HTTP::Content	content;

      enter();

      // request the HTTP GET method.
      if (HTTP::GET(uri,
		    content, code) == StatusError)
	escape("unable to GET the given URI");

      // decode the JSON document based on the given content.
      if (JSON::Decode(content, response) == StatusError)
	escape("unable to decode the response");

      leave();
    }

    ///
    /// this method performs a POST request on the resource given by _uri_.
    ///
    Status		REST::POST(const URI&			uri,
				   const JSON::Document&	request,
				   JSON::Document&		response,
				   HTTP::Code&			code)
    {
      HTTP::Content	content;
      String		string;

      enter();

      // encode the request into a string.
      if (JSON::Encode(request, string) == StatusError)
	escape("unable to encore the request");

      // request the HTTP POST method.
      if (HTTP::POST(uri,
		     JSON::Document::Type, string,
		     content, code) == StatusError)
	escape("unable to POST the given URI");

      // decode the JSON document based on the given content.
      if (JSON::Decode(content, response) == StatusError)
	escape("unable to decode the response");

      leave();
    }

    ///
    /// this method performs a PUT request on the resource given by _uri_
    ///
    Status		REST::PUT(const URI&			uri,
				  const JSON::Document&		request,
				  JSON::Document&		response,
				  HTTP::Code&			code)
    {
      HTTP::Content	content;
      String		string;

      enter();

      // encode the request into a string.
      if (JSON::Encode(request, string) == StatusError)
	escape("unable to encore the request");

      // request the HTTP PUT method.
      if (HTTP::PUT(uri,
		    JSON::Document::Type, string,
		    content, code) == StatusError)
	escape("unable to PUT the given URI");

      // decode the JSON document based on the given content.
      if (JSON::Decode(content, response) == StatusError)
	escape("unable to decode the response");

      leave();
    }

    ///
    /// this method performs a DELETE request on the resource given by _uri_
    ///
    Status		REST::DELETE(const URI&			uri,
				     JSON::Document&		response,
				     HTTP::Code&		code)
    {
      HTTP::Content	content;

      enter();

      // request the HTTP DELETE method.
      if (HTTP::DELETE(uri,
		       content, code) == StatusError)
	escape("unable to DELETE the given URI");

      // decode the JSON document based on the given content.
      if (JSON::Decode(content, response) == StatusError)
	escape("unable to decode the response");

      leave();
    }

  }
}
