#ifndef ELLE_HTTPCLIENT_HXX
# define ELLE_HTTPCLIENT_HXX

# include <elle/log.hh>
# include <elle/print.hh>

#include <elle/serialize/JSONArchive.hh>

namespace elle
{
  template<typename T>
  T
  HTTPClient::post(std::string const& url, elle::format::json::Object const& req)
  {
    ELLE_LOG_COMPONENT("elle.HTTPClient");

    std::stringstream res;

    // http request
    try
      { this->_request(url, "POST", req.repr(), res); }
    catch (std::exception const& err)
      {
        ELLE_TRACE("POST %s %s threw an error", url, req.repr());
        throw HTTPException(ResponseCode::internal_server_error, err.what());
      }

    T ret;

    ELLE_TRACE("POST %s %s => %s", url, req.repr(), res.str());

    // deserialize response
    try
      {
        elle::serialize::InputJSONArchive(res, ret);
      }
    catch (std::exception const& err)
      {
        throw HTTPException(ResponseCode::bad_content, err.what());
      }

    if (!ret.success() && this->_check_errors())
      throw HTTPException(ResponseCode::internal_server_error,
                            ret.response_details);

    return ret;
  }

  template<typename T>
  T
  HTTPClient::get(std::string const& url)
  {
    ELLE_LOG_COMPONENT("elle.HTTPClient");
    std::stringstream res;

// http request
    try
      { this->_request(url, "GET", "", res); }
    catch (std::runtime_error const& err)
      {
        ELLE_TRACE("GET %s threw an error", url);
        throw HTTPException(ResponseCode::internal_server_error, err.what());
      }

    T ret;

    ELLE_TRACE("GET %s => %s", url, res.str());

// deserialize response
    try
      {
        elle::serialize::InputJSONArchive(res, ret);
      }
    catch (std::exception const& err)
      {
        throw HTTPException(ResponseCode::bad_content, err.what());
      }

    if (!ret.success() && this->_check_errors())
      throw HTTPException(ResponseCode::internal_server_error,
                          ret.response_details);

    return ret;
  }

}

#endif
