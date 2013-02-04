#if 0 // XXX[no longer used?]
#include <elle/utility/HTTP.hh>

#include <elle/types.hh>

#include <elle/standalone/Region.hh>

#include <elle/utility/URI.hh>

namespace elle
{
  namespace utility
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the name of the virtual agent.
    ///
    const String                HTTP::Agent("elle");

    ///
    /// this default response string is used whenever the caller is not
    /// interested in the response.
    ///
    String                      HTTP::Trash::_Content;

    ///
    /// this default code is used whenever the caller is not interested
    /// in the returned code.
    ///
    HTTP::Code                  HTTP::Trash::_Code;

    ///
    /// this buffer is used by curl to specify errors.
    ///
    Character                   HTTP::Error[CURL_ERROR_SIZE];

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method performs a GET request on the resource given by _uri_.
    ///
    Status              HTTP::Get(const URI&                    uri,
                                  Content&                      response,
                                  Code&                         code)
    {
      standalone::Region output;
      ::CURL*           curl;
      long              r;

      // initialize curl.
      if ((curl = ::curl_easy_init()) == nullptr)
        goto _error;

      // set the agent.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_USERAGENT,
                             HTTP::Agent.c_str()) != ::CURLE_OK)
        goto _error;

      // set the query.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_URL,
                             uri.c_str()) != ::CURLE_OK)
        goto _error;

      // set the write callback.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_WRITEFUNCTION,
                             &HTTP::Write) != ::CURLE_OK)
        goto _error;

      // set the callback data.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_WRITEDATA,
                             &output) != ::CURLE_OK)
        goto _error;

      // perform the operation.
      if (::curl_easy_perform(curl) != 0)
        goto _error;

      // append the output's content to the response string.
      response.assign(reinterpret_cast<const char*>(output.contents),
                      static_cast<size_t>(output.size));

      // retrieve the code.
      if (::curl_easy_getinfo(curl,
                              ::CURLINFO_RESPONSE_CODE,
                              &r) != ::CURLE_OK)
        goto _error;

      // retrieve the HTTP code.
      code = static_cast<HTTP::Code>(r);

      // clean up the session.
      ::curl_easy_cleanup(curl);

      return Status::Ok;

    _error:
      // clean up the session.
      ::curl_easy_cleanup(curl);

      throw Exception("%s", HTTP::Error);
    }

    ///
    /// this method performs a POST request on the resource given by _uri_.
    ///
    Status              HTTP::Post(const URI&                   uri,
                                   const String&                type,
                                   const Content&               request,
                                   Content&                     response,
                                   Code&                        code)
    {
      standalone::Region input(reinterpret_cast<const Byte*>(request.c_str()),
                               request.length());
      standalone::Region output;
      String            header = "Content-Type: " + type;
      ::CURL*           curl;
      long              r;

      // initialize curl.
      if ((curl = ::curl_easy_init()) == nullptr)
        goto _error;

      // set the agent.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_USERAGENT,
                             HTTP::Agent.c_str()) != ::CURLE_OK)
        goto _error;

      // set the query.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_URL,
                             uri.c_str()) != ::CURLE_OK)
        goto _error;

      // specify that we want to perform a POST.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_POST,
                             1L) != ::CURLE_OK)
        goto _error;

      // set POST fields.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_POSTFIELDS,
                             request.c_str()) != ::CURLE_OK)
        goto _error;

      // set the POST fields' size.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_POSTFIELDSIZE,
                             request.size()) != ::CURLE_OK)
        goto _error;

      //
      // set the write callback.
      //
      {
        // set the write callback.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_WRITEFUNCTION,
                               &HTTP::Write) != CURLE_OK)
          goto _error;

        // set the callback data.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_WRITEDATA,
                               &output) != CURLE_OK)
          goto _error;
      }

      //
      // set the header
      //
      {
        ::curl_slist*   headerlist;

        // append the content-type to the header list.
        if ((headerlist = ::curl_slist_append(nullptr,
                                              header.c_str())) == nullptr)
          goto _error;

        // set the list as representing the header.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_HTTPHEADER,
                               headerlist) != CURLE_OK)
          goto _error;
      }

      // perform the operation.
      if (::curl_easy_perform(curl) != 0)
        goto _error;

      // append the output's content to the response string.
      response.assign(reinterpret_cast<const char*>(output.contents),
                      static_cast<size_t>(output.size));

      // retrieve the code.
      if (::curl_easy_getinfo(curl,
                              ::CURLINFO_RESPONSE_CODE,
                              &r) != ::CURLE_OK)
        goto _error;

      // retrieve the HTTP code.
      code = static_cast<HTTP::Code>(r);

      // clean up the session.
      ::curl_easy_cleanup(curl);

      return Status::Ok;

    _error:
      // clean up the session.
      ::curl_easy_cleanup(curl);

      throw Exception("%s", HTTP::Error);
    }

    ///
    /// this method performs a PUT request on the resource given by _uri_
    ///
    Status              HTTP::Put(const URI&                    uri,
                                  const String&                 type,
                                  const Content&                request,
                                  Content&                      response,
                                  Code&                         code)
    {
      standalone::Region input(reinterpret_cast<const Byte*>(request.c_str()),
                               request.length());
      standalone::Region output;
      String            header = "Content-Type: " + type;
      ::CURL*           curl;
      long              r;

      // initialize curl.
      if ((curl = ::curl_easy_init()) == nullptr)
        goto _error;

      // set the agent.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_USERAGENT,
                             HTTP::Agent.c_str()) != ::CURLE_OK)
        goto _error;

      // set the query.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_URL,
                             uri.c_str()) != ::CURLE_OK)
        goto _error;

      // specify that we want to perform a PUT.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_PUT,
                             1L) != ::CURLE_OK)
        goto _error;

      // also specify that we want to upload some content.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_UPLOAD,
                             1L) != ::CURLE_OK)
        goto _error;

      //
      // set the read callback.
      //
      {
        // set the read callback.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_READFUNCTION,
                               &HTTP::Read) != CURLE_OK)
          goto _error;

        // set the callback data.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_READDATA,
                               &input) != CURLE_OK)
          goto _error;
      }

      //
      // set the write callback.
      //
      {
        // set the write callback.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_WRITEFUNCTION,
                               &HTTP::Write) != CURLE_OK)
          goto _error;

        // set the callback data.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_WRITEDATA,
                               &output) != CURLE_OK)
          goto _error;
      }

      // XXX move this up! would be better with after CURLOPT_UPLOAD
      // set the upload size.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_INFILESIZE,
                             static_cast<long>(request.size())) != CURLE_OK)
        goto _error;

      //
      // set the header
      //
      {
        ::curl_slist*   headerlist;

        // append the content-type to the header list.
        if ((headerlist = ::curl_slist_append(nullptr,
                                              header.c_str())) == nullptr)
          goto _error;

        // set the list as representing the header.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_HTTPHEADER,
                               headerlist) != CURLE_OK)
          goto _error;
      }

      // perform the operation.
      if (::curl_easy_perform(curl) != 0)
        goto _error;

      // append the output's content to the response string.
      response.assign(reinterpret_cast<const char*>(output.contents),
                      static_cast<size_t>(output.size));

      // retrieve the code.
      if (::curl_easy_getinfo(curl,
                              ::CURLINFO_RESPONSE_CODE,
                              &r) != ::CURLE_OK)
        goto _error;

      // retrieve the HTTP code.
      code = static_cast<HTTP::Code>(r);

      // clean up the session.
      ::curl_easy_cleanup(curl);

      return Status::Ok;

    _error:
      // clean up the session.
      ::curl_easy_cleanup(curl);

      throw Exception("%s", HTTP::Error);
    }

    ///
    /// this method performs a DELETE request on the resource given by _uri_
    ///
    Status              HTTP::Delete(const URI&                 uri,
                                     Content&                   response,
                                     Code&                      code)
    {
      standalone::Region output;
      ::CURL*           curl;
      long              r;

      // initialize curl.
      if ((curl = ::curl_easy_init()) == nullptr)
        goto _error;

      // set the agent.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_USERAGENT,
                             HTTP::Agent.c_str()) != ::CURLE_OK)
        goto _error;

      // set the query.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_URL,
                             uri.c_str()) != ::CURLE_OK)
        goto _error;

      // set the custom HTTP request.
      if (::curl_easy_setopt(curl,
                             ::CURLOPT_CUSTOMREQUEST,
                             "DELETE") != ::CURLE_OK)
        goto _error;

      //
      // set the write callback.
      //
      {
        // set the write callback.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_WRITEFUNCTION,
                               &HTTP::Write) != CURLE_OK)
          goto _error;

        // set the callback data.
        if (::curl_easy_setopt(curl,
                               ::CURLOPT_WRITEDATA,
                               &output) != CURLE_OK)
          goto _error;
      }

      // perform the operation.
      if (::curl_easy_perform(curl) != 0)
        goto _error;

      // append the output's content to the response string.
      response.assign(reinterpret_cast<const char*>(output.contents),
                      static_cast<size_t>(output.size));

      // retrieve the code.
      if (::curl_easy_getinfo(curl,
                              ::CURLINFO_RESPONSE_CODE,
                              &r) != ::CURLE_OK)
        goto _error;

      // retrieve the HTTP code.
      code = static_cast<HTTP::Code>(r);

      // clean up the session.
      ::curl_easy_cleanup(curl);

      return Status::Ok;

    _error:
      // clean up the session.
      ::curl_easy_cleanup(curl);

      throw Exception("%s", HTTP::Error);
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method is used as a callback whenever curl needs to read more
    /// HTTP input from the calling application.
    ///
    size_t              HTTP::Read(void*                        ptr,
                                   size_t                       size,
                                   size_t                       nmemb,
                                   void*                        userdata)
    {
      standalone::Region* region;

      // retrieve the region.
      region = reinterpret_cast<standalone::Region*>(userdata);

      // compute the size.
      size_t            s =
        (static_cast<size_t>(region->size) < (size * nmemb)) ?
        static_cast<size_t>(region->size) :
        (size * nmemb);

      // copy the data.
      ::memcpy(ptr,
               reinterpret_cast<const void*>(region->contents),
               s);

      // update the region.
      region->size -= s;
      region->contents += s;

      return s;
    }

    ///
    /// this method is used as a callback by curl in order to write the
    /// HTTP output in an application-specific buffer.
    ///
    size_t              HTTP::Write(void*                       ptr,
                                    size_t                      size,
                                    size_t                      nmemb,
                                    void*                       userdata)
    {
      standalone::Region* region;

      // cast the userdata.
      region = reinterpret_cast<standalone::Region*>(userdata);

      // append the given data to the region.
      if (region->Append(reinterpret_cast<const Byte*>(ptr),
                         static_cast<Natural64>(size * nmemb)) == Status::Error)
        return (0);

      return (size * nmemb);
    }

  }
}
#endif
